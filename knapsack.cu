#include <cuda.h>
#include <cuda_runtime.h>
#include <cooperative_groups.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <ctime>

#include "knapsack.h"
#include "timer.h"
#include "stock.h"

extern bool verbose;

namespace cg = cooperative_groups;

void allocateItems(const std::vector<Stock>& stocks,
  const int* stock_values,
  int*& item_costs, int*& item_values, int*& item_quantities,
  unsigned short*& chosen) {
  
  cudaMallocManaged(&item_costs, stocks.size() * sizeof(int));
  cudaMallocManaged(&item_values, stocks.size() * sizeof(int));
  cudaMallocManaged(&item_quantities, stocks.size() * sizeof(int));
  cudaMallocManaged(&chosen, stocks.size() * sizeof(unsigned short));
  for(size_t i = 0; i < stocks.size(); ++i) {
    int w = stocks[i].getPrice();
    int v = stock_values[i];
    int q = stocks[i].getQuantity();
    item_costs[i] = w;
    item_values[i] = v;
    item_quantities[i] = q;
    chosen[i] = 0;
  }
}

bool allocateTables(int*& table, int*& pointers, int*& quants,
  const size_t num_items, const int budget) {
  const size_t size = (size_t)((num_items+1) * ((unsigned long)budget + 1));

  cudaError_t err1 = cudaMalloc(&table, size * sizeof(int));
  if(err1 != cudaSuccess) {
    printf("Table alloc failed\n");
    return false;
  }
  cudaError_t err2 = cudaMallocManaged(&pointers, size * sizeof(int));
  if(err2 != cudaSuccess) {
    printf("Pointer alloc failed\n");
    cudaFree(table);
    return false;
  }
 
  cudaError_t err3 = cudaMallocManaged(&quants, size * sizeof(int));
  if(err3 != cudaSuccess) {
    printf("Pointer alloc failed\n");
    cudaFree(table);
    cudaFree(pointers);
    return false;
  }

  if(verbose) {
    printf("Total Table memory: %lu\n", 3 * size * sizeof(int));
  }

  return false;
}

void freeTable(int* table, int* pointers, int* quants) {
  cudaFree(table);
  cudaFree(pointers);
  cudaFree(quants);
}

void freeItems(int* item_costs, int* item_values, int* item_quantities,
  unsigned short* chosen) {

  cudaFree(chosen);
  cudaFree(item_costs);
  cudaFree(item_values);
  cudaFree(item_quantities);
}

__global__ void knapsackKernel(
  const int* item_costs,
  const int* item_values,
  const int* item_quantities,
  int* table,
  int* pointers,
  int* quants,
  const size_t num_items,
  const int offset,
  const int budget) {

  cg::grid_group grid = cg::this_grid();
  const int w = blockIdx.x * blockDim.x + threadIdx.x + offset;
  const int cols = budget+1;

  if(w <= num_items) {
    table[w * cols + 0] = 0;
    pointers[w * cols + 0] = 0;
    quants[w * cols + 0] = 0;
  }

  if(w <= budget) {
    table[w] = 0;
    pointers[w] = 0;
    quants[w] = 0;
  }
  
  grid.sync();

  for(int i = 1; i <= num_items; ++i) {
    if(w <= budget) {
      int a = table[(i-1)*cols + w];
      int cost = item_costs[i-1];
      int value = item_values[i-1];
      int quantity = item_quantities[i-1];
      int last = w;
      int quant = 0;
      
      if(value > 0) {
        for(int q = 1; q <= quantity && q * cost <= w; ++q) {
          int val = table[(i-1)*cols + (w - q * cost)] + (q * value);
          if(val > a) {
            a = val;
            last = (w - q * cost);
            quant = q;
          }
        }
      }

      table[i*cols + w] = a;
      pointers[i*cols + w] = last;
      quants[i*cols + w] = quant;
    }
    grid.sync();
  }
}

__global__ void pullValue(int* table, int* val, size_t idx) {
  if(blockIdx.x * blockDim.x * threadIdx.x == 0) {
    *val = table[idx];
  }
}

__global__ void getChosenStocks(int* table, int* pointers, int* quants,
  int* item_costs, int* item_values, int* item_quantities,
  unsigned short* chosen, int num_items, int budget, int result) {

  if(blockIdx.x + threadIdx.x == 0) {
    int cols = budget+1;
    int w = budget;
    for(int i = num_items; i > 0 && result > 0; --i) {
      chosen[i-1] = quants[i*cols + w];
      w = pointers[i*cols + w];
      result = table[(i-1)*cols] + w;
    }
  }
}

void knapsack(const std::vector<Stock>& stocks,
  const int* stock_values,
  std::vector< std::vector<int> >& solution,
  int& total, size_t num_items, int budget, double* timer) {

  int* val;
  unsigned short* chosen;
  int* table;
  int* pointers;
  int* quants;
  int offset = 0;

  assert(num_items == stocks.size());

  cudaMallocManaged(&val, 1 * sizeof(int));
  *val = 0;

  int* item_costs;
  int* item_values;
  int* item_quantities;

  //Allocate dedicated and shared memory for GPU usage
  allocateItems(stocks, stock_values, item_costs, item_values, item_quantities, chosen);
  allocateTables(table, pointers, quants, num_items, budget);

  //Args for knapsack kernel
  void* args[] = {
    &item_costs,
    &item_values,
    &item_quantities,
    &table,
    &pointers,
    &quants,
    &num_items,
    &offset,
    &budget
  };  

  //Dimensions for kernel
  const unsigned int max_blocks = 120;
  const unsigned int work_per_call = max_blocks * 1024;

  //We may not be able to run knapsack on the whole table at once
  //So, we run on "strips" of the table, which achieves the same result
  auto start = clock_time();
  for(int i = 0; i < budget; i += work_per_call) {
    offset = i;
    if(verbose) {
      printf("work done so far: %7d | launching %d total threads\n", i, work_per_call);
    }
    dim3 dimGrid(max_blocks, 1, 1);
    dim3 dimBlock(1024, 1, 1);
    //Launch cooperative kernel to ensure that all threads can be synchronized together
    cudaLaunchCooperativeKernel((void*)knapsackKernel, dimGrid, dimBlock, args);
  }
  pullValue<<< 1, 1 >>>(table, val, num_items*(budget+1) + budget);
  cudaDeviceSynchronize();

  int v = *val;
  if(verbose) {
    printf("Total value is %d, fetching solution\n", v);
  }

  //The "backtrack" step of knapsack
  //Since the table is in GPU dedicated memory, we launch a kernel with thread
  getChosenStocks<<< 1, 1 >>>(table, pointers, quants, item_costs, item_values, item_quantities,
          chosen, num_items, budget, v);
  cudaDeviceSynchronize();
  auto end = clock_time();
  if(timer != NULL) *timer = calc_time(start, end);
  if(verbose) {
    printf("Fetched solution\n");
  }

  int total_weight = 0;
  int total_value = 0;

  //Move solution into a vector
  solution.clear();
  for(int i = 0; i < num_items; ++i) {
    if(chosen[i] == 0) {
      continue;
    }

    solution.push_back({stocks[i].getID(), item_costs[i], chosen[i]});

    total_weight += chosen[i] * item_costs[i];
    total_value += chosen[i] * item_values[i];
    if(verbose) {
      printf("Chose %d of stock #%d\n", chosen[i], i+1);
    }
  }

  if(total_weight > budget || total_value != v) {
    printf("Error with knapsack\n");
  }

  //Free memory
  freeItems(item_costs, item_values, item_quantities, chosen);
  freeTable(table, pointers, quants);
  cudaFree(val);

  total = v;
}

bool mapRankToGPU(int myrank) {
  int cudaDeviceCount, cE;

  if( (cE = cudaGetDeviceCount( &cudaDeviceCount)) != cudaSuccess ) {
    printf(" Unable to determine cuda device count, error is %d, count is %d\n", cE, cudaDeviceCount );
    return false;
  }

  if( (cE = cudaSetDevice( myrank % cudaDeviceCount )) != cudaSuccess ) {
    printf(" Unable to have gpu rank %d set to cuda device %d, error is %d \n", myrank, (myrank % cudaDeviceCount), cE);
    return false;
  }

  if(verbose) {
    printf("Mapping gpu rank %d to CUDA device %d\n", myrank, (myrank % cudaDeviceCount));
  }

  return true;
}
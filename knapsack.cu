#include <cuda.h>
#include <cuda_runtime.h>
#include <cooperative_groups.h>
#include <vector>
#include <iostream>
#include <random>
#include <ctime>

#include "stock.h"

namespace cg = cooperative_groups;


__device__ unsigned int knapsackBarrier = 0;

// addr must be aligned properly.
__device__ unsigned int atomicLoad(const unsigned int *addr)
{
  const volatile unsigned int *vaddr = addr; // volatile to bypass cache
  __threadfence(); // for seq_cst loads. Remove for acquire semantics.
  const unsigned int value = *vaddr;
  // fence to ensure that dependent reads are correctly ordered
  __threadfence(); 
  return value; 
}

// addr must be aligned properly.
__device__ void atomicStore(unsigned int *addr, unsigned int value)
{
  volatile unsigned int *vaddr = addr; // volatile to bypass cache
  // fence to ensure that previous non-atomic stores are visible to other threads
  __threadfence(); 
  *vaddr = value;
}

void allocateItems(const std::vector<Stock>& stocks,
                    int*& item_costs, int*& item_values, int*& item_quantities,
                    unsigned short*& chosen) {
  
  cudaMallocManaged(&item_costs, stocks.size() * sizeof(int));
  cudaMallocManaged(&item_values, stocks.size() * sizeof(int));
  cudaMallocManaged(&item_quantities, stocks.size() * sizeof(int));
  cudaMallocManaged(&chosen, stocks.size() * sizeof(unsigned short));
  for(size_t i = 0; i < stocks.size(); ++i) {
    int w = stocks[i].getPrice();
    int v = stocks[i].expectedValue();
    int q = stocks[i].getQuantity();
    item_costs[i] = w;
    item_values[i] = v;
    item_quantities[i] = q;
    chosen[i] = 0;
  }
}

void allocateTable(int*& table, const size_t num_items, const int budget) {
  cudaMalloc(&table, (num_items+1) * (budget + 1) * sizeof(int));
}

void freeTable(int* table) {
  cudaFree(table);
}

void freeItems(int* item_costs, int* item_values, int* item_quantities,
               unsigned short* chosen) {
  cudaFree(chosen);
  cudaFree(item_costs);
  cudaFree(item_values);
  cudaFree(item_quantities);
}

__global__ void knapsackKernel(int* item_costs,
                         int* item_values,
                         int* item_quantities,
                         unsigned short* chosen,
                         int* table,
                         size_t num_items,
                         int budget) {

  cg::grid_group grid = cg::this_grid();
  const int w = blockIdx.x * blockDim.x + threadIdx.x;

  
  const int rows = num_items+1;
  const int cols = budget+1;
  //table[item][cost] = table[item*cols + cost];
  if(w <= num_items) {
    table[w * cols + 0] = 0;
  }

  // __syncthreads();

  if(w <= budget) {
    table[w] = 0;
  }
  
  // __syncthreads();
  grid.sync();

  if(w <= budget) {
    for(int i = 1; i <= num_items; ++i) {
      table[i*cols + w] = table[(i-1)*cols + w];
      int a = table[i*cols + w];
      for(int q = 1; q <= item_quantities[i-1] && q * item_costs[i-1] <= w; ++q) {
        int val = table[(i-1)*cols + (w - q * item_costs[i-1])] + (q * item_values[i-1]);
        if(val > a) {
          a = val;
        }
      }
      table[i*cols + w] = a;
      // __syncthreads();
      grid.sync();
    }
  }
}

__global__ void pullValue(int* table, int* val, size_t idx) {
  if(blockIdx.x * blockDim.x * threadIdx.x == 0) {
    *val = table[idx];
  }
}

int knapsack(std::vector<Stock>& stocks, size_t num_items, int budget) {

  int* val;
  unsigned short* chosen;
  int* table;
  cudaMallocManaged(&val, 1 * sizeof(int));
  *val = 0;

  int* item_weights;
  int* item_values;
  int* item_quantities;

  allocateItems(stocks, item_weights, item_values, item_quantities, chosen);
  allocateTable(table, num_items, budget);
  void* args[] = {
    &item_weights,
    &item_values,
    &item_quantities,
    &chosen,
    &table,
    &num_items,
    &budget
  };
  for(int i = 1; i < 8192; ++i) {
    if(budget < (i*1024)) {
      dim3 dimGrid(i, 1, 1);
      printf("Launching %d blocks\n", i);
      dim3 dimBlock(1024, 1, 1);
      cudaLaunchCooperativeKernel((void*)knapsackKernel, dimGrid, dimBlock, args);
      cudaDeviceSynchronize();
      // knapsackKernel<<< i, 1024 >>>(item_weights, item_values, item_quantities, chosen, table, num_items, budget);
      break;
    }
  }

  pullValue<<< 1, 1 >>>(table, val, num_items*(budget+1) + budget);

  cudaDeviceSynchronize();
  int v = *val;

  freeItems(item_weights, item_values, item_quantities, chosen);
  freeTable(table);
  cudaFree(val);

  return v;
}

void generate_test(std::vector<Stock>& stocks) {
  stocks.push_back(Stock(0, 10, 5, std::vector<std::pair<int, int>>(1, {100, 60})));
  stocks.push_back(Stock(0, 20, 4, std::vector<std::pair<int, int>>(1, {100, 100})));
  stocks.push_back(Stock(0, 30, 2, std::vector<std::pair<int, int>>(1, {100, 120})));
  srand(time(NULL));
  for(int i = 0; i < 0; ++i) {
    int w = rand() % 71 + 70;
    int v = rand() % 24 - 9;
    int q = rand() % 340;
    if(q) {
      // printf("%d %d %d\n", w, v, q);
    }
    stocks.push_back(Stock(0, w, q, std::vector<std::pair<int, int>>(1, {100, v})));
  }
}

int main() {

  std::vector<Stock> stocks;
  generate_test(stocks);
  int v = knapsack(stocks, stocks.size(), 165000);
  std::cout << v << std::endl;

  return 0;
}
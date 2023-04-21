#ifndef SIM_H
#define SIM_H

#define CPU_COUNT_ 25
#define GPU_COUNT_ 6
#define ITEM_SIZE 25
#define ROUNDS 1

#include <vector>
#include "clockcycle.h"

extern bool verbose;

typedef struct env_t {
  int globalrank, localrank, num_ranks, num_nodes;
  int cpu, gpu, worker;
  int cpu_rank, gpu_rank, worker_rank, node;
  int num_cpus, num_gpus;
} env_t;

double calc_time(int start, int end) {
  return (double)(end - start) / 512000000.0;
}

void read_file(const env_t* env, std::vector<Stock>& stocks, MPI_File& fh) {
  if(env->cpu) {
    const int count = 100000 / env->num_cpus; // 500
    int* buffer = new int[count * 25];
    MPI_Offset start = env->cpu_rank * (count * 25);
    MPI_Status status;

    MPI_File_read_at(fh, start, buffer, 25 * count, MPI_INT, &status);

    for(int i = 0; i < count; ++i) {
      stocks.push_back(Stock(buffer + i*25));
    }

    delete [] buffer;
  }
}

void msg_1_cpu_to_gpu(const env_t* env, int*& buffer, double& timer) {
  const int items_per_cpu = 100000 / (CPU_COUNT_ * env->num_nodes);
  int start = clock_now();
  if(env->cpu) {
    MPI_Request* requests = new MPI_Request[env->num_gpus];
    for(int n = 0; n < env->num_nodes; ++n) {
      for(int l = 0; l < GPU_COUNT_; ++l) {
        int gpu_rank = (GPU_COUNT_ * n) + l;
        MPI_Isend(buffer, items_per_cpu * ITEM_SIZE, MPI_INT, n*32 + l, 0, MPI_COMM_WORLD, requests + gpu_rank);
      }
    }

    MPI_Waitall(env->num_gpus, requests, MPI_STATUSES_IGNORE);
    
  } else if(env->gpu) {
    //Receive
    MPI_Request* requests = new MPI_Request[env->num_cpus];
    MPI_Status* statuses = new MPI_Status[env->num_cpus];

    //New buffer size is (total cpu count) * (items per cpu) * (size of item buffer)
    buffer = new int[env->num_cpus * items_per_cpu * ITEM_SIZE];
    for(int n = 0; n < env->num_nodes; ++n) {
      for(int l = 0; l < 32; ++l) {
        if(l < GPU_COUNT_ || l >= CPU_COUNT_ + GPU_COUNT_) {
          continue;
        }
        int cpu_rank = (CPU_COUNT_ * n) + (l - GPU_COUNT_);
        int* ptr = buffer + (cpu_rank * ITEM_SIZE * items_per_cpu);
        MPI_Irecv(ptr, items_per_cpu * ITEM_SIZE, MPI_INT, n*32 + l, 0, MPI_COMM_WORLD, requests + cpu_rank);
      }
    }
    
    MPI_Waitall(env->num_cpus, requests, MPI_STATUSES_IGNORE);
  
    delete [] requests;
    delete [] statuses;
  }
  int end = clock_now();

  timer = calc_time(start, end);
}

void msg_2_cpu_to_gpu(const env_t* env, int*& buffer, double& timer) {
  if(env->cpu) {
    
  } else if(env->gpu) {

  }
}

void pack_stocks(const std::vector<Stock>& stocks, int*& buffer) {
  buffer = new int[stocks.size() * ITEM_SIZE];
  for(size_t i = 0; i < stocks.size(); ++i) {
    stocks[i].write_data(buffer + (i * ITEM_SIZE));
  }
}

void unpack_stocks(std::vector<Stock>& stocks, const int* buffer) {
  for(size_t i = 0; i < stocks.size(); ++i) {
    const int* ptr = buffer + (i * ITEM_SIZE);
    
    int id = ptr[0];
    int price = ptr[1];
    int quantity = ptr[2];
    std::vector< std::pair<int, int> > distr;

    assert(ptr[3] == -8888);
    assert(ptr[24] == -1888);
    size_t idx = 4;
    while(idx != 24) {
      int p = ptr[idx++];
      int v = ptr[idx++];
      distr.push_back({p, v});
    }
    
    stocks[i] = Stock(id, price, quantity, distr);
  }
}

void calculate_stock_prices(std::vector<Stock>& stocks, int*& buffer) {
  buffer = new int[stocks.size() * 2];
  for(size_t i = 0; i < stocks.size(); ++i) {
    int id = stocks[i].getID();
    int price = stocks[i].generatePrice();

    buffer[i*2] = id;
    buffer[i*2 + 1] = price;

    stocks[i].setPrice(price);
  }
}

void unpack_prices(std::vector<int>& prices, int* buffer) {
  const size_t n = prices.size()*2;
  for(size_t i = 0; i < n; i += 2) {
    int id = buffer[i];
    int price = buffer[i+1];
    prices[id] = price;
  }
}

void invest(env_t* env, std::vector<Investor>& investors, const std::vector<Stock>& stocks) {
  std::vector<Stock> market_stock_arr[100];
  for(size_t i = 0; i < stocks.size(); i += 1000) {
    for(int j = 0; j < 1000; ++j) {
      market_stock_arr[(i + j) / 1000].push_back(stocks[i + j]);
    }
  }

  int stock_values[100][6][1000];

  for(int i = 0; i < 100; ++i) {
    assert(market_stock_arr[i].size() == 1000);
    for(int j = 0; j < 1000; ++j) {
      stock_values[i][0][j] = market_stock_arr[i][j].expectedValue();
      stock_values[i][1][j] = market_stock_arr[i][j].maximumValue();
      stock_values[i][2][j] = market_stock_arr[i][j].minimumValue();
      stock_values[i][3][j] = market_stock_arr[i][j].mostLikelyValue();
      stock_values[i][4][j] = market_stock_arr[i][j].twoMostLikelyValues();
      stock_values[i][5][j] = market_stock_arr[i][j].randomSample();
    }
  }

  for(size_t i = 0; i < investors.size(); ++i) {
    if(investors[i].getBalance() <= 0) {
      continue;
    }

    const std::vector<Stock>& market_stocks = market_stock_arr[investors[i].getMarket()];

    int strategy = investors[i].getStrategy();
    int aggressiveness = investors[i].getAggressiveness();
    int market = investors[i].getMarket();
    int balance = investors[i].getBalance();
    double perc;

    switch(aggressiveness) {
      case 0: perc = 0.10; break;
      case 1: perc = 0.15; break;
      case 2: perc = 0.25; break;
      case 3: perc = 0.35; break;
      case 4: perc = 0.50; break;
      case 5: perc = 0.75; break;
      case 6: perc = 0.85; break;
      case 7: perc = 1.0; break;
      default: assert(false); break;
    }

    int bal = (int)(double(balance) * perc);
    
    std::vector< std::vector<int> > solution;
    int total;
    if(verbose && env->gpu_rank == 5 && (i % 100 == 0)) {
      printf("Starting knapsack (investor %lu of %lu) with balance %d\n", i, investors.size(), bal);
    }
    int v_temp = verbose;
    verbose = false;
    knapsack(market_stocks, stock_values[market][strategy], solution, total, market_stocks.size(), bal);
    verbose = v_temp;

    investors[i].invest(solution);
    if(verbose && env->gpu_rank == 5 && (i % 100 == 0)) {
      printf("Ending knapsack\n");
    }
  }

  if(verbose && env->gpu_rank == 5) {
    printf("Ending knapsack (final)\n");
  }
}

void evaluate(std::vector<Investor>& investors, std::vector<int>& prices) {
  for(size_t i = 0; i < investors.size(); ++i) {
    investors[i].eval(prices);
  }
}

void write_to_db() {

}

#endif
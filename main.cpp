#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <random>
#include <unordered_map>
#include <cstdlib>
#include "mpi.h"

#include "knapsack.h"
#include "stock.h"
#include "investor.h"
#include "clockcycle.h"
#include "sim.h"

bool verbose = false;

void terminate() {
  MPI_Finalize();
}

int main(int argc, char** argv) {

  //MPI Init
  MPI_Init(&argc, &argv);
  srand(time(NULL));
  env_t* env = new env_t;

  MPI_Comm_size(MPI_COMM_WORLD, &env->num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &env->globalrank);
  env->num_nodes = env->num_ranks / 32;
  env->node = env->globalrank / 32;
  env->localrank = env->globalrank % 32;
  env->gpu = env->localrank < GPU_COUNT_;
  env->cpu = env->localrank >= GPU_COUNT_ && env->localrank < 31;
  env->worker = env->localrank == 31;

  env->cpu_rank = env->cpu ? CPU_COUNT_ * env->node + (env->localrank - GPU_COUNT_) : -1;
  env->gpu_rank = env->gpu ? GPU_COUNT_ * env->node + env->localrank : -1;
  env->worker_rank = env->node;
  env->num_cpus = CPU_COUNT_ * env->num_nodes;
  env->num_gpus = GPU_COUNT_ * env->num_nodes;

  if(argc < 2) {
    if(env->globalrank == 0) {
      printf("USAGE: ./executable <file> <options>\n");
    }
    terminate();
    return 1;
  }

  int n = 32;
  for(int i = 2; i < argc; ++i) {
    std::string arg(argv[i]);

    if(arg == "-v") {
      verbose = true;
    } else {
      printf("Invalid argument \"%s\"\n", argv[i]);
      return 1;
    }
  }

  if(env->gpu) {
    mapRankToGPU(env->gpu_rank);
  }

  if(verbose && env->globalrank == 0) {
    printf("Running program with %d total ranks, in sections of %d on %d node(s)\n", env->num_ranks, n, env->num_nodes);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_File fh;

  std::vector<Stock> stocks;
  std::vector<Investor> investors;

  if(verbose && env->cpu_rank == 0) {
    printf("Reading from file %s\n", argv[1]);
  }

  MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
  read_file(env, stocks, fh);
  MPI_File_close(&fh);

  MPI_Barrier(MPI_COMM_WORLD);

  if(verbose && env->globalrank == 0) {
    printf("Finished reading file %s\n", argv[1]);
  }

  int local_investor_count = 0;
  int node_investor_count;

  if(env->gpu) {
    stocks = std::vector<Stock>(100000);
    local_investor_count = 4800 / env->num_gpus;
    node_investor_count = 4800 / env->num_nodes;
    investors = std::vector<Investor>(local_investor_count);
    const int offset = local_investor_count * env->gpu_rank;
    for(int i = 0; i < local_investor_count; ++i) {
      int id = offset + i;
      int strategy = id / 800;
      int aggressiveness = id % 8;
      int market = id % 100;

      investors[i] = Investor(id, strategy, aggressiveness, market, 100000);
    }
  }

  // for(int i = 0; i < env->num_gpus; ++i) {

  //   if(env->gpu && i == env->gpu_rank) {
  //     for(auto& inv: investors) {
  //       printf("%4d %d %d\n", inv.getID(), inv.getStrategy(), inv.getAggressiveness());
  //     }
  //   }

  //   MPI_Barrier(MPI_COMM_WORLD);
  // }

  // if(1) {
  //   terminate();
  //   return 0;
  // }

  double io_a_times[ROUNDS];
  double io_b_times[ROUNDS];
  double k_times[ROUNDS];

  for(int i = 0; i < ROUNDS; ++i) {
    int* buffer = NULL;

    //STEP 1:  CPUs send stocks to GPUs, and GPUs run the parallel knapsack algorithm
    if(env->cpu) {
      pack_stocks(stocks, buffer);
    }

    msg_1_cpu_to_gpu(env, buffer, io_a_times[i]);

    if(env->gpu) {
      unpack_stocks(stocks, buffer);
      for(size_t i = 0; i < stocks.size(); ++i) {
        if(stocks[i].getID() == -1) {
          printf("GPU %d failed to receive all stocks\n", env->gpu_rank);
          return 1;
        }
      }
    }

    delete [] buffer;
    buffer = NULL;

    if(env->gpu) {
      int k_start_time = clock_now();
      invest(env, investors, stocks);
      int k_end_time = clock_now();

      k_times[i] = calc_time(k_start_time, k_end_time);
    }

    MPI_Barrier(MPI_COMM_WORLD);


    //STEP 2:  CPUs send new stock prices, and investor balances are updated
    if(env->cpu) {
      calculate_stock_prices(stocks, buffer);
    }

    msg_2_cpu_to_gpu(env, buffer, io_b_times[i]);
    if(env->gpu) {
      std::vector<int> prices(stocks.size(), -7777777);
      unpack_prices(prices, buffer);
      for(size_t i = 0; i < stocks.size(); ++i) {
        if(prices[i] == -7777777) {
          printf("GPU %d could not receive all prices\n", env->gpu_rank);
          return 1;
        }
      }
      evaluate(investors, prices);
    }

    delete [] buffer;
    buffer = NULL;

    //Step 3:  Data is recorded to DB
    //TODO


    if(verbose && env->gpu && (env->gpu_rank == 5 || 1)) {
      printf("Round %d complete (GPU %d) (I/O time: %5.5fs) (Knapsack time: %5.5fs)\n", i+1, env->gpu_rank, io_a_times[i], k_times[i]);
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }
  if(env->globalrank == 0) printf("End loop\n");

  delete env;

  terminate();

  return 0;
}

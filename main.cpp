#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <random>
#include <iomanip>
#include <unordered_map>
#include <cstdlib>
#include "mpi.h"

#include "knapsack.h"
#include "stock.h"
#include "investor.h"
#include "timer.h"
#include "clockcycle.h"
#include "macros.h"
#include "pack.h"
#include "env.h"
#include "msg.h"
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

  std::ofstream outFile1, outFile2;

  if(env->gpu) {
    char outFile1Name[100] = {'\0'};
    char outFile2Name[100] = {'\0'};
    sprintf(outFile1Name, "output/gpu/gpu_%d_with_%d_nodes.out", env->gpu_rank, env->num_nodes);
    sprintf(outFile2Name, "output/io/gpu_%d_with_%d_nodes.out", env->gpu_rank, env->num_nodes);
    outFile1.open(outFile1Name);
    outFile2.open(outFile2Name);
    assert(outFile1.is_open());
    assert(outFile2.is_open());

    mapRankToGPU(env->gpu_rank);
  }

  if(env->worker) {
    char outFile1Name[100] = {'\0'};
    char outFile2Name[100] = {'\0'};
    sprintf(outFile1Name, "output/worker/worker_%d_with_%d_nodes.out", env->worker_rank, env->num_nodes);
    sprintf(outFile2Name, "output/round/worker_%d_with_%d_nodes.out", env->worker_rank, env->num_nodes);
    outFile1.open(outFile1Name);
    outFile2.open(outFile2Name);
    assert(outFile1.is_open());
    assert(outFile2.is_open());
  }

  if(verbose && env->globalrank == 0) {
    printf("Running program with %d total ranks, in sections of %d on %d node(s)\n", env->num_ranks, n, env->num_nodes);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_File fh;

  if(verbose && env->cpu_rank == 0) {
    printf("Reading from file %s\n", argv[1]);
  }
  std::vector<Stock> stocks;
  std::vector<Investor> investors;
  generate_investors(env, investors);

  MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
  read_file(env, stocks, fh);
  MPI_File_close(&fh);

  MPI_Barrier(MPI_COMM_WORLD);

  if(verbose && env->globalrank == 0) {
    printf("Finished reading file %s\n", argv[1]);
  }

  double io_a1_times[ROUNDS];
  double io_a2_times[ROUNDS];
  double io_b1_times[ROUNDS];
  double io_b2_times[ROUNDS];
  double k_times[ROUNDS];

  if(env->gpu) {
    stocks = std::vector<Stock>(100000);
  }

  for(int i = 0; i < ROUNDS; ++i) {
    auto round_start = clock_time();
    int* buffer = NULL;

    //STEP 1:  Stocks and investors are sent to GPUs,
    //          and GPUs run the parallel knapsack algorithm
    if(env->cpu) {
      pack_stocks(stocks, buffer);
    }

    msg_1(env, buffer, io_a1_times[i]);
    if(env->gpu) {
      unpack_stocks(stocks, buffer);
      assert(stocks.size() == 100000);
      for(size_t i = 0; i < stocks.size(); ++i) {
        if(stocks[i].getID() == -1) {
          printf("GPU %d failed to receive all stocks\n", env->gpu_rank);
          return 1;
        }
      }
    }

    if(buffer != NULL) delete [] buffer;
    buffer = NULL;

    if(env->worker) {
      solution_vec solution = load_balance(env, investors);
      pack_investors(env, investors, solution, buffer);
    }

    msg_2(env, buffer, io_a2_times[i]);


    if(env->gpu) {
      unpack_investors(env, investors, buffer);
    }

    std::vector<std::pair<int, double> > times;
    if(env->gpu) {
      auto k_start_time = clock_time();
      invest(env, investors, stocks, times);
      auto k_end_time = clock_time();

      k_times[i] = calc_time(k_start_time, k_end_time);
    }

    //STEP 2:  CPUs send new stock prices, and investor balances are updated
    calculate_stock_prices(env, stocks, buffer);

    msg_3(env, buffer, io_b1_times[i]);
    if(env->gpu) {
      std::vector<int> prices(stocks.size(), -7777777);
      unpack_prices(prices, buffer);
      for(size_t i = 0; i < stocks.size(); ++i) {
        if(prices[i] == -7777777) {
          printf("GPU %d could not receive all prices (price #%lu)\n", env->gpu_rank, i);
          return 1;
        }
      }
      evaluate(investors, prices);
    }

    if(buffer != NULL) delete [] buffer;
    buffer = NULL;

    if(env->gpu) {
      pack_results(env, investors, buffer);
    }

    msg_4(env, buffer, io_b2_times[i]);

    if(env->worker) {
      unpack_results(env, investors, buffer);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if(buffer != NULL) delete [] buffer;
    buffer = NULL;
    auto round_end = clock_time();
    auto round_time = calc_time(round_start, round_end);

    //Step 3:  Data is written to file
    if(env->worker) {
      //nodes node round id strategy aggresiveness market balance
      auto start = clock_time();
      for(const auto& inv: investors) {
        outFile1 << env->num_nodes << " " << env->node
                << " " << i+1 << " " << inv.getID()
                << " " << inv.getStrategy()
                << " " << inv.getAggressiveness()
                << " " << inv.getMarket()
                << " " << inv.getBalance() << std::endl;  
      }
      auto end = clock_time();
      double io_time = calc_time(start, end);

      if(env->worker_rank == 0) {
        //nodes round time i/o time
        outFile2 << env->num_nodes << " "
                 << i+1 << " "
                 << std::setprecision(4) << round_time << " "
                 << std::setprecision(4) << io_time << std::endl;
      }
    }

    if(env->gpu) {
      //nodes node round balance time
      auto start = clock_time();
      for(const auto& p: times) {
        outFile1 << env->num_nodes << " " << env->node
                << " " << i+1 << " " << p.first << " "
                << std::setprecision(4) << p.second << std::endl;
      }
      auto end = clock_time();
      double io_time = calc_time(start, end);

      //nodes gpu round time
      outFile2 << env->num_nodes << " " << env->gpu_rank
               << " " << i+1 << " " << std::setprecision(7) << io_time << std::endl;
    }

    if(verbose && env->gpu) {
      printf("Round %d complete (GPU %d) (I/O time: %5.5fs) (Knapsack time: %5.5fs)\n", i+1, env->gpu_rank, io_a1_times[i], k_times[i]);
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }
  if(env->globalrank == 0) printf("End loop\n");

  if(outFile1.is_open()) {
    outFile1.close();
  }

  if(outFile2.is_open()) {
    outFile2.close();
  }

  delete env;

  terminate();

  return 0;
}

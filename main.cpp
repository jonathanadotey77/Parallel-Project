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


  if(verbose && env->globalrank == 0) {
    printf("Running program with %d total ranks, in sections of %d on %d node(s)\n", env->num_ranks, n, env->num_nodes);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_File fh;

  std::vector<Stock> stocks;

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

  if(env->gpu) {
    stocks = std::vector<Stock>(100000);
  }

  float a_times[ROUNDS];

  for(int i = 0; i < ROUNDS; ++i) {
    int* buffer = NULL;
    if(env->cpu) {
      pack_stocks(env, stocks, buffer);
    }

    msg_cpu_to_gpu(env, buffer, a_times[i]);

    if(env->gpu) {
      unpack_stocks(stocks, buffer);
      for(size_t i = 0; i < stocks.size(); ++i) {
        if(stocks[i].getID() == -1) {
          printf("GPU %d failed to receive all stocks\n", env->gpu_rank);
          return 1;
        }
      }
    }

    if(verbose && env->globalrank == 0) {
      printf("Round %d complete (I/O time: %5.3fs)\n", i+1, a_times[i]);
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }

  delete env;

  terminate();

  return 0;
}

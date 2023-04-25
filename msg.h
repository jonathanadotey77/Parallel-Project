#ifndef MSG_H
#define MSG_H

#include "macros.h"
#include "mpi.h"
#include "env.h"

void msg_1(const env_t* env, int*& buffer, double& timer) {
  const int items_per_cpu = 100000 / (CPU_COUNT_ * env->num_nodes);
  auto start = clock_time();
  if(env->cpu) {  
    MPI_Request* requests = new MPI_Request[env->num_gpus];
    for(int n = 0; n < env->num_nodes; ++n) {
      for(int l = 0; l < GPU_COUNT_; ++l) {
        int gpu_rank = (GPU_COUNT_ * n) + l;
        MPI_Isend(buffer, items_per_cpu * ITEM_SIZE, MPI_INT, n*32 + l, 0, MPI_COMM_WORLD, requests + gpu_rank);
      }
    }

    MPI_Waitall(env->num_gpus, requests, MPI_STATUSES_IGNORE);
    delete [] requests;
  } else if(env->gpu) {
    //Receive
    MPI_Request* requests = new MPI_Request[env->num_cpus];

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
  }
  auto end = clock_time();
  timer = calc_time(start, end);

  MPI_Barrier(MPI_COMM_WORLD);
}

void msg_2(const env_t* env, int*& buffer, double& timer) {
  const int num_investors = 4800 / env->num_nodes;
  const int block_size = num_investors * INVESTOR_SIZE;
  auto start = clock_time();
  if(env->worker) {
    MPI_Request* requests = new MPI_Request[GPU_COUNT_];
    for(int i = 0; i < GPU_COUNT_; ++i) {
      int* ptr = buffer + (i * block_size);
      MPI_Isend(ptr, block_size, MPI_INT, env->node * 32 + i, 1, MPI_COMM_WORLD, requests + i);
    }

    MPI_Waitall(GPU_COUNT_, requests, MPI_STATUSES_IGNORE);
  } else if(env->gpu) {
    buffer = new int[block_size];
    MPI_Request request;
    MPI_Irecv(buffer, block_size, MPI_INT, 32 * env->node + 31, 1, MPI_COMM_WORLD, &request);
    MPI_Wait(&request, MPI_STATUS_IGNORE);
  }

  auto end = clock_time();
  timer = calc_time(start, end);

  MPI_Barrier(MPI_COMM_WORLD);
}

void msg_3(const env_t* env, int*& buffer, double& timer) {
  const int items_per_cpu = 100000 / (env->num_cpus);
  auto start = clock_time();
  if(env->cpu) {
    MPI_Request* requests = new MPI_Request[env->num_gpus];
    for(int n = 0; n < env->num_nodes; ++n) {
      for(int l = 0; l < GPU_COUNT_; ++l) {
        int gpu_rank = (GPU_COUNT_ * n) + l;
        MPI_Isend(buffer, items_per_cpu * 2, MPI_INT, n*32 + l, 0, MPI_COMM_WORLD, requests + gpu_rank);
      }
    }

    MPI_Waitall(env->num_gpus, requests, MPI_STATUSES_IGNORE);
    delete [] requests;
  } else if(env->gpu) {
    MPI_Request* requests = new MPI_Request[env->num_cpus];
    buffer = new int[env->num_cpus * items_per_cpu * 2];
    for(int i = 0; i < env->num_cpus * items_per_cpu * 2; ++i) {
      buffer[i] = 0;
    }
    for(int n = 0; n < env->num_nodes; ++n) {
      for(int l = 0; l < 32; ++l) {
        if(l < GPU_COUNT_ || l >= CPU_COUNT_ + GPU_COUNT_) {
          continue;
        }
        int cpu_rank = (CPU_COUNT_ * n) + (l - GPU_COUNT_);
        int* ptr = buffer + (cpu_rank * items_per_cpu * 2);
        MPI_Irecv(ptr, items_per_cpu * 2, MPI_INT, n*32 + l, 0, MPI_COMM_WORLD, requests + cpu_rank);
      }
    }

    MPI_Waitall(env->num_cpus, requests, MPI_STATUSES_IGNORE);
    delete [] requests;
  }

  auto end = clock_time();
  timer = calc_time(start, end);

  MPI_Barrier(MPI_COMM_WORLD);
}

void msg_4(const env_t* env, int*& buffer, double& timer) {
  const int num_investors = 4800 / env->num_nodes;
  const int block_size = num_investors * 2;

  auto start = clock_time();
  if(env->worker) {
    buffer = new int[GPU_COUNT_ * block_size];
    MPI_Request* requests = new MPI_Request[GPU_COUNT_];
    for(int i = 0; i < GPU_COUNT_; ++i) {
      int* ptr = buffer + (i * block_size);
      MPI_Irecv(ptr, block_size, MPI_INT, 32 * env->node + i, 1, MPI_COMM_WORLD, requests + i);
    }

    MPI_Waitall(GPU_COUNT_, requests, MPI_STATUSES_IGNORE);
    delete [] requests;
  } else if(env->gpu) {
    MPI_Request request;
    MPI_Isend(buffer, block_size, MPI_INT, 32 * env->node + 31, 1, MPI_COMM_WORLD, &request);
  }
  auto end = clock_time();
  timer = calc_time(start, end);

  MPI_Barrier(MPI_COMM_WORLD);
}

#endif
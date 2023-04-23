#ifndef ENV_H
#define ENV_H

typedef struct env_t {
  int globalrank, localrank, num_ranks, num_nodes;
  int cpu, gpu, worker;
  int cpu_rank, gpu_rank, worker_rank, node;
  int num_cpus, num_gpus;
} env_t;

#endif
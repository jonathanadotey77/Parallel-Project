#include <iostream>
// #include <fstream>
#include <stdio.h>
#include <vector>
#include <random>
#include <cstdlib>
#include "mpi.h"

#include "knapsack.h"
#include "stock.h"

bool verbose = false;

bool read_stock_file(std::vector<Stock>& stocks, char* filename) {
  //Read file input
  return true;
}


void terminate() {
  MPI_Finalize();
}



int main(int argc, char** argv) {
  //MPI Init
  MPI_Init(&argc, &argv);
  srand(time(NULL));

  int myrank, num_ranks;

  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  printf("Rank %d of %d\n", myrank, num_ranks);
  MPI_Barrier(MPI_COMM_WORLD);
  std::vector<Stock> stocks;

  

  terminate();

  return 0;
}
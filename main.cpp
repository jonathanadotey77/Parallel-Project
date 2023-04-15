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

bool verbose = false;


void terminate() {
  MPI_Finalize();
}

float calc_time(int start, int end) {
  return (float)(end - start) / 512000000.0;
}

bool load_stocks(std::string filename, std::vector<Stock>& stocks) {
  std::ifstream inFile(filename);

  if(!inFile.is_open()) {
    std::cerr << "Could not open file " << filename << std::endl;
    return false;
  }

  int id, price, quantity;
  std::vector< std::pair<int, int> > distr;

  while(inFile >> id) {
    inFile >> price >> quantity;
    int a, b;

    inFile >> a;
    inFile >> a;

    while(a != -1888) {
      inFile >> b;

      distr.push_back({a, b});

      inFile >> a;
    }

    stocks.push_back(Stock(id, price, quantity, distr));
    std::cout << stocks.back().to_string();
    distr.clear();
  }

  inFile.close();

  return true;
}

void parse_buffer(const char* buffer, std::vector<Stock>& stocks) {
  std::vector<int> nums;

  std::string temp;
  for(auto ptr = buffer; *ptr != '\0'; ++ptr) {
    char c = *ptr;

    if(c == ' ' || c == '\n' || c == '\r') {
      if(temp.size() > 0) {
        nums.push_back(std::stoi(temp));
      }
      temp.clear();
    } else {
      temp.push_back(c);
    }
  }
  
  if(temp.size() > 0) {
    nums.push_back(std::stoi(temp));
  }
  temp.clear();

  size_t i = 0;

  while(i < nums.size()) {
    int id = nums[i++];
    int price = nums[i++];
    int quantity = nums[i++];
    std::vector< std::pair<int, int> > distr;

    int a = nums[i++];
    int b; 
    while(i < nums.size() && (a = nums[i++]) != -1888) {
      b = nums[i++];
      distr.push_back({a, b});
    }
    
    stocks.push_back(Stock(id, price, quantity, distr));
  }
}

int main(int argc, char** argv) {

  //MPI Init
  MPI_Init(&argc, &argv);
  srand(time(NULL));

  int myrank, localrank, num_ranks;

  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);


  if(argc < 2) {
    if(myrank == 0) {
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

  int num_nodes = num_ranks / 32;

  if(verbose && myrank == 0) {
    printf("Running program with %d total ranks, in sections of %d on %d node(s)\n", num_ranks, n, num_nodes);
  }

  int node = myrank / n;
  localrank = myrank % n;
  bool gpu = localrank < 6;
  bool cpu = localrank >= 6 && localrank < 31;
  bool worker = localrank == 31;

  int cpu_rank = cpu ? 25 * node + (localrank - 6) : -1;
  int gpu_rank = gpu ? 6 * node + localrank : -1;
  int worker_rank = node;

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_File fh;

  
  std::vector<Stock> stocks;

  if(verbose && cpu_rank == 0) {
    printf("Reading from file %s\n", argv[1]);
  }

  MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
  int* buffer = NULL;
  if(cpu) {
    const int count = 100000 / (25 * num_nodes); // 500
    MPI_Offset start = cpu_rank * (count * 25);
    buffer = new int[count * 25];
    MPI_Status status;

    MPI_File_read_at(fh, start, buffer, 25 * count, MPI_INT, &status);

    for(int i = 0; i < count; ++i) {
      stocks.push_back(Stock(buffer + i*25));
    }

    delete [] buffer;
  }
  MPI_File_close(&fh);
  MPI_Barrier(MPI_COMM_WORLD);

  if(verbose && myrank == 0) {
    printf("Finished reading file %s\n", argv[1]);
  }

  terminate();

  return 0;
}

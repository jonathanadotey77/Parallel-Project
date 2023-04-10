#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <string>

#include "knapsack.h"
#include "testing.h"

bool verbose = false;

void usage() {
  std::cerr << "Usage: ./executable <input file> <budget> <option>\n";
}

int main(int argc, char** argv) {
  if(argc != 3 && argc != 4) {
    usage();
    return 1;
  }

  if(argc == 4 && std::string(argv[3]) == "-v") {
    verbose = true;
  }

  std::string filename(argv[1]);
  int budget = atoi(argv[2]);

  if(budget) {
    if(budget < 0) {
      std::cerr << "Budget cannot be negative\n";
      return 1;
    }
    std::cout << "Budget is " << budget << std::endl;

    if(budget > 4000000) {
      std::cerr << "Budget cannot be over 1 million\n";
      return 1;
    }
  } else {
    std::cout << "Budget is zero; setting to $1,000,000\n";
    budget = 1000000;
  }

  std::vector<Stock> stocks;
  bool load_success = load_stocks(filename, stocks);
  if(!load_success) {
    return 1;
  }
  
  printf("Num items: %lu\n", stocks.size());
  int v;
  std::vector< std::pair<int, int> > solution;
  knapsack(stocks, solution, v, stocks.size(), budget);
  std::cout << v << std::endl;

  return 0;
}
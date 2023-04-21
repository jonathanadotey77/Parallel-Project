#include <string>
#include <vector>

#include "stock.h"

bool mapRankToGPU(int myrank);

void allocateItems(const std::vector<Stock>& stocks,
  int* stock_values,
  int*& item_costs, int*& item_values, int*& item_quantities,
  unsigned short*& chosen);
  
bool load_stocks(std::string filename, std::vector<Stock>& stocks);

bool allocateTables(int*& table, int*& pointers, int*& quants,
  const size_t num_items, const int budget);

void freeTable(int* table, int* pointers, int* quants);

void freeItems(int* item_costs, int* item_values, int* item_quantities,
  unsigned short* chosen);

void knapsack(const std::vector<Stock>& stocks,
  const int* stock_values,
  std::vector< std::vector<int> >& solution,
  int& total, size_t num_items, int budget);
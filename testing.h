#ifndef TESTING_H
#define TESTING_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "stock.h"

void generate_small_test(std::vector<Stock>& stocks);
void generate_simple_test(std::vector<Stock>& stocks);
void generate_full_test(std::vector<Stock>& stocks, int n = 1000);

bool load_stocks(std::string filename, std::vector<Stock>& stocks);

#endif
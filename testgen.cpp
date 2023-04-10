#include <iostream>
#include <fstream>
#include <vector>

#include "testing.h"
#include "stock.h"

void write_test(const std::string& filename, const std::vector<Stock>& stocks) {
  std::ofstream outFile(filename);

  for(const Stock& s: stocks) {
    outFile << s.to_string() << std::endl;
  }

  outFile.close();
}

int main(int argc, char** argv) {
  if(argc != 3) {
    std::cerr << "Need exactly 3 args\n";
    return 1;
  }

  std::string filename(argv[1]);
  std::string arg(argv[2]);

  if(arg != "simple" && arg != "small" && arg != "full") {
    std::cerr << "Invalid argument\n";
  }

  std::vector<Stock> stocks;

  if(arg == "simple") {
    generate_simple_test(stocks);
  } else if(arg == "small") {
    generate_small_test(stocks);
  } else {
    generate_full_test(stocks);
  }

  write_test(filename, stocks);

  return 0;
}
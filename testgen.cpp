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

  std::vector<Stock> stocks;

  if(arg == "simple") {
    generate_simple_test(stocks);
  } else if(arg == "small") {
    generate_small_test(stocks);
  } else if(arg == "full") {
    generate_full_test(stocks);
  }else if(arg == "large") {
    generate_full_test(stocks, 100000);
  }else if(arg == "huge") {
    generate_full_test(stocks, 1000000);
  } else {
    std::cerr << "Invalid argument\n";
    return 1;
  }

  // write_test(filename, stocks);
  std::ofstream os(filename, std::ios::binary);

  for(const auto& s: stocks) {
    s.write_to_file(os);
  }
  os.close();
  return 0;
}
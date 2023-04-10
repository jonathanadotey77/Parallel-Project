#include <ctime>

#include "testing.h"

void generate_small_test(std::vector<Stock>& stocks) {
  // stocks.push_back(Stock(0, 10, 5, std::vector<std::pair<int, int>>(1, {100, 60})));
  // stocks.push_back(Stock(0, 20, 4, std::vector<std::pair<int, int>>(1, {100, 100})));
  // stocks.push_back(Stock(0, 30, 2, std::vector<std::pair<int, int>>(1, {100, 120})));
  stocks.push_back(Stock(1, 2, 2, std::vector<std::pair<int, int>>(1, {100, 10})));
  stocks.push_back(Stock(2, 4, 3, std::vector<std::pair<int, int>>(1, {100, 20})));
  stocks.push_back(Stock(3, 6, 1, std::vector<std::pair<int, int>>(1, {100, 30})));
  stocks.push_back(Stock(4, 8, 2, std::vector<std::pair<int, int>>(1, {100, 40})));
  stocks.push_back(Stock(5, 10, 1, std::vector<std::pair<int, int>>(1, {100, 50})));
  stocks.push_back(Stock(6, 12, 3, std::vector<std::pair<int, int>>(1, {100, 60})));
  stocks.push_back(Stock(7, 14, 1, std::vector<std::pair<int, int>>(1, {100, 70})));
  stocks.push_back(Stock(8, 16, 2, std::vector<std::pair<int, int>>(1, {100, 80})));
  stocks.push_back(Stock(9, 18, 1, std::vector<std::pair<int, int>>(1, {100, 90})));
  stocks.push_back(Stock(10,  20, 3, std::vector<std::pair<int, int>>(1, {100, 100})));
  stocks.push_back(Stock(11,  22, 2, std::vector<std::pair<int, int>>(1, {100, 110})));
  stocks.push_back(Stock(12,  24, 1, std::vector<std::pair<int, int>>(1, {100, 120})));
  stocks.push_back(Stock(13,  26, 3, std::vector<std::pair<int, int>>(1, {100, 130})));
  stocks.push_back(Stock(14,  28, 1, std::vector<std::pair<int, int>>(1, {100, 140})));
  stocks.push_back(Stock(15,  30, 2, std::vector<std::pair<int, int>>(1, {100, 150})));
  stocks.push_back(Stock(16,  32, 1, std::vector<std::pair<int, int>>(1, {100, 160})));
  stocks.push_back(Stock(17,  34, 3, std::vector<std::pair<int, int>>(1, {100, 170})));
  stocks.push_back(Stock(18,  36, 2, std::vector<std::pair<int, int>>(1, {100, 180})));
  stocks.push_back(Stock(19,  38, 1, std::vector<std::pair<int, int>>(1, {100, 190})));
  stocks.push_back(Stock(20,  40, 3, std::vector<std::pair<int, int>>(1, {100, 200})));
  
}

void generate_simple_test(std::vector<Stock>& stocks) {
  srand(time(NULL));
  for(int i = 0; i < 1000; ++i) {
    int w = rand() % 71 + 70;
    int v = rand() % 24 - 9;
    int q = rand() % 340 + 200;
    if(q) {
      // printf("%d %d %d\n", w, v, q);
    }
    stocks.push_back(Stock(i, w, q, std::vector<std::pair<int, int>>(1, {100, v})));
  }
}

void generate_full_test(std::vector<Stock>& stocks) {
  srand(time(NULL));
  for(int i = 0; i < 1000; ++i) {
    int price = rand() % 71 + 70;
    int quantity = rand() % 340 + 200;

    std::vector< std::pair<int, int> > distr;
    int d = 2 + rand() % 9;

    int t = 100;

    int range = price / 7;

    for(int j = 0; j < d && t > 0; ++j) {
      int p;
      if(j+1 == d) {
        p = t;
      } else {
        p = 1 + rand() % t;
      }

      t -= p;

      int v = range + rand() % (range*2);

      distr.push_back({p, v});
    }

    stocks.push_back(Stock(i, price, quantity, distr));
  }
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
    distr.clear();
  }

  inFile.close();

  return true;
}
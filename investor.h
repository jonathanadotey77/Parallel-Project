#ifndef INVESTOR_H
#define INVESTOR_H

#include <vector>
#include <tuple>
#include <unordered_map>

class Investor {
public:

  Investor(int s, int a, int b) :
    strategy(s), aggressiveness(a), balance(b) {}

  //Inner vector is [stockId, oldPrice, quantity]
  void invest(const std::vector< std::vector<int> >& stocks) {
    this->portfolio = stocks;
  }

  void eval(std::unordered_map<int, int>& newPrices) {
    for(const std::vector<int>& item: portfolio) {
      int stockId = item[0];
      int oldPrice = item[1];
      int quantity = item[2];

      balance -= oldPrice * quantity;
      balance += newPrices[stockId] * quantity;
    }
  }

  int getStrategy() const {
    return this->strategy;
  }

  int getAggressiveness() const {
    return this->aggressiveness;
  }

  int getBalance() const {
    return this->balance;
  }

private:

  //Inner vector is [stockId, oldPrice, quantity]
  std::vector< std::vector<int> > portfolio;
  int strategy;
  int aggressiveness;
  int balance;
};

#endif

#ifndef INVESTOR_H
#define INVESTOR_H

#include <vector>
#include <tuple>
#include <unordered_map>
#include <assert.h>

class Investor {
public:

  Investor() : id(-1) {}

  Investor(int ID, int s, int a, int m, int b) :
    id(ID), strategy(s), aggressiveness(a), market(m), balance(b) {}

  //Inner vector is [stockId, oldPrice, quantity]
  void invest(const std::vector< std::vector<int> >& stocks) {
    this->portfolio = stocks;
  }

  void eval(std::vector<int>& newPrices) {
    for(const std::vector<int>& item: portfolio) {
      int stockId = item[0];
      int oldPrice = item[1];
      int quantity = item[2];

      balance -= oldPrice * quantity;
      balance += newPrices[stockId] * quantity;
    }
  }

  int getID() const {
    return this->id;
  }

  int getStrategy() const {
    return this->strategy;
  }

  int getAggressiveness() const {
    return this->aggressiveness;
  }

  int getMarket() const {
    return this->market;
  }

  int getBalance() const {
    return this->balance;
  }

private:

  //Inner vector is [stockId, oldPrice, quantity]
  std::vector< std::vector<int> > portfolio;
  int id;
  int strategy;
  int aggressiveness;
  int market;
  int balance;
};

#endif

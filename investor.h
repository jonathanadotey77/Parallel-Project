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

    // printf("Investor %d gain/loss: %d\n", this->id, balance - old);
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

  void setBalance(int bal) {
    this->balance = bal;
  }

  int getSpending() const {
    double perc = 0.0;
    switch(this->aggressiveness) {
      case 0: perc = 0.10; break;
      case 1: perc = 0.15; break;
      case 2: perc = 0.25; break;
      case 3: perc = 0.35; break;
      case 4: perc = 0.50; break;
      case 5: perc = 0.75; break;
      case 6: perc = 0.85; break;
      case 7: perc = 1.0; break;
      default: assert(false); break;
    }

    return (int)(double(balance) * perc);
  }

  int* get_data() const {
    int* data = new int[5];
    data[0] = id;
    data[1] = strategy;
    data[2] = aggressiveness;
    data[3] = market;
    data[4] = balance;

    return data;
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

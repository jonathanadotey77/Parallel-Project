#ifndef STOCK_H
#define STOCK_H

#include <vector>

class Stock {
public:

  Stock(int ID, int p, int q, const std::vector< std::pair<int, int> >& distr)
    : id(ID), price(p), quantity(q), distribution(distr) {}

  int getID() const { return id; }
  int getPrice() const { return price; }
  int getQuantity() const { return quantity; }

  int expectedValue() const {
    double e = 0;

    for(size_t i = 0; i < this->distribution.size(); ++i) {
      e += ((float)distribution[i].first / 100.0) * (float)distribution[i].second;
    }

    return (int)(e + 0.5);
  }
  int maximumValue() const;
  int minimumValue() const;
  int mostLikelyValue() const;
  int nMostLikelyValues(int n) const;

private:

  int id;
  int price;
  int quantity;
  std::vector< std::pair<int, int> > distribution;
};

#endif
#ifndef STOCK_H
#define STOCK_H

#include <vector>
#include <string>
#include <assert.h>
#include <algorithm>

class Stock {
public:

  Stock(int ID, int p, int q, const std::vector< std::pair<int, int> >& distr)
    : id(ID), price(p), quantity(q), distribution(distr) {assert(check_distr());}

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

  int maximumValue() const {
    int mx = distribution[0].second;

    for(const std::pair<int, int>& p: this->distribution) {
      int val = p.second;
      if(val > mx) {
        mx = val;
      }
    }

    return mx;
  }

  int minimumValue() const {
    int mn = distribution[0].second;

    for(const std::pair<int, int>& p: this->distribution) {
      int val = p.second;
      if(val < mn) {
        mn = val;
      }
    }

    return mn;
  }

  int mostLikelyValue() const {
    int max_prob = distribution[0].first;
    int max_val = distribution[1].second;

    for(const std::pair<int, int>& p: this->distribution) {
      int prob = p.first;
      int val = p.second;
      if(prob > max_prob) {
        max_prob = prob;
        max_val = val;
      }
    }

    return max_val;
  }
  int twoMostLikelyValues() const {
    std::vector< std::pair<int, int> > copy = this->distribution;
    std::sort(copy.begin(), copy.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
      return a.first > b.first || (a.first == b.first && a.second > b.second);
    });

    return (copy[0].second + copy[1].second) / 2;
  }

  bool check_distr() const {
    int s = 0;

    for(const std::pair<int, int> & p: distribution) {
      s += p.first;
    }

    return s == 100;
  }

  std::string to_string() const {
    std::string s;

    s += std::to_string(id) + " " + std::to_string(price) + " " + std::to_string(quantity) + " -8888";

    for(auto& p: distribution) {
      s += " " + std::to_string(p.first) + " " + std::to_string(p.second);
    }

    s += " -1888";

    return s;
  }

private:

  int id;
  int price;
  int quantity;
  std::vector< std::pair<int, int> > distribution;
};

#endif
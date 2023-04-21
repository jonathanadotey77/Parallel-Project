#ifndef STOCK_H
#define STOCK_H

#include <vector>
#include <string>
#include <assert.h>
#include <algorithm>
#include <random>

class Stock {
public:

  Stock() {
    id = -1;
  }

  Stock(int ID, int p, int q, const std::vector< std::pair<int, int> >& distr)
    : id(ID), price(p), quantity(q), distribution(distr) {
      assert(check_distr());
    }
  
  Stock(const int* buffer) {
    this->id = buffer[0];
    this->price = buffer[1];
    this->quantity = buffer[2];

    assert(buffer[3] == -8888);
    assert(buffer[24] == -1888);
    size_t idx = 4;
    while(idx != 24) {
      int p = buffer[idx++];
      int v = buffer[idx++];
      this->distribution.push_back({p, v});
    }

    assert(check_distr());
  }

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

  int randomSample() const {
    return generatePrice();
  }

  bool check_distr() const {
    if(distribution.size() != 10) {
      return false;
    }
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

  void write_to_file(std::ostream& os) const {
    
    int* data = this->get_data();

    for(int i = 0; i < 25; ++i) {
      os.write((char*)(data + i), sizeof(int));
    }

    delete [] data;
  }

  int* get_data() const {
    int* buffer = new int[25];

    buffer[0] = id;
    buffer[1] = price;
    buffer[2] = quantity;
    buffer[3] = -8888;

    size_t idx = 4;
    for(const auto& p: distribution) {
      buffer[idx++] = p.first;
      buffer[idx++] = p.second;
    }

    buffer[24] = -1888;

    return buffer;
  }

  void write_data(int* buffer) const {
    buffer[0] = id;
    buffer[1] = price;
    buffer[2] = quantity;
    buffer[3] = -8888;

    size_t idx = 4;
    for(const auto& p: distribution) {
      buffer[idx++] = p.first;
      buffer[idx++] = p.second;
    }

    buffer[24] = -1888;
  }

  void setPrice(int p) {
    this->price = p;
    this->regenerateDistribution();
  }

  int generatePrice(bool v = false) const {

    int p = rand() % 101;

    size_t i = 0;
    while(i < distribution.size()) {
      if(v) {
        printf("p is %d\n", p);
      }
      if(p < distribution[i].first) {
        return distribution[i].second;
      }

      p -= distribution[i].first;
      ++i;
    }

    return distribution.back().second;
  }

  void regenerateDistribution() {
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

      int v = rand() % (range*2) - range;

      distr.push_back({p, v});
    }

    while(distr.size() < 10) {
      distr.push_back({0, 0});
    }

    this->distribution = distr;
    assert(check_distr());
  }

private:

  int id;
  int price;
  int quantity;
  std::vector< std::pair<int, int> > distribution;
};

#endif

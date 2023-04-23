#ifndef PACK_H
#define PACK_H

#include <vector>
#include <unordered_map>
#include "macros.h"
#include "stock.h"
#include "env.h"

void pack_stocks(const std::vector<Stock>& stocks, int*& buffer) {
  buffer = new int[stocks.size() * ITEM_SIZE];
  for(size_t i = 0; i < stocks.size(); ++i) {
    stocks[i].write_data(buffer + (i * ITEM_SIZE));
  }
}

void unpack_stocks(std::vector<Stock>& stocks, const int* buffer) {
  for(size_t i = 0; i < stocks.size(); ++i) {
    const int* ptr = buffer + (i * ITEM_SIZE);
    
    int id = ptr[0];
    int price = ptr[1];
    int quantity = ptr[2];
    std::vector< std::pair<int, int> > distr;

    assert(ptr[3] == -8888);
    assert(ptr[24] == -1888);
    size_t idx = 4;
    while(idx != 24) {
      int p = ptr[idx++];
      int v = ptr[idx++];
      distr.push_back({p, v});
    }
    
    stocks[i] = Stock(id, price, quantity, distr);
  }
}

void pack_investors(const env_t* env, const std::vector<Investor>& investors,
  const std::vector< std::vector<int> >& solution, int*& buffer
) {
  const size_t num_investors = size_t(4800 / env->num_nodes);
  std::unordered_map<int, const Investor*> mp;
  for(size_t i = 0; i < investors.size(); ++i) {
    mp[investors[i].getID()] = &investors[i];
  }
  const int block_size = num_investors * INVESTOR_SIZE;
  buffer = new int[GPU_COUNT_ * block_size];
  for(int i = 0; i < GPU_COUNT_; ++i) {
    for(size_t j = 0; j < num_investors; ++j) {
      if(j < solution[i].size()) {
        const Investor* ptr = mp[solution[i][j]];
        buffer[(i*block_size) + j*5] = ptr->getID();
        buffer[(i*block_size) + j*5+1] = ptr->getStrategy();
        buffer[(i*block_size) + j*5+2] = ptr->getAggressiveness();
        buffer[(i*block_size) + j*5+3] = ptr->getMarket();
        buffer[(i*block_size) + j*5+4] = ptr->getBalance();
      } else {
        buffer[(i*block_size) + j*5] = -1;
        buffer[(i*block_size) + j*5+1] = -1;
        buffer[(i*block_size) + j*5+2] = -1;
        buffer[(i*block_size) + j*5+3] = -1;
        buffer[(i*block_size) + j*5+4] = -1;
      }
    }
  }
}

void unpack_investors(const env_t* env, std::vector<Investor>& investors, const int* buffer) {
  investors.clear();
  const int num_investors = 4800 / env->num_nodes;

  for(int i = 0; i < num_investors; ++i) {
    const int* ptr = buffer + i*5;

    int id = ptr[0];
    if(id == -1) {
      break;
    }
    int strategy = ptr[1];
    int aggressiveness = ptr[2];
    int market = ptr[3];
    int balance = ptr[4];

    investors.push_back(Investor(id, strategy, aggressiveness, market, balance));
  }
}

void calculate_stock_prices(const env_t* env, std::vector<Stock>& stocks, int*& buffer) {
  for(int c = 0; c < env->num_cpus; ++c) {
    if(env->cpu && env->cpu_rank == c) {
      buffer = new int[stocks.size() * 2];
      for(size_t i = 0; i < stocks.size(); ++i) {
        int id = stocks[i].getID();
        int price = stocks[i].generatePrice();
        int newPrice = stocks[i].getPrice() + price;

        buffer[i*2] = id;
        buffer[i*2 + 1] = newPrice;

        // printf("Stock %d from %d to %d\n", id, stocks[i].getPrice(), newPrice);
        stocks[i].setPrice(newPrice);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
}

void unpack_prices(std::vector<int>& prices, const int* buffer) {
  const size_t n = prices.size();
  for(size_t i = 0; i < n; ++i) {
    int id = buffer[i*2];
    int price = buffer[i*2+1];
    assert(id < (int)n);
    prices[id] = price;
  }
}

void pack_results(const env_t* env, const std::vector<Investor>& investors, int*& buffer) {
  const size_t num_investors = size_t(4800 / env->num_nodes);

  buffer = new int[num_investors * 2];

  for(size_t i = 0; i < num_investors; ++i) {
    if(i < investors.size()) {
      buffer[i*2] = investors[i].getID();
      buffer[i*2+1] = investors[i].getBalance();
    } else {
      buffer[i*2] = -1;
      buffer[i*2+1] = -1;
    }
  }
}

void unpack_results(const env_t* env, std::vector<Investor>& investors, const int* buffer) {
  const int num_investors = 4800 / env->num_nodes;
  std::unordered_map<int, Investor*> mp;
  for(size_t i = 0; i < investors.size(); ++i) {
    mp[investors[i].getID()] = &investors[i];
  }

  size_t count = 0;
  for(int i = 0; i < num_investors * GPU_COUNT_; ++i) {
    int id = buffer[i*2];
    int balance = buffer[i*2+1];

    if(id == -1) {
      continue;
    }

    Investor* ptr = mp[id];
    ptr->setBalance(balance);
    ++count;
  }

  assert(count == investors.size());
}

#endif
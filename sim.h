#ifndef SIM_H
#define SIM_H

#include <vector>
#include <algorithm>
#include <unordered_map>
#include "macros.h"
#include "mpi.h"
#include "env.h"
#include "timer.h"

extern bool verbose;

typedef std::vector< std::vector<int> > solution_vec;

void read_file(const env_t* env, std::vector<Stock>& stocks, MPI_File& fh) {
  if(env->cpu) {
    const int count = 100000 / env->num_cpus; // 4000
    int* buffer = new int[count * 25];
    MPI_Offset start = env->cpu_rank * (count * 25) * sizeof(int);
    MPI_Status status;

    MPI_File_read_at(fh, start, buffer, 25 * count, MPI_INT, &status);

    for(int i = 0; i < count; ++i) {
      stocks.push_back(Stock(buffer + i*25));
    }

    delete [] buffer;
  }
}

void generate_investors(const env_t* env, std::vector<Investor>& investors) {
  if(env->worker) {
    const int num_investors = 4800 / env->num_nodes;

    investors = std::vector<Investor>(num_investors);
    int offset = env->node * num_investors;
    for(int i = 0; i < num_investors; ++i) {
      int id = offset + i;
      int strategy = id / 800;
      int aggressiveness = id % 8;
      int market = id % 100;

      investors[i] = Investor(id, strategy, aggressiveness, market, 100000);
    }
  }
}

solution_vec load_balance( const env_t* env, const std::vector<Investor>& investors) {
  assert(4800 / env->num_nodes == (int)investors.size());
  solution_vec solution(6);
  std::vector< std::pair<int, int> > vec;
  int weights[GPU_COUNT_];
  for(int i = 0; i < GPU_COUNT_; ++i) {
    weights[i] = 0;
  }

  for(const auto& inv: investors) {
    vec.push_back({inv.getID(), inv.getSpending()});
  }

  //Sort by balance (non increasing)
  std::sort(vec.begin(), vec.end(),
  [] (const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return a.second > b.second || (a.second == b.second && a.first < b.first);
  });

  //Continually add to slot with smallest workload
  for(size_t i = 0; i < vec.size(); ++i) {
    //Find min
    int slot = 0;
    int mn = weights[0];

    for(int j = 1; j < GPU_COUNT_; ++j) {
      if(weights[j] < mn) {
        mn = weights[j];
        slot = j;
      }
    }

    solution[slot].push_back(vec[i].first);
    weights[slot] += vec[i].second;
  }

  return solution;
}

void invest(const env_t* env, std::vector<Investor>& investors, const std::vector<Stock>& stocks) {
  std::vector<Stock> market_stock_arr[100];
  for(size_t i = 0; i < stocks.size(); i += 1000) {
    for(int j = 0; j < 1000; ++j) {
      market_stock_arr[(i + j) / 1000].push_back(stocks[i + j]);
    }
  }

  int stock_values[100][6][1000];

  for(int i = 0; i < 100; ++i) {
    assert(market_stock_arr[i].size() == 1000);
    for(int j = 0; j < 1000; ++j) {
      stock_values[i][0][j] = market_stock_arr[i][j].expectedValue();
      stock_values[i][1][j] = market_stock_arr[i][j].maximumValue();
      stock_values[i][2][j] = market_stock_arr[i][j].minimumValue();
      stock_values[i][3][j] = market_stock_arr[i][j].mostLikelyValue();
      stock_values[i][4][j] = market_stock_arr[i][j].twoMostLikelyValues();
      stock_values[i][5][j] = market_stock_arr[i][j].randomSample();
    }
  }

  for(size_t i = 0; i < investors.size(); ++i) {
    if(investors[i].getBalance() <= 0) {
      continue;
    }

    const std::vector<Stock>& market_stocks = market_stock_arr[investors[i].getMarket()];

    int strategy = investors[i].getStrategy();
    int aggressiveness = investors[i].getAggressiveness();
    int market = investors[i].getMarket();
    int balance = investors[i].getBalance();
    double perc;

    switch(aggressiveness) {
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

    int bal = (int)(double(balance) * perc);
    
    std::vector< std::vector<int> > solution;
    int total;
    if(verbose && env->gpu_rank == 5 && (i % 50 == 0)) {
      printf("Starting knapsack (investor %lu of %lu) with balance %d\n", i, investors.size(), bal);
    }
    int v_temp = verbose;
    verbose = false;
    knapsack(market_stocks, stock_values[market][strategy], solution, total, market_stocks.size(), bal);
    verbose = v_temp;

    investors[i].invest(solution);
    if(verbose && env->gpu_rank == 5 && (i % 50 == 0)) {
      printf("Ending knapsack\n");
    }
  }

  if(verbose && env->gpu_rank == 5) {
    printf("Ending knapsack (final)\n");
  }
}

void evaluate(std::vector<Investor>& investors, std::vector<int>& prices) {
  for(size_t i = 0; i < investors.size(); ++i) {
    investors[i].eval(prices);
  }
}

void write_to_db() {

}

#endif
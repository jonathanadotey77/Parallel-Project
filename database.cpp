#include <iostream>
#include <fstream>
#include <assert.h>

int main(int argc, char** argv) {
  for(int n = 1; n <= 16; n <<= 1) {
    
    int num_gpus = n * 6;
    int num_workers = n;

    for(int g = 0; g < num_gpus; ++g) {
      char filename[100] = {'\0'};

      sprintf(filename, "output/gpu/gpu_%d_with_%d_nodes.out", g, n);
      std::ifstream inFile(filename);
      assert(inFile.is_open());
      int num_nodes, node, round, balance;
      double time;
      while(inFile >> num_nodes) {
        inFile >> node >> round >> balance;
        inFile >> time;
        assert(num_nodes == n);
        //Send tuple to DB
      }
      inFile.close();
    }

    for(int w = 0; w < num_workers; ++w) {
      char filename[100] = {'\0'};

      sprintf(filename, "output/round/worker_%d_with_%d_nodes.out", w, n);

      std::ifstream inFile(filename);
      assert(inFile.is_open());
      int num_nodes, round;
      double time;

      while(inFile >> num_nodes) {
        inFile >> round;
        inFile >> time;
        assert(num_nodes == n);
        //Send tuple to DB
      }
      inFile.close();
    }

    for(int w = 0; w < num_workers; ++w) {
      char filename[100] = {'\0'};

      sprintf(filename, "output/worker/worker_%d_with_%d_nodes.out", w, n);

      std::ifstream inFile(filename);
      assert(inFile.is_open());

      int num_nodes, node, round, id, strategy, aggressiveness, market, balance;
      while(inFile >> num_nodes) {
        inFile >> node >> round >> id
               >> strategy >> aggressiveness
               >> market >> balance;
        assert(num_nodes == n);
        //Send tuple to DB
      }

      inFile.close();
    }

  }
  return 0;
}
#include <iostream>
#include <fstream>
#include <assert.h>
#include "DataController.h"

int main(int argc, char** argv) {
  DataController conn("data.db");
  for(int n = 1; n <= 16; n <<= 1) {
    std::cout << "--------------------------------------------------Inserting for " << n 
    << " nodes--------------------------------------------------"<< std::endl;
    
    int num_gpus = n * 6;
    int num_workers = n;

    for(int g = 0; g < num_gpus; ++g) {
      char filename[100] = {'\0'};

      sprintf(filename, "output/gpu/gpu_%d_with_%d_nodes.out", g, n);
      std::ifstream inFile(filename);
      assert(inFile.is_open());
      int num_nodes, node, round_num, balance;
      double time;
      while(inFile >> num_nodes) {
        inFile >> node >> round_num >> balance;
        inFile >> time;
        assert(num_nodes == n);
        //Send tuple to DB
        conn.insertKnapsackTime(num_nodes, node, round_num, balance, time);
      }
      inFile.close();
    }

     std::cout  << "Knapsick Time Inserted" << std::endl;


    for(int w = 0; w < num_workers; ++w) {
      char filename[100] = {'\0'};

      sprintf(filename, "output/round/worker_%d_with_%d_nodes.out", w, n);

      std::ifstream inFile(filename);
      assert(inFile.is_open());
      int num_nodes, round_num;
      double round_time;
      double io_time;

      while(inFile >> num_nodes) {
        inFile >> round_num;
        inFile >> round_time;
        inFile >>io_time;
        assert(num_nodes == n);
        //Send tuple to DB
        conn.insertRoundTime(num_nodes, round_num, round_time);
        conn.insertWorkerIoTime(num_nodes, round_num, io_time);
      }
      inFile.close();
    }
     std::cout  << "Round Time and Worker Io Time Inserted" << std::endl;

    for(int w = 0; w < num_workers; ++w) {
      char filename[100] = {'\0'};

      sprintf(filename, "output/worker/worker_%d_with_%d_nodes.out", w, n);

      std::ifstream inFile(filename);
      assert(inFile.is_open());

      int num_nodes, node, round_num, id, strategy, aggressiveness, market, balance;
      while(inFile >> num_nodes) {
        inFile >> node >> round_num >> id
               >> strategy >> aggressiveness
               >> market >> balance;
        assert(num_nodes == n);
        //Send tuple to DB
        conn.insertInvestorStatus(num_nodes, node, round_num, id, strategy, aggressiveness, market, balance);
      }

      inFile.close();
    }
    std::cout  << "Investor data inserted" << std::endl;

    for(int g = 0; g < num_gpus; ++g) {
      char filename[100] = {'\0'};

      sprintf(filename, "output/io/gpu_%d_with_%d_nodes.out", g, n);
      std::ifstream inFile(filename);
      assert(inFile.is_open());
      int gpu, num_nodes, round_num;
      double time;
      while(inFile >> num_nodes) {
        inFile >> gpu >> round_num;
        inFile >> time;
        assert(num_nodes == n);
        //Send tuple to DB
        conn.insertGpuIoTime(num_nodes, round_num, time);
      }
      inFile.close();
    }
    std::cout << "Gpu Io Time Inserted" << std::endl;
     std::cout << "-------------------------------------------------- Finished " << n 
    << " nodes--------------------------------------------------"<< std::endl;
  }



  //insert into kernel times
  for(int i = 0; i < 4; ++i){
    char filename[100] = {'\0'};

      sprintf(filename, "k_times/t%d.txt",i);
      std::ifstream inFile(filename);
      assert(inFile.is_open());
      int balance;
      double time;
      while(inFile >> balance) {
        inFile >> time;
        //Send tuple to DB
        conn.insertKernelTime(balance, time);
      }
      inFile.close();
  }

  std::cout << "Kernel Times Inserted" << std::endl;

  //insert into weak time table
  for(int i = 1; i <= 16; i  <<= 1){
    char filename[100] = {'\0'};
    sprintf(filename, "output/weak/%d_nodes.out",i);
    std::ifstream inFile(filename);
    assert(inFile.is_open());
    int num_nodes;
    double time;
    while(inFile >> num_nodes) {
      inFile >> time;
      //Send tuple to DB
      conn.insertWeakTime(num_nodes, time);
    }
    inFile.close();
  }
  std::cout << "Weak Times Inserted" << std::endl;
  return 0;
}
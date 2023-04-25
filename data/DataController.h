//
// Created by Simon Sandrew on 4/20/23.
//
#ifndef FINALPROJECTREPO_DATACONTROLLER_H
#define FINALPROJECTREPO_DATACONTROLLER_H
#include <sqlite3.h>
#include "string"
#include "exception"

class DataController {
public:
    DataController(const std::string& filename);
    void insertRoundTime(int num_nodes, int round_num, double round_time);
    void insertInvestorStatus(int num_nodes,
                              int node_id,
                              int round_num,
                              int investor_id,
                              int investor_strategy,
                              int investor_aggressiveness,
                              int investor_market,
                              int investor_balance);
    void insertKnapsackTime(int num_nodes,
                            int node_id,
                            int round_num,
                            int knapsack_balance,
                            double time);

    void printDatabases() const;
    void deleteAllRowsFromTable(const std::string& tableName);


private:
    sqlite3* conn_;
};


#endif //FINALPROJECTREPO_DATACONTROLLER_H

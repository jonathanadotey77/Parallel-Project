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
    void printDatabases() const;
    void insertIntoKnapsackTimings(int runId, int numNodes, int gpu, int round, int runtimeMS);
    void insertIntoInvestorBalances(int investorId, int roundNum, int balance);
    void incrementWins(const std::string& strategy, int aggressiveness);
    void insertIntoInvestors(int investorId, const std::string& strategy, int aggressiveness);
    void deleteAllRowsFromTable(const std::string& tableName);
private:
    sqlite3* conn_;
};


#endif //FINALPROJECTREPO_DATACONTROLLER_H

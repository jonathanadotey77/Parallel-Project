#include "DataController.h"
#include <iostream>

int main() {
    DataController dataController("test.db");
    // Insert into knapsack_timings
    dataController.insertIntoKnapsackTimings(1, 10, 0, 1, 100);
    dataController.insertIntoKnapsackTimings(2, 20, 1, 2, 200);
    dataController.insertIntoKnapsackTimings(3, 30, 0, 3, 300);

    // Insert into investor_balances
    dataController.insertIntoInvestorBalances(1, 1, 1000);
    dataController.insertIntoInvestorBalances(1, 2, 2000);
    dataController.insertIntoInvestorBalances(2, 1, 1500);

    // Increment win
    dataController.incrementWins("aggressive", 5);
    dataController.incrementWins("conservative", 3);
    dataController.incrementWins("balanced", 4);

    // Insert into investors
    dataController.insertIntoInvestors(1, "aggressive", 5);
    dataController.insertIntoInvestors(2, "balanced", 4);
    dataController.insertIntoInvestors(3, "conservative", 3);

    // Print databases
    dataController.printDatabases();

    // Delete all rows from the knapsack_timings table
    dataController.deleteAllRowsFromTable("knapsack_timings");

    // Delete all rows from the investor_balances table
    dataController.deleteAllRowsFromTable("investor_balances");

    // Delete all rows from the wins table
    dataController.deleteAllRowsFromTable("wins");

    // Delete all rows from the investors table
    dataController.deleteAllRowsFromTable("investors");

    return 0;
}

//
// Created by Simon Sandrew on 4/20/23.
//

#include "DataController.h"
#include "sqlite3.h"
#include <iomanip>
#include <iostream>
DataController::DataController(const std::string &filename) {
    // Open the database connection

    int rc = sqlite3_open_v2(filename.c_str(), &conn_,
                             SQLITE_OPEN_READWRITE  | SQLITE_OPEN_NOMUTEX,
                             nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: "
        + std::string(sqlite3_errmsg(conn_)));
    }
}
void DataController::printDatabases() const   {
    // Get the list of tables in the database
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(conn_, "SELECT "
                                                "name "
                                            "FROM "
                                            "sqlite_master "
                                            "WHERE type='table';",
                                            -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::runtime_error("Failed to fetch tables: " + std::string(sqlite3_errmsg(conn_)));
    }

    // For each table, get the number of rows and the three most recent entries
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* tableName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::cout << "Table: " << tableName << '\n';

        // Get the number of rows in the table
        sqlite3_stmt* countStmt;
        std::string countSql = "SELECT COUNT(*) FROM " + std::string(tableName) + ";";
        rc = sqlite3_prepare_v2(conn_, countSql.c_str(), -1, &countStmt, nullptr);
        if (rc != SQLITE_OK) {
            std::runtime_error("Failed to fetch row count: " + std::string(sqlite3_errmsg(conn_)));
        }
        rc = sqlite3_step(countStmt);
        if (rc != SQLITE_ROW) {
            std::runtime_error("Failed to fetch row count: " + std::string(sqlite3_errmsg(conn_)));
        }
        int rowCount = sqlite3_column_int(countStmt, 0);
        std::cout << "  Number of rows: " << rowCount << '\n';

        // Get the three most recent entries in the table
        sqlite3_stmt* recentStmt;
        std::string recentSql = "SELECT * FROM " + std::string(tableName) + " ORDER BY rowid DESC LIMIT 3;";
        rc = sqlite3_prepare_v2(conn_, recentSql.c_str(), -1, &recentStmt, nullptr);
        if (rc != SQLITE_OK) {
            std::runtime_error("Failed to fetch recent rows: " + std::string(sqlite3_errmsg(conn_)));
        }
        std::cout << "  Most recent entries:\n";
        while (sqlite3_step(recentStmt) == SQLITE_ROW) {
            std::cout << "    ";
            for (int i = 0; i < sqlite3_column_count(recentStmt); i++) {
                const char* columnName = sqlite3_column_name(recentStmt, i);
                const char* value = reinterpret_cast<const char*>(sqlite3_column_text(recentStmt, i));
                std::cout << columnName << ": " << value << "| ";
            }
            std::cout << '\n';
        }

        sqlite3_finalize(countStmt);
        sqlite3_finalize(recentStmt);
    }

    sqlite3_finalize(stmt);
}


void DataController::insertIntoKnapsackTimings(
        int runId, int numNodes, int gpu, int round, int runtimeMS) {
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(conn_, "INSERT INTO knapsack_timings("
                                               "runId"
                                               ",numNodes"
                                               ",gpu"
                                               ",round"
                                               ",runtimeMS"
                                             ") VALUES (?, ?, ?, ?, ?);",
                                       -1,
                                       &stmt,
                                       nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare insert statement: " +
        std::string(sqlite3_errmsg(conn_)));
    }

    rc = sqlite3_bind_int(stmt, 1, runId);
    rc |= sqlite3_bind_int(stmt, 2, numNodes);
    rc |= sqlite3_bind_int(stmt, 3, gpu);
    rc |= sqlite3_bind_int(stmt, 4, round);
    rc |= sqlite3_bind_int(stmt, 5, runtimeMS);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind values to insert statement: " + std::string(sqlite3_errmsg(conn_)));
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        throw std::runtime_error("Failed to execute insert statement: " + std::string(sqlite3_errmsg(conn_)));
    }

    sqlite3_finalize(stmt);
}

void DataController::insertIntoInvestorBalances(int investorId, int roundNum, int balance) {
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(conn_, "INSERT INTO investor_balances("
                                                "investorId"
                                                ",roundNum"
                                                ",balance"
                                              ") VALUES (?, ?, ?);",
                                       -1,
                                       &stmt,
                                       nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare insert statement: " + std::string(sqlite3_errmsg(conn_)));
    }

    rc = sqlite3_bind_int(stmt, 1, investorId);
    rc |= sqlite3_bind_int(stmt, 2, roundNum);
    rc |= sqlite3_bind_int(stmt, 3, balance);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind values to insert statement: " + std::string(sqlite3_errmsg(conn_)));
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        throw std::runtime_error("Failed to execute insert statement: " + std::string(sqlite3_errmsg(conn_)));
    }

    sqlite3_finalize(stmt);
}

void DataController::insertIntoInvestors(int investorId, const std::string& strategy, int aggressiveness) {
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(conn_, "INSERT INTO investors("
                                               "investorId"
                                               ",strategy"
                                               ",aggressiveness"
                                            ") VALUES (?, ?, ?);", -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare insert statement: " + std::string(sqlite3_errmsg(conn_)));
    }

    rc = sqlite3_bind_int(stmt, 1, investorId);
    rc |= sqlite3_bind_text(stmt, 2, strategy.c_str(), -1, SQLITE_TRANSIENT);
    rc |= sqlite3_bind_int(stmt, 3, aggressiveness);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind values to insert statement: " + std::string(sqlite3_errmsg(conn_)));
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        throw std::runtime_error("Failed to execute insert statement: " + std::string(sqlite3_errmsg(conn_)));
    }

    sqlite3_finalize(stmt);
}

void DataController::incrementWins(const std::string& strategy, int aggressiveness) {
    // Check if a row with the given strategy and aggressiveness exists in the wins table
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(conn_,
                                "SELECT wins FROM wins WHERE strategy = ? AND aggressiveness = ?;",
                                -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare select statement: " +
                                 std::string(sqlite3_errmsg(conn_)));
    }

    rc = sqlite3_bind_text(stmt, 1, strategy.c_str(), -1, SQLITE_TRANSIENT);
    rc |= sqlite3_bind_int(stmt, 2, aggressiveness);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to bind values to select statement: " +
                                 std::string(sqlite3_errmsg(conn_)));
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // A row with the given strategy and aggressiveness exists, so increment the wins value
        int wins = sqlite3_column_int(stmt, 0) + 1;
        sqlite3_finalize(stmt);

        // Update the row with the new wins value
        rc = sqlite3_prepare_v2(conn_,
                                "UPDATE wins SET wins = ? WHERE strategy = ? AND aggressiveness = ?;",
                                -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare update statement: " +
                                     std::string(sqlite3_errmsg(conn_)));
        }

        rc = sqlite3_bind_int(stmt, 1, wins);
        rc |= sqlite3_bind_text(stmt, 2, strategy.c_str(), -1,
                                SQLITE_TRANSIENT);
        rc |= sqlite3_bind_int(stmt, 3, aggressiveness);
        if (rc != SQLITE_OK) {
            throw std::runtime_error(
                    "Failed to bind values to update statement: " +
                    std::string(sqlite3_errmsg(conn_)));
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to execute update statement: " +
                                     std::string(sqlite3_errmsg(conn_)));
        }
    } else if (rc == SQLITE_DONE) {
        // A row with the given strategy and aggressiveness does not exist, so insert a new row with a wins value of 1
        sqlite3_finalize(stmt);

        rc = sqlite3_prepare_v2(conn_,
                                "INSERT INTO wins(strategy, aggressiveness, wins) VALUES (?, ?, ?);",
                                -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare insert statement: " +
                                     std::string(sqlite3_errmsg(conn_)));
        }

        rc = sqlite3_bind_text(stmt, 1, strategy.c_str(), -1, SQLITE_TRANSIENT);
        rc |= sqlite3_bind_int(stmt, 2, aggressiveness);
        rc |= sqlite3_bind_int(stmt, 3, 1);
        if (rc != SQLITE_OK) {
            throw std::runtime_error(
                    "Failed to bind values to insert statement: " +
                    std::string(sqlite3_errmsg(conn_)));
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to execute insert statement: " +
                                     std::string(sqlite3_errmsg(conn_)));
        }
    } else {
        throw std::runtime_error("Failed to execute select statement: " +
                                 std::string(sqlite3_errmsg(conn_)));
    }
}
void DataController::deleteAllRowsFromTable(const std::string& tableName) {
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(conn_, ("DELETE FROM " + tableName + ";").c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare delete statement: " + std::string(sqlite3_errmsg(conn_)));
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        throw std::runtime_error("Failed to execute delete statement: " + std::string(sqlite3_errmsg(conn_)));
    }

    sqlite3_finalize(stmt);
}









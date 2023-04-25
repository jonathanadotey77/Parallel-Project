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


void DataController::insertRoundTime(int num_nodes, int round_num, double round_time) {
    std::string query = "INSERT INTO round_times(num_nodes, round_num, round_time) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(conn_, query.c_str(), -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_bind_int(stmt, 1, num_nodes);
    sqlite3_bind_int(stmt, 2, round_num);
    sqlite3_bind_double(stmt, 3, round_time);
    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        throw std::runtime_error("Error executing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_finalize(stmt);
}

void DataController::insertInvestorStatus(int num_nodes, int node_id, int round_num, int investor_id,
                          int investor_strategy, int investor_aggressiveness, int investor_market,
                          int investor_balance) {
    std::string query = "INSERT INTO investor_status(num_nodes, node_id, round_num, investor_id, investor_strategy, investor_aggressiveness, investor_market, investor_balance) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(conn_, query.c_str(), -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_bind_int(stmt, 1, num_nodes);
    sqlite3_bind_int(stmt, 2, node_id);
    sqlite3_bind_int(stmt, 3, round_num);
    sqlite3_bind_int(stmt, 4, investor_id);
    sqlite3_bind_int(stmt, 5, investor_strategy);
    sqlite3_bind_int(stmt, 6, investor_aggressiveness);
    sqlite3_bind_int(stmt, 7, investor_market);
    sqlite3_bind_int(stmt, 8, investor_balance);
    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        throw std::runtime_error("Error executing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_finalize(stmt);
}

void DataController::insertKnapsackTime(int num_nodes, int node_id, int round_num, int knapsack_balance, double time) {
    std::string query = "INSERT INTO knapsack_times(num_nodes, node_id, round_num, knapsack_balance, time) VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(conn_, query.c_str(), -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_bind_int(stmt, 1, num_nodes);
    sqlite3_bind_int(stmt, 2, node_id);
    sqlite3_bind_int(stmt, 3, round_num);
    sqlite3_bind_int(stmt, 4, knapsack_balance);
    sqlite3_bind_double(stmt, 5, time);
    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        throw std::runtime_error("Error executing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_finalize(stmt);
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

void DataController::insertGpuIoTime(int num_nodes, int round, double time) {
    std::string query = "INSERT INTO gpu_io_times(num_nodes, round, time) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(conn_, query.c_str(), -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_bind_int(stmt, 1, num_nodes);
    sqlite3_bind_int(stmt, 2, round);
    sqlite3_bind_double(stmt, 3, time);
    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        throw std::runtime_error("Error executing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_finalize(stmt);
}

void DataController::insertWorkerIoTime(int num_nodes, int round, double time) {
    std::string query = "INSERT INTO worker_io_times(num_nodes, round, time) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(conn_, query.c_str(), -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_bind_int(stmt, 1, num_nodes);
    sqlite3_bind_int(stmt, 2, round);
    sqlite3_bind_double(stmt, 3, time);
    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        throw std::runtime_error("Error executing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_finalize(stmt);
}

void DataController::insertKernelTime(int balance, double time) {
    std::string query = "INSERT INTO kernel_times(balance, time) VALUES (?, ?)";
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(conn_, query.c_str(), -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_bind_int(stmt, 1, balance);
    sqlite3_bind_double(stmt, 2, time);
    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        throw std::runtime_error("Error executing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_finalize(stmt);
}

void DataController::insertWeakTime(int num_nodes, double time) {
    std::string query = "INSERT INTO weak_times(num_nodes, time) VALUES (?, ?)";
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(conn_, query.c_str(), -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_bind_int(stmt, 1, num_nodes);
    sqlite3_bind_double(stmt, 2, time);
    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        throw std::runtime_error("Error executing statement: " + std::string(sqlite3_errmsg(conn_)));
    }
    sqlite3_finalize(stmt);
}


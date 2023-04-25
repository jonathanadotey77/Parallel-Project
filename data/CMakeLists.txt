cmake_minimum_required(VERSION 3.20)
project(FinalProjectRepo)

set(CMAKE_CXX_STANDARD 14)

add_executable(FinalProjectRepo  database.cpp DataController.cpp)
set(CMAKE_CXX_FLAGS "-lsqlite3")

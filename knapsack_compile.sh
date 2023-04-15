#!/bin/bash -x

nvcc -gencode arch=compute_70,code=sm_70 -O3 knapsack.cu knapsack_test.cu -o knapsack

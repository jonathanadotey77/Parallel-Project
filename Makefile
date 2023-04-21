all: main.cpp knapsack.cu
	mpic++ -O3 main.cpp -c -o main.o -std=c++11 -Wall -Wextra
	nvcc -O3 -gencode arch=compute_70,code=sm_70 knapsack.cu -c -o knapsack.o
	mpic++ -O3 main.o knapsack.o -o simulate \
-L/usr/local/cuda-11.2/lib64/ -lcudadevrt -lcudart -lstdc++

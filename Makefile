all: main.cpp stock.cpp knapsack.cu
	mpic++ -O3 main.cpp -c -o main.o -std=c++11
	mpic++ -O3 stock.cpp -c -o stock.o -std=c++11
	nvcc -O3 knapsack.cu -c -o knapsack.o
	mpic++ -O3 main.o knapsack.o stock.o -o simulate \
-L/usr/local/cuda-11.2/lib64/ -lcudadevrt -lcudart -lstdc++
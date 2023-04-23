#include "timer.h"

double calc_time(time_point start, time_point end) {
  std::chrono::duration<double> diff = end - start;
  return diff.count();
}

time_point clock_time() {
  return std::chrono::high_resolution_clock::now();
}
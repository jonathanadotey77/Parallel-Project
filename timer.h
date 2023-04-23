#ifndef TIMER_H
#define TIMER_H

#include <chrono>

using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

double calc_time(time_point start, time_point end);

time_point clock_time();

#endif
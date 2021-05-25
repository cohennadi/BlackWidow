#pragma once

#include <chrono>
#include <vector>

template <class T>
float getTime_ms(const T & tStart, const T & tEnd) {
    return ((float)(std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tStart).count()))/1000.0;
}

std::vector<char> readFile(const char* filename);

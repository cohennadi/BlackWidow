#pragma once

#ifdef _DEBUG
#include <iostream>
#define LOG(log_message) do {std::cout << (log_message) << std::endl;} while(false)
#else
#define LOG(log_message)
#endif
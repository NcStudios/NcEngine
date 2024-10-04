#pragma once

#include "fmt/format.h"

#include <iostream>

#define LOG(...) std::cout << fmt::format(__VA_ARGS__) << '\n';

#pragma once

#include <cstdlib>
#include <memory>

namespace nc
{
template<class T>
using unique_c_ptr = std::unique_ptr<T, decltype(&::free)>;
}
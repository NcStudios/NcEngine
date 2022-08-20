#pragma once

#include <cstdlib>
#include <memory>

namespace nc
{
using unique_stbi = std::unique_ptr<unsigned char[], decltype(&::free)>;
// using unique_stbi = std::unique_ptr<unsigned char[], decltype([](void* ptr){ ::free(ptr); })>;
}
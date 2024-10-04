#pragma once

#include <cstdint>

inline auto ReadPixel(const unsigned char* data, size_t position) -> uint32_t
{
    const auto r = uint32_t{data[position++]} << 24;
    const auto g = uint32_t{data[position++]} << 16;
    const auto b = uint32_t{data[position++]} << 8;
    const auto a = uint32_t{data[position]};
    return r | g | b | a;
}

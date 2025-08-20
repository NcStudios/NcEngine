/**
 * @file New.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "Platform.h"

#include <new>

#ifdef __cpp_lib_hardware_interference_size
    #ifdef NC_COMPILER_GCC
        // GCC issues diagnostics on any ODR use of constants in 'std' namespace b/c they are tuneable values.
        #define NC_DESTRUCTIVE_INTERFERENCE_SIZE  __GCC_DESTRUCTIVE_SIZE
        #define NC_CONSTRUCTIVE_INTERFERENCE_SIZE __GCC_CONSTRUCTIVE_SIZE
    #else
        #define NC_DESTRUCTIVE_INTERFERENCE_SIZE  std::hardware_destructive_interference_size
        #define NC_CONSTRUCTIVE_INTERFERENCE_SIZE std::hardware_constructive_interference_size
    #endif
#else
    #define NC_DESTRUCTIVE_INTERFERENCE_SIZE  64
    #define NC_CONSTRUCTIVE_INTERFERENCE_SIZE 64
#endif

namespace nc
{
inline constexpr auto hardware_destructive_interference_size  = NC_DESTRUCTIVE_INTERFERENCE_SIZE;
inline constexpr auto hardware_constructive_interference_size = NC_CONSTRUCTIVE_INTERFERENCE_SIZE;
} // namespace nc

#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #define NC_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define NC_PLATFORM_LINUX
#endif

#if defined(_MSC_VER)
    #define NC_COMPILER_MSVC
#elif defined(__GNUC__)
    #define NC_COMPILER_GCC
#elif defined(__clang__)
    #define NC_COMPILER_CLANG
#endif

#if defined(NC_COMPILER_MSVC)
    #define NC_FORCE_INLINE __forceinline
    #define NC_NO_INLINE __declspec(noinline)
#elif defined(NC_COMPILER_GCC)
    #define NC_FORCE_INLINE __inline__ __attribute__((always_inline))
    #define NC_NO_INLINE __attribute__((noinline))
#elif defined(NC_COMPILER_CLANG)
    #define NC_FORCE_INLINE __inline__ __attribute__((always_inline))
    #define NC_NO_INLINE __attribute__((noinline))
#endif

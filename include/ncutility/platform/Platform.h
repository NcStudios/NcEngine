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
#endif

#if defined(NC_COMPILER_MSVC)
    #define NC_FORCE_INLINE __forceinline
    #define NC_NO_INLINE __declspec(noinline)
#elif defined(NC_COMPILER_GCC)
    #define NC_FORCE_INLINE __inline__ __attribute__((always_inline))
    #define NC_NO_INLINE __attribute__((noinline))
#endif

#if defined(NC_COMPILER_MSVC)
    #define NC_PRAGMA(x) __pragma(x)
    #define NC_DISABLE_WARNING_PUSH NC_PRAGMA(warning (push))
    #define NC_DISABLE_WARNING_POP NC_PRAGMA(warning (pop))
    #define NC_DISABLE_WARNING_MSVC(w) NC_PRAGMA(warning (disable : w))
#else
    #define NC_DISABLE_WARNING_MSVC(w)
#endif

#if defined(NC_COMPILER_GCC)
    #define NC_PRAGMA(x) _Pragma(#x)
    #define NC_DISABLE_WARNING_PUSH NC_PRAGMA(GCC diagnostic push)
    #define NC_DISABLE_WARNING_POP NC_PRAGMA(GCC diagnostic pop)
    #define NC_DISABLE_WARNING_GCC(w) NC_PRAGMA(GCC diagnostic ignored w)
#else
    #define NC_DISABLE_WARNING_GCC(w)
#endif

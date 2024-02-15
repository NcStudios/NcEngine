#pragma once

#ifdef _MSC_VER
#define NC_NO_INLINE __declspec(noinline)
#else
#define NC_NO_INLINE __attribute__((noinline))
#endif

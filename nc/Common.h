#pragma once
#include <stdint.h>
#include <typeinfo>

namespace nc
{
    //using ComponentHandle = uint32_t;
    typedef uint32_t ComponentHandle;
    typedef uint32_t EntityHandle;
    using ComponentIndex = uint32_t;
    //using EntityHandle = uint32_t;
    using ComponentType = const char*;
}
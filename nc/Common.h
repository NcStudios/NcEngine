#pragma once
#include <stdint.h>
#include <typeinfo>

namespace nc
{
    using EntityHandle = uint32_t;
    using ComponentHandle = uint32_t;
    const ComponentHandle NullHandle = 0u;
    using ComponentIndex = uint32_t;
    using ComponentType = const char*;
}
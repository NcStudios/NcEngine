#pragma once
#include <stdint.h>
#include <typeinfo>

namespace nc
{
    using EntityHandle = uint32_t;
    using ComponentHandle = uint32_t;
    const ComponentHandle NullHandle = 0u;
    using ComponentType = const char*;

    /** Currently this is only important for component allocators.
     *  It could be moved there if that stays true. */
    enum class MemoryState : uint8_t
    {
        Invalid = 0u,
        Valid = 1u
    };
}
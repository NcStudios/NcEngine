#pragma once

#include <stdint.h>

namespace nc
{
    class EntityHandle
    {
        public:
            using Handle_t = uint32_t;

            explicit constexpr EntityHandle(Handle_t value) : m_value{value} {}
            explicit constexpr operator Handle_t() const { return m_value; }
            friend auto constexpr operator<=>(const EntityHandle&, const EntityHandle&) = default;
            static constexpr EntityHandle Invalid() { return EntityHandle{s_nullValue}; }

            struct Hash
            {
                size_t operator()(const EntityHandle& handle) const
                {
                    return static_cast<size_t>(static_cast<Handle_t>(handle));
                }
            };

        private:
            static constexpr Handle_t s_nullValue = 0u;
            const Handle_t m_value;
    };
}
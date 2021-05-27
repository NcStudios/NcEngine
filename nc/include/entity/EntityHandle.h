#pragma once

#include <limits>
#include <cstdint>

namespace nc
{
    struct HandleTraits
    {
        using handle_type = uint64_t;
        using index_type = uint32_t;
        using version_type = uint16_t;
        using layer_type = uint8_t;
        using flags_type = uint8_t;

        static constexpr handle_type NullHandle = std::numeric_limits<handle_type>::max();
        static constexpr index_type NullIndex = std::numeric_limits<index_type>::max();
        static constexpr size_t FlagsShift = 56u;
        static constexpr size_t LayerShift = 48u;
        static constexpr size_t VersionShift = 32u;
    };

    struct EntityFlags
    {
        static constexpr HandleTraits::flags_type None = 0b00000000;
        static constexpr HandleTraits::flags_type Static = 0b00000001;
    };

    class EntityHandle
    {
        public:
            static constexpr EntityHandle Null() noexcept
            {
                return EntityHandle{HandleTraits::NullHandle};
            }
            
            explicit constexpr EntityHandle(HandleTraits::handle_type handle) noexcept
                : m_handle{handle}
            {}
            
            constexpr bool Valid() const noexcept
            {
                return m_handle != HandleTraits::NullHandle;
            }

            explicit constexpr operator HandleTraits::handle_type() const noexcept
            {
                return m_handle;
            }

            friend auto constexpr operator<=>(const EntityHandle&, const EntityHandle&) = default;

        private:
            HandleTraits::handle_type m_handle;
            friend struct HandleUtils;
    };
}
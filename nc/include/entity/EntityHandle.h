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

    /** @todo Maybe move some of these functions so only the engine can see them. I 
     *  don't think game logic should every care about handle details. */

    class EntityHandle
    {
        using handle_type = HandleTraits::handle_type;
        using index_type = HandleTraits::index_type;
        using version_type = HandleTraits::version_type;
        using layer_type = HandleTraits::layer_type;
        using flags_type = HandleTraits::flags_type;

        public:
            static constexpr EntityHandle Null() noexcept;
            explicit constexpr EntityHandle(index_type index, version_type version, layer_type layer, flags_type flags) noexcept;
            explicit constexpr EntityHandle(HandleTraits::handle_type handle) noexcept;
            explicit constexpr operator HandleTraits::handle_type() const noexcept;

            friend auto constexpr operator<=>(const EntityHandle&, const EntityHandle&) = default;
            [[nodiscard]] constexpr bool Valid() const noexcept;
            [[nodiscard]] constexpr auto Index() const noexcept -> index_type;
            [[nodiscard]] constexpr auto Version() const noexcept -> version_type;
            [[nodiscard]] constexpr auto Layer() const noexcept -> layer_type;
            [[nodiscard]] constexpr auto Flags() const noexcept -> flags_type;
            [[nodiscard]] constexpr bool IsStatic() const noexcept;

            constexpr void SetVersion(version_type version) noexcept;
            constexpr void Recycle(layer_type layer, flags_type flags) noexcept;

        private:
            handle_type m_handle;

            constexpr auto Join(index_type index, version_type version, layer_type layer, flags_type flags) const noexcept -> handle_type;
    };

    constexpr EntityHandle EntityHandle::Null() noexcept
    {
        return EntityHandle{HandleTraits::NullHandle};
    }

    constexpr EntityHandle::EntityHandle(index_type index, version_type version, layer_type layer, flags_type flags) noexcept
        : m_handle{Join(index, version, layer, flags)}
    {
    }

    constexpr EntityHandle::EntityHandle(handle_type handle) noexcept
        : m_handle{handle}
    {
    }

    constexpr EntityHandle::operator handle_type() const noexcept
    {
        return m_handle;
    }

    constexpr bool EntityHandle::Valid() const noexcept
    {
        return m_handle != HandleTraits::NullHandle;
    }

    // can do better
    constexpr void EntityHandle::SetVersion(version_type version) noexcept
    {
        m_handle = Join(Index(), version, Layer(), Flags());
    }

    constexpr void EntityHandle::Recycle(layer_type layer, flags_type flags) noexcept
    {
        m_handle = Join(Index(), Version() + 1u, layer, flags);
    }

    constexpr auto EntityHandle::Index() const noexcept -> index_type
    {
        return static_cast<index_type>(m_handle);
    }

    constexpr auto EntityHandle::Version() const noexcept -> version_type
    {
        return static_cast<version_type>(m_handle >> HandleTraits::VersionShift);
    }

    constexpr auto EntityHandle::Layer() const noexcept -> layer_type
    {
        return static_cast<layer_type>(m_handle >> HandleTraits::LayerShift);
    }

    constexpr auto EntityHandle::Flags() const noexcept -> flags_type
    {
        return static_cast<flags_type>(m_handle >> HandleTraits::FlagsShift);
    }

    constexpr bool EntityHandle::IsStatic() const noexcept
    {
        return static_cast<bool>(Flags() & EntityFlags::Static);
    }

    constexpr auto EntityHandle::Join(index_type index, version_type version, layer_type layer, flags_type flags) const noexcept -> handle_type
    {
        return (handle_type)flags   << HandleTraits::FlagsShift   |
               (handle_type)layer   << HandleTraits::LayerShift   |
               (handle_type)version << HandleTraits::VersionShift |
               (handle_type)index;
    }
}
#pragma once

#include "math/Vector.h"
#include "math/Quaternion.h"

#include <concepts>
#include <cstdint>
#include <limits>
#include <string>

namespace nc
{
    /** Traits describing the underlying value of an Entity. */
    struct EntityTraits
    {
        using underlying_type = uint64_t;
        using index_type = uint32_t;
        using version_type = uint16_t;
        using layer_type = uint8_t;
        using flags_type = uint8_t;

        static constexpr underlying_type NullHandle = std::numeric_limits<underlying_type>::max();
        static constexpr index_type NullIndex = std::numeric_limits<index_type>::max();
        static constexpr size_t FlagsShift = 56u;
        static constexpr size_t LayerShift = 48u;
        static constexpr size_t VersionShift = 32u;
    };

    /** Concepts to enforce exact matches to types described by EntityTraits. */
    template<class T>
    concept explicit_index_type = std::same_as<T, EntityTraits::index_type>;

    template<class T>
    concept explicit_version_type = std::same_as<T, EntityTraits::version_type>;

    template<class T>
    concept explicit_layer_type = std::same_as<T, EntityTraits::layer_type>;

    struct EntityFlags
    {
        static constexpr EntityTraits::flags_type None = 0b00000000;
        static constexpr EntityTraits::flags_type Static = 0b00000001;
    };

    /** A wrapper around an integer used to identify objects. */
    class Entity
    {
        public:
            explicit constexpr Entity(EntityTraits::underlying_type handle) noexcept;
            static constexpr Entity Null() noexcept;
            explicit constexpr operator EntityTraits::underlying_type() const noexcept;
            friend auto constexpr operator<=>(const Entity&, const Entity&) = default;
            constexpr bool Valid() const noexcept;

        private:
            EntityTraits::underlying_type m_handle;
            friend struct EntityUtils;
    };

    /** Initialization data for Entities.
     *  - Rotation must be a normalized quaternion.
     *  - Scale must not be zero in any dimension.
     *  - Layer must be in the range [1, 64]. */
    struct EntityInfo
    {
        Vector3 position = Vector3::Zero();
        Quaternion rotation = Quaternion::Identity();
        Vector3 scale = Vector3::One();
        Entity parent = Entity::Null();
        std::string tag = "Entity";
        EntityTraits::layer_type layer = 1u;
        EntityTraits::flags_type flags = 0u;
    };

    /** Utilities for working with an Entity's underlying value. */
    struct EntityUtils
    {
        using underlying_type = EntityTraits::underlying_type;
        using index_type = EntityTraits::index_type;
        using version_type = EntityTraits::version_type;
        using layer_type = EntityTraits::layer_type;
        using flags_type = EntityTraits::flags_type;

        [[nodiscard]] static constexpr auto Index(Entity entity) noexcept -> index_type;
        [[nodiscard]] static constexpr auto Version(Entity entity) noexcept -> version_type;
        [[nodiscard]] static constexpr auto Layer(Entity entity) noexcept -> layer_type;
        [[nodiscard]] static constexpr auto Flags(Entity entity) noexcept -> flags_type;
        [[nodiscard]] static constexpr bool IsStatic(Entity entity) noexcept;
        [[nodiscard]] static constexpr auto SetVersion(Entity entity, version_type version) noexcept -> Entity;
        [[nodiscard]] static constexpr auto Recycle(Entity entity, layer_type layer, flags_type flags) noexcept -> Entity;
        [[nodiscard]] static constexpr auto Join(index_type index, version_type version, layer_type layer, flags_type flags) noexcept -> underlying_type;
    };

    constexpr Entity::Entity(EntityTraits::underlying_type handle) noexcept
        : m_handle{handle}
    {
    }
    
    constexpr Entity Entity::Null() noexcept
    {
        return Entity{EntityTraits::NullHandle};
    }

    constexpr Entity::operator EntityTraits::underlying_type() const noexcept
    {
        return m_handle;
    }

    constexpr bool Entity::Valid() const noexcept
    {
        return m_handle != EntityTraits::NullHandle;
    }

    // can do better
    constexpr auto EntityUtils::SetVersion(Entity entity, version_type version) noexcept -> Entity
    {
        entity.m_handle = Join(Index(entity), version, Layer(entity), Flags(entity));
        return entity;
    }

    constexpr auto EntityUtils::Recycle(Entity entity, layer_type layer, flags_type flags) noexcept -> Entity
    {
        entity.m_handle = Join(Index(entity), Version(entity) + 1u, layer, flags);
        return entity;
    }

    constexpr auto EntityUtils::Index(Entity entity) noexcept -> index_type
    {
        return static_cast<index_type>(entity.m_handle);
    }

    constexpr auto EntityUtils::Version(Entity entity) noexcept -> version_type
    {
        return static_cast<version_type>(entity.m_handle >> EntityTraits::VersionShift);
    }

    constexpr auto EntityUtils::Layer(Entity entity) noexcept -> layer_type
    {
        return static_cast<layer_type>(entity.m_handle >> EntityTraits::LayerShift);
    }

    constexpr auto EntityUtils::Flags(Entity entity) noexcept -> flags_type
    {
        return static_cast<flags_type>(entity.m_handle >> EntityTraits::FlagsShift);
    }

    constexpr bool EntityUtils::IsStatic(Entity entity) noexcept
    {
        return static_cast<bool>(Flags(entity) & EntityFlags::Static);
    }

    constexpr auto EntityUtils::Join(index_type index, version_type version, layer_type layer, flags_type flags) noexcept -> underlying_type
    {
        return static_cast<underlying_type>(flags)   << EntityTraits::FlagsShift   |
               static_cast<underlying_type>(layer)   << EntityTraits::LayerShift   |
               static_cast<underlying_type>(version) << EntityTraits::VersionShift |
               static_cast<underlying_type>(index);
    }
}
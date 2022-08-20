#pragma once

#include "math/Vector.h"
#include "math/Quaternion.h"

#include <concepts>
#include <cstdint>
#include <limits>
#include <string>

namespace nc
{
/** Identifies an object in the registry. */
class Entity
{
    public:
        using index_type = uint32_t;
        using layer_type = uint8_t;
        using flags_type = uint8_t;

        static constexpr index_type NullIndex = std::numeric_limits<index_type>::max();

        struct Flags
        {
            static constexpr Entity::flags_type None                     = 0b00000000;
            static constexpr Entity::flags_type Static                   = 0b00000001;
            static constexpr Entity::flags_type Persistent               = 0b00000010;
            static constexpr Entity::flags_type NoCollisionNotifications = 0b00000100;
        };

        explicit constexpr Entity() noexcept
            : m_index{NullIndex}, m_layer{0}, m_flags{Flags::None}
        {
        }

        explicit constexpr Entity(index_type index, layer_type layer, flags_type flags) noexcept
            : m_index{index}, m_layer{layer}, m_flags{flags}
        {
        }

        static constexpr auto Null() noexcept { return Entity{}; }
        constexpr auto Valid() const noexcept { return m_index != NullIndex; }
        constexpr auto Index() const noexcept { return m_index; }
        constexpr auto Layer() const noexcept { return m_layer; }
        constexpr auto Flags() const noexcept { return m_flags; }
        constexpr auto IsStatic() const noexcept { return m_flags & Flags::Static; }
        constexpr auto IsPersistent() const noexcept { return m_flags & Flags::Persistent; }
        constexpr auto ReceivesCollisionEvents() const noexcept { return !(m_flags & Flags::NoCollisionNotifications); }
        explicit constexpr operator index_type() const noexcept { return m_index; }
        friend bool constexpr operator==(const Entity& a, const Entity& b) { return a.Index() == b.Index(); }
        friend bool constexpr operator!=(const Entity& a, const Entity& b) { return !(a == b); }

    private:
        index_type m_index;
        layer_type m_layer;
        flags_type m_flags;
};

/** Concepts to enforce exact matches to Entity member types. */
template<class T>
concept explicit_index_type = std::same_as<T, Entity::index_type>;

template<class T>
concept explicit_layer_type = std::same_as<T, Entity::layer_type>;

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
    Entity::layer_type layer = 1u;
    Entity::flags_type flags = Entity::Flags::None;
};
} // namespace nc

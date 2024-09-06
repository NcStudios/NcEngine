#pragma once

#include "ncmath/Vector.h"
#include "ncmath/Quaternion.h"

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
            static constexpr Entity::flags_type None                     = 0b00000000; // Default behavior
            static constexpr Entity::flags_type Static                   = 0b00000001; // The Entitiy's Transform will not be moved after construction
            static constexpr Entity::flags_type Persistent               = 0b00000010; // Entity persists across scene load/unload
            static constexpr Entity::flags_type NoCollisionNotifications = 0b00000100; // Do not send Collision/Trigger events involving the Entity
            static constexpr Entity::flags_type NoSerialize              = 0b00001000; // Exclude the Entity and its children from scene serialization
            static constexpr Entity::flags_type Internal                 = 0b00010000; // Entity was created by the engine/editor
        };

        constexpr Entity() noexcept
            : m_index{NullIndex}, m_layer{0}, m_flags{Flags::None}
        {
        }

        explicit constexpr Entity(index_type index, layer_type layer, flags_type flags) noexcept
            : m_index{index}, m_layer{layer}, m_flags{flags}
        {
        }

        static constexpr auto FromHash(uint64_t hash) noexcept -> Entity
        {
            const auto index = static_cast<Entity::index_type>((hash >> 16) & 0xFFFFFFFF);
            const auto layer = static_cast<Entity::layer_type>((hash >> 8) & 0xFF);
            const auto flags = static_cast<Entity::flags_type>(hash & 0xFF);
            return Entity{index, layer, flags};
        }

        static constexpr auto Null() noexcept { return Entity{}; }
        constexpr auto Valid() const noexcept { return m_index != NullIndex; }
        constexpr auto Index() const noexcept { return m_index; }
        constexpr auto Layer() const noexcept { return m_layer; }
        constexpr auto Flags() const noexcept { return m_flags; }
        constexpr auto IsStatic() const noexcept -> bool { return m_flags & Flags::Static; }
        constexpr auto IsPersistent() const noexcept -> bool { return m_flags & Flags::Persistent; }
        constexpr auto ReceivesCollisionEvents() const noexcept -> bool { return !(m_flags & Flags::NoCollisionNotifications); }
        constexpr auto IsSerializable() const noexcept -> bool { return !(m_flags & Flags::NoSerialize); }
        constexpr auto IsInternal() const noexcept -> bool { return m_flags & Flags::Internal; }
        explicit constexpr operator index_type() const noexcept { return m_index; }
        friend bool constexpr operator==(const Entity& a, const Entity& b) { return a.Index() == b.Index(); }
        friend bool constexpr operator!=(const Entity& a, const Entity& b) { return !(a == b); }

        struct Hash
        {
            constexpr auto operator()(const Entity& entity) const noexcept
            {
                return static_cast<uint64_t>(entity.Index()) << 16
                     | static_cast<uint64_t>(entity.Layer()) << 8
                     | static_cast<uint64_t>(entity.Flags());
            }
        };

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

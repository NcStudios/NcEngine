/**
 * @file Entity.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncmath/Vector.h"
#include "ncmath/Quaternion.h"

#include <cstdint>
#include <limits>
#include <string>

namespace nc
{
/** @brief Identifies an object in the registry. */
class Entity
{
    public:
        using index_type = uint32_t;
        using layer_type = uint8_t;
        using flags_type = uint8_t;
        using user_data_type = uint8_t;

        /** @brief Index used for an invalid Entity. */
        static constexpr index_type NullIndex = std::numeric_limits<index_type>::max();

        /** @brief Flags for configuring internal Entity behavior. */
        struct Flags
        {
            static constexpr Entity::flags_type None                     = 0b00000000; ///< Default behavior
            static constexpr Entity::flags_type Static                   = 0b00000001; ///< The Entitiy's Transform will not be moved after construction
            static constexpr Entity::flags_type Persistent               = 0b00000010; ///< Entity persists across scene load/unload
            static constexpr Entity::flags_type NoCollisionNotifications = 0b00000100; ///< Do not send Collision/Trigger events involving the Entity
            static constexpr Entity::flags_type NoSerialize              = 0b00001000; ///< Exclude the Entity and its children from scene serialization
            static constexpr Entity::flags_type Internal                 = 0b00010000; ///< Entity was created by the engine/editor
        };

        /** @brief Construct a null Entity. */
        constexpr Entity() noexcept
            : m_index{NullIndex},
              m_layer{0},
              m_flags{Flags::None},
              m_userData{0}
        {
        }

        /** @brief Construct an Entity. */
        explicit constexpr Entity(index_type index,
                                  layer_type layer = 0,
                                  flags_type flags = Flags::None,
                                  user_data_type userData = 0) noexcept
            : m_index{index},
              m_layer{layer},
              m_flags{flags},
              m_userData{userData}
        {
        }

        /** @brief Construct a null Entity. */
        static constexpr auto Null() noexcept { return Entity{}; }

        /** @brief Reconstruct an Entity from a hashed value. */
        static constexpr auto FromHash(uint64_t hash) noexcept -> Entity
        {
            const auto index = static_cast<Entity::index_type>((hash >> 24) & 0xFFFFFFFF);
            const auto layer = static_cast<Entity::layer_type>((hash >> 16) & 0xFF);
            const auto flags = static_cast<Entity::flags_type>((hash >> 8) & 0xFF);
            const auto userData = static_cast<Entity::user_data_type>(hash & 0xFF);
            return Entity{index, layer, flags, userData};
        }

        /** @name Entity Properties */
        constexpr auto Valid()                   const noexcept -> bool           { return m_index != NullIndex; }
        constexpr auto Index()                   const noexcept -> index_type     { return m_index; }
        constexpr auto Layer()                   const noexcept -> layer_type     { return m_layer; }
        constexpr auto Flags()                   const noexcept -> flags_type     { return m_flags; }
        constexpr auto UserData()                const noexcept -> user_data_type { return m_userData; }
        constexpr auto IsStatic()                const noexcept -> bool           { return m_flags & Flags::Static; }
        constexpr auto IsPersistent()            const noexcept -> bool           { return m_flags & Flags::Persistent; }
        constexpr auto ReceivesCollisionEvents() const noexcept -> bool           { return !(m_flags & Flags::NoCollisionNotifications); }
        constexpr auto IsSerializable()          const noexcept -> bool           { return !(m_flags & Flags::NoSerialize); }
        constexpr auto IsInternal()              const noexcept -> bool           { return m_flags & Flags::Internal; }

        /** @name Operators */
        explicit constexpr operator index_type() const noexcept { return m_index; }
        friend bool constexpr operator==(const Entity& a, const Entity& b) { return a.Index() == b.Index(); }
        friend bool constexpr operator!=(const Entity& a, const Entity& b) { return !(a == b); }

        /** @brief Entity Hasher */
        struct Hash
        {
            constexpr auto operator()(const Entity& entity) const noexcept
            {
                return static_cast<uint64_t>(entity.Index()) << 24
                     | static_cast<uint64_t>(entity.Layer()) << 16
                     | static_cast<uint64_t>(entity.Flags()) << 8
                     | static_cast<uint64_t>(entity.UserData());
            }
        };

    private:
        index_type m_index;
        layer_type m_layer;
        flags_type m_flags;
        user_data_type m_userData;
};

/** @brief Initialization data for Entities. */
struct EntityInfo
{
    Vector3 position = Vector3::Zero();              ///< initial Transform position
    Quaternion rotation = Quaternion::Identity();    ///< initial Transform rotation (must be normalized)
    Vector3 scale = Vector3::One();                  ///< initial Transform scale (must be non-zero in all dimensions)
    Entity parent = Entity::Null();                  ///< optional Entity to parent under
    std::string tag = "Entity";                      ///< initial Tag value
    Entity::layer_type layer = 1u;                   ///< the layer the Entity belongs to
    Entity::flags_type flags = Entity::Flags::None;  ///< enabled flags
    Entity::user_data_type userData = 0u;            ///< custom data for client-side use
};
} // namespace nc

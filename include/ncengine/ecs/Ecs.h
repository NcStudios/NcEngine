/**
 * @file Ecs.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

#include "ncengine/ecs/AccessPolicy.h"

namespace nc::ecs
{
/**
 * @brief Interface for higher-level entity and component operations with optional type access restriction.
 * @tparam Base A FilterBase describing the baseline set of types accessible through the interface.
 * @tparam Includes A list of types accessible through the interface in addition to the Base.
 */
template<FilterBase Base, class... Includes>
class EcsInterface
{
    public:
        using PolicyType = AccessPolicy<Base, Includes...>;
        using FilterType = typename PolicyType::FilterType;

        /** @brief Construct an EcsInterface instance. */
        explicit EcsInterface(AccessPolicy<Base, Includes...> policy) noexcept
            : m_policy{policy} {}

        /** @brief Implicit conversion operator to a more restricted interface. */
        template<FilterBase TargetBase, class... TargetIncludes>
            requires PolicyType::template ConvertibleTo<TargetBase, TargetIncludes...>
        operator EcsInterface<TargetBase, TargetIncludes...>() const noexcept
        {
            return EcsInterface<TargetBase, TargetIncludes...>(m_policy);
        }

        /** @brief Emplace an entity. */
        template<std::same_as<Entity> T>
            requires PolicyType::template HasAccess<Entity, Transform, Tag>
        auto Emplace(EntityInfo info = {}) -> Entity
        {
            const auto handle = m_policy.template OnPool<Entity>([&info](auto&& pool) -> decltype(auto)
            {
                return pool.Add(info.layer, info.flags);
            });

            if (info.parent.Valid())
            {
                auto& parentTransform = Get<Transform>(info.parent);
                Emplace<Transform>(handle, info.position, info.rotation, info.scale, parentTransform.TransformationMatrix());
                Get<Hierarchy>(info.parent).children.push_back(handle);
            }
            else
            {
                Emplace<Transform>(handle, info.position, info.rotation, info.scale);
            }

            Emplace<Hierarchy>(handle, info.parent);
            Emplace<detail::FreeComponentGroup>(handle);
            Emplace<Tag>(handle, std::move(info.tag));
            return handle;
        }

        /** @brief Emplace a component. */
        template<Component T, class... Args>
            requires PolicyType::template HasAccess<T>
        auto Emplace(Entity entity, Args&&... args) -> T&
        {
            NC_ASSERT(Contains<Entity>(entity), "Bad entity");
            if constexpr (std::derived_from<T, FreeComponent>)
            {
                return Get<detail::FreeComponentGroup>(entity).template Add<T>(entity, std::forward<Args>(args)...);
            }
            else
            {
                return m_policy.template OnPool<T>([entity, ...args = std::forward<Args>(args)] (auto&& pool) mutable -> T&
                {
                    return pool.Emplace(entity, std::forward<decltype(args)>(args)...);
                });
            }
        }

        /** @brief Child an Entity to another, or pass Entity::Null() to detach from an existing parent. */
        void SetParent(Entity entity, Entity parent)
            requires PolicyType::template HasAccess<Hierarchy>
        {
            auto& hierarchy = Get<Hierarchy>(entity);
            auto oldParent = std::exchange(hierarchy.parent, parent);
            if (oldParent.Valid())
            {
                std::erase(Get<Hierarchy>(oldParent).children, entity);
            }

            if (parent.Valid())
            {
                Get<Hierarchy>(parent).children.push_back(entity);
            }
        }

        /** @brief Get the root Entity in a hierarchy. */
        auto GetRoot(Entity entity) -> Entity
            requires PolicyType::template HasAccess<Hierarchy>
        {
            const auto& hierarchy = Get<Hierarchy>(entity);
            return !hierarchy.parent.Valid() ? entity : GetRoot(hierarchy.parent);
        }

        /** @brief Remove an entity. */
        template<std::same_as<Entity> T>
            requires PolicyType::template HasAccess<Entity, Transform>
        auto Remove(Entity entity) -> bool
        {
            return Contains<Entity>(entity) ? RemoveNode<true>(entity) : false;
        }

        /** @brief Remove a component. */
        template<Component T>
            requires PolicyType::template HasAccess<T>
        auto Remove(Entity entity) -> bool
        {
            if (!Contains<Entity>(entity))
                return false;

            if constexpr (std::derived_from<T, FreeComponent>)
            {
                return m_policy.template OnPool<detail::FreeComponentGroup>([entity](auto& pool)
                {
                    return pool.Get(entity).template Remove<T>();
                });
            }
            else
            {
                return m_policy.template OnPool<T>([entity](auto& pool)
                {
                    return pool.Remove(entity);
                });
            }
        }

        /** @brief Get a component. */
        template<Component T>
            requires PolicyType::template HasAccess<T>
        auto Get(Entity entity) -> T&
        {
            if constexpr (std::derived_from<T, FreeComponent>)
            {
                auto& bag = Get<detail::FreeComponentGroup>(entity);
                return bag.template Get<T>();
            }
            else
            {
                return m_policy.template OnPool<T>([entity](auto&& pool) -> T&
                {
                    return pool.Get(entity);
                });
            }
        }

        /** @brief Get a component. */
        template<Component T>
            requires PolicyType::template HasAccess<T>
        auto Get(Entity entity) const -> const T&
        {
            if constexpr (std::derived_from<T, FreeComponent>)
            {
                auto& bag = Get<detail::FreeComponentGroup>(entity);
                return bag.template Get<T>();
            }
            else
            {
                return m_policy.template OnPool<T>([entity](auto&& pool) -> const T&
                {
                    return pool.Get(entity);
                });
            }
        }

        /** @brief Get a contiguous view of all instances of a type. */
        template<class T>
            requires PolicyType::template HasAccess<T>
                 && (PooledComponent<T> || std::same_as<Entity, T>)
        auto GetAll() -> std::span<T>
        {
            auto& pool = m_policy.template OnPool<T>([](auto&& p) -> decltype(auto)
            {
                return std::forward<decltype(p)>(p);
            });

            return std::span<T>{pool.begin(), pool.end()};
        }

        /** @brief Get a contiguous view of all instances of a type. */
        template<class T>
            requires PolicyType::template HasAccess<T>
                 && (PooledComponent<T> || std::same_as<Entity, T>)
        auto GetAll() const -> std::span<const T>
        {
            const auto& pool = m_policy.template OnPool<T>([](auto&& p) -> decltype(auto)
            {
                return std::forward<decltype(p)>(p);
            });

            return std::span<const T>{pool.begin(), pool.end()};
        }

        /** @brief Get a range of pointers to all ComponentPoolBase instances. */
        auto GetComponentPools()
            requires PolicyType::template BaseContains<FilterBase::All>
        {
            return m_policy.GetComponentPools();
        }

        /** @brief Get the parent entity a component is attached to or Entity::Null(). */
        template<PooledComponent T>
            requires PolicyType::template HasAccess<T>
        auto GetParent(const T* component) const -> Entity
        {
            return m_policy.template OnPool<T>([component](auto&& pool)
            {
                return pool.GetParent(component);
            });
        }

        /** @brief Check if an entity or component exists. */
        template<RegistryType T>
            requires PolicyType::template HasAccess<T>
        auto Contains(Entity entity) const -> bool
        {
            if constexpr (std::derived_from<T, FreeComponent>)
            {
                if (!Contains<detail::FreeComponentGroup>(entity))
                    return false;

                const auto& bag = Get<detail::FreeComponentGroup>(entity);
                return bag.template Contains<T>();
            }
            else
            {
                return m_policy.template OnPool<T>([entity](auto&& pool)
                {
                    return pool.Contains(entity);
                });
            }
        }

    private:
        PolicyType m_policy;

        template<bool IsRoot>
        auto RemoveNode(Entity entity) -> bool
        {
            auto& hierarchy = Get<Hierarchy>(entity);
            if constexpr(IsRoot)
            {
                if (hierarchy.parent.Valid())
                {
                    std::erase(Get<Hierarchy>(hierarchy.parent).children, entity);
                }
            }

            for (auto child : hierarchy.children)
                RemoveNode<false>(child);

            m_policy.template OnPool<Entity>([entity](auto&& p) { p.Remove(entity); });
            return true;
        }
};

/** @brief Helper alias for an EcsInterface with access to all types. */
using Ecs = EcsInterface<FilterBase::All>;

/** @brief Helper alias for an EcsInterface with access to common ecs types in addition to any explicitly approved types. */
template<class... Includes>
using BasicEcs = EcsInterface<FilterBase::Basic, Includes...>;

/** @brief Helper alias for an EcsInterface with access to only explicitly approved types and those derived from FreeComponent. */
template<class... Includes>
using ExplicitEcs = EcsInterface<FilterBase::None, Includes...>;
} // namespace nc::ecs

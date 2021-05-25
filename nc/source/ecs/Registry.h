#pragma once

#include "entity/EntityHandle.h"

#include <algorithm>
#include <functional>
#include <span>
#include <stdexcept>
#include <vector>


#include <iostream>

namespace nc::ecs
{
    template<class T>
    class UnorderedPool
    {
        public:
            UnorderedPool() : m_data{} {}

            template<class... Args>
            auto Emplace(Args&&... args) -> T*
            {
                return &m_data.emplace_back(std::forward<Args>(args)...);
            }

            void Remove(HandleTraits::index_type i)
            {
                m_data.at(i) = std::move(m_data.back());
                m_data.pop_back();
                //return m_data[i].GetParentHandle();
            }

            auto Get(HandleTraits::index_type i) -> T*
            {
                return &m_data.at(i);
            }

            auto View() -> std::span<T>
            {
                return std::span{m_data};
            }

            void Clear()
            {
                m_data.clear();
            }

            auto Size() const -> size_t
            {
                return m_data.size();
            }

        private:
            std::vector<T> m_data;
    };

    /** Callbacks allowing external systems to hook into addition and deletion events. */
    template<class T>
    struct SystemCallbacks
    {
        using on_add_type = std::function<void(const T&)>;
        using on_remove_type = std::function<void(EntityHandle)>;
        on_add_type OnAdd = nullptr;
        on_remove_type OnRemove = nullptr;
    };


    template<class ValueType, class BindType>
    struct BoundType
    {
        ValueType value = ValueType{};
    };

    template<class BindType>
    struct BoundType<HandleTraits::index_type, BindType>
    {
        HandleTraits::index_type value = std::numeric_limits<HandleTraits::index_type>::max();
    };

    /** Constructs a domain such that for each type T in Ts..., there exists exactly
     *  one value of type ValueType associated with type T. */
    template<typename ValueType, class... Ts>
    struct UniquelyQuantifiedDomain
    {
        template<class T>
        struct BoundValue { ValueType value = ValueType{}; };

        std::tuple<BoundValue<Ts>...> domain;
    };

    // template<class... Ts>
    // struct UniquelyQuantifiedDomain<HandleTraits::index_type, Ts...>
    // {
    //     template<class T>
    //     struct BoundValue { HandleTraits::index_type value = std::numeric_limits<HandleTraits::index_type>::max(); };

    //     std::tuple<BoundValue<Ts>...> domain;
    // };

    /** A cluster of indices each associated with a unique type used to index pools of their 
     *  corresponding types. */
    // template<std::integral I, I DefaultValue, class... Ts>
    // struct TypedIndexPack
    // {
    //     template<class T>
    //     struct TypedIndex { I value = DefaultValue; };

    //     std::tuple<TypedIndex<Ts>...> indices;
    // };

    template<class... Ts>
    class Registry
    {
        using index_type = HandleTraits::index_type;
        static constexpr index_type NullIndex = std::numeric_limits<index_type>::max();
        //using index_pack_type = UniquelyQuantifiedDomain<index_type, Ts...>;
        using index_pack_type = std::tuple<BoundType<index_type, Ts>...>;

        template<class T>
        using storage_type = UnorderedPool<T>;
        using pools_type = std::tuple<storage_type<Ts>...>;
        static constexpr size_t PoolCount = sizeof...(Ts);
        
        using entity_storage_type = storage_type<index_type>;
        using entity_pools_type = UniquelyQuantifiedDomain<entity_storage_type, Ts...>;

        using callbacks_type = std::tuple<SystemCallbacks<Ts>...>;


        public:
            Registry(size_t maxEntities);

            template<class T, class... Args>
            auto AddComponent(EntityHandle handle, Args&&... args) -> T*;

            template<class T>
            bool RemoveComponent(EntityHandle handle);

            template<class T>
            bool HasComponent(EntityHandle handle);

            template<class T>
            auto GetComponent(EntityHandle handle) -> T*;

            template<class T>
            auto ViewAll() -> std::span<T>;

            void Clear();

            template<class T>
            void RegisterOnAddCallback(SystemCallbacks<T>::on_add_type func);

            template<class T>
            void RegisterOnRemoveCallback(SystemCallbacks<T>::on_remove_type func);

        private:
            std::vector<index_pack_type> m_sparse;
            pools_type m_pools;
            entity_pools_type m_entityPools;
            callbacks_type m_callbacks;

            template<class T>
            auto GetPoolIndex(HandleTraits::index_type sparseIndex) -> HandleTraits::index_type;

            template<class T>
            void SetPoolIndex(HandleTraits::index_type sparseIndex, HandleTraits::index_type value);

            //template<class T>
            //auto GetEntityPool() -> dense_entity_type& { return std::get<typename entity_pools_type::BoundValue<T>>(m_entityPools.domain).value; }

            template<class T>
            auto GetEntityPool() -> entity_storage_type& { return std::get<typename entity_pools_type::BoundValue<T>>(m_entityPools.domain).value; }

            template<class T>
            auto GetComponentPool() -> storage_type<T>& { return std::get<storage_type<T>>(m_pools); }

            template<class T>
            auto GetCallbacks() -> SystemCallbacks<T>& { return std::get<SystemCallbacks<T>>(m_callbacks); }
    };

    template<class... Ts>
    Registry<Ts...>::Registry(size_t maxEntities)
        : m_sparse{maxEntities},
          m_pools{storage_type<Ts>{}...},
          m_entityPools{}
    {}

    template<class... Ts>
    template<class T, class... Args>
    T* Registry<Ts...>::AddComponent(EntityHandle handle, Args&&... args)
    {
        if(HasComponent<T>(handle))
            throw std::runtime_error("Registry::Emplace - Cannot add multiple components of the same type to a single entity");
        
        auto& pool = GetComponentPool<T>();
        auto* ptr = pool.Emplace(handle, std::forward<Args>(args)...);
        auto index = pool.Size() - 1;

        SetPoolIndex<T>(handle.Index(), index);

        auto& entityPool = GetEntityPool<T>();
        
        //entityPool.push_back(handle.Index());
        entityPool.Emplace(handle.Index());

        if(auto& func = GetCallbacks<T>().OnAdd; func)
            func(*ptr);
        
        return ptr;
    }

    template<class... Ts>
    template<class T>
    auto Registry<Ts...>::GetPoolIndex(HandleTraits::index_type sparseIndex) -> HandleTraits::index_type
    {
        auto& typeSequence = m_sparse.at(sparseIndex);
        return std::get<BoundType<index_type, T>>(typeSequence).value;
        //return std::get<typename index_pack_type::BoundValue<T>>(typeSequence.domain).value;
    }

    template<class... Ts>
    template<class T>
    void Registry<Ts...>::SetPoolIndex(HandleTraits::index_type sparseIndex, HandleTraits::index_type value)
    {
        auto& typeSequence = m_sparse.at(sparseIndex);
        
        std::get<BoundType<index_type, T>>(typeSequence).value = value;
        //std::get<typename index_pack_type::BoundValue<T>>(typeSequence.domain).value = value;
    }

    template<class... Ts>
    template<class T>
    bool Registry<Ts...>::RemoveComponent(EntityHandle handle)
    {
        auto sparseIndex = handle.Index();
        auto poolIndex = GetPoolIndex<T>(sparseIndex);
        if(poolIndex == NullIndex)
            return false;
        
        GetComponentPool<T>().Remove(poolIndex);

        SetPoolIndex<T>(sparseIndex, NullIndex);

        auto& entityPool = GetEntityPool<T>();

        // entityPool.at(poolIndex) = entityPool.back();
        // auto movedHandle2 = entityPool.at(poolIndex);
        // entityPool.pop_back();

        // if(handle.Index() != movedHandle2)
        //     SetPoolIndex<T>(movedHandle2, poolIndex);

        //auto movedHandle = entityPool.View().back();
        auto movedHandle = *entityPool.Get(entityPool.Size() - 1);
        entityPool.Remove(poolIndex);
        if(handle.Index() != movedHandle)
            SetPoolIndex<T>(movedHandle, poolIndex);

        if(auto& func = GetCallbacks<T>().OnRemove; func)
            func(handle);

        return true;
    }

    template<class... Ts>
    template<class T>
    bool Registry<Ts...>::HasComponent(EntityHandle handle)
    {
        auto poolIndex = GetPoolIndex<T>(handle.Index());
        return poolIndex == NullIndex ? false : true;
    }

    template<class... Ts>
    template<class T>
    T* Registry<Ts...>::GetComponent(EntityHandle handle)
    {
        auto poolIndex = GetPoolIndex<T>(handle.Index());
        return poolIndex == NullIndex ? nullptr : GetComponentPool<T>().Get(poolIndex);
    }

    template<class... Ts>
    template<class T>
    std::span<T> Registry<Ts...>::ViewAll()
    {
        return GetComponentPool<T>().View();
    }

    template<class... Ts>
    void Registry<Ts...>::Clear()
    {
        std::apply([](auto&&... pools) { (pools.Clear(), ...); }, m_pools);
        std::apply([](auto&&... boundPool) { (boundPool.value.Clear(), ...); }, m_entityPools.domain);
        std::ranges::fill(m_sparse, index_pack_type{}); // reset all to NullIndex
    }

    template<class... Ts>
    template<class T>
    void Registry<Ts...>::RegisterOnAddCallback(typename SystemCallbacks<T>::on_add_type func)
    {
        GetCallbacks<T>().OnAdd = std::move(func);
    }

    template<class... Ts>
    template<class T>
    void Registry<Ts...>::RegisterOnRemoveCallback(typename SystemCallbacks<T>::on_remove_type func)
    {
        GetCallbacks<T>().OnRemove = std::move(func);
    }
}
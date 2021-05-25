#pragma once

#include "entity/EntityHandle.h"

#include <algorithm>
#include <functional>
#include <span>
#include <stdexcept>
#include <vector>

namespace nc::ecs
{
    template<class T>
    class UnorderedPool
    {
        public:
            UnorderedPool() : m_data{} {}

            size_t Size() const { return m_data.size(); }

            template<class... Args>
            T* Emplace(Args&&... args)
            {
                return &m_data.emplace_back(std::forward<Args>(args)...);
            }

            EntityHandle Remove(HandleTraits::index_type i)
            {
                m_data.at(i) = std::move(m_data.back());
                m_data.pop_back();
                return m_data[i].GetParentHandle();
            }

            T* Get(HandleTraits::index_type i) { return &m_data.at(i); }
            std::span<T> View() { return std::span{m_data}; }
            void Clear() { m_data.clear(); }

        private:
            std::vector<T> m_data;
    };

    template<class... Ts>
    class Registry
    {
        template<class T>
        using storage_type = UnorderedPool<T>;
        using index_type = HandleTraits::index_type;
        using pools_type = std::tuple<storage_type<Ts>...>;

        static constexpr index_type NullIndex = std::numeric_limits<index_type>::max();
        static constexpr size_t PoolCount = sizeof...(Ts);

        struct TypedSequence
        {
            template<class T>
            struct TypedIndex
            {
                index_type value = std::numeric_limits<index_type>::max();
            };

            std::tuple<TypedIndex<Ts>...> indices;
        };

        template<class T>
        struct Callbacks
        {
            using on_add_type = std::function<void(const T&)>;
            using on_remove_type = std::function<void(EntityHandle)>;
            on_add_type OnAdd = nullptr;
            on_remove_type OnRemove = nullptr;
        };

        public:
            Registry(size_t maxEntities);

            template<class T, class... Args>
            T* AddComponent(EntityHandle handle, Args&&... args);

            template<class T>
            bool RemoveComponent(EntityHandle handle);

            template<class T>
            bool HasComponent(EntityHandle handle);

            template<class T>
            T* GetComponent(EntityHandle handle);

            template<class T>
            std::span<T> ViewAll();

            void Clear();

            template<class T>
            void RegisterOnAddCallback(Callbacks<T>::on_add_type func)
            {
                std::get<Callbacks<T>>(m_callbacks).OnAdd = std::move(func);
            }

            template<class T>
            void RegisterOnRemoveCallback(Callbacks<T>::on_remove_type func)
            {
                std::get<Callbacks<T>>(m_callbacks).OnRemove = std::move(func);
            }

        private:
            std::vector<TypedSequence> m_sparse;
            pools_type m_pools;
            std::tuple<Callbacks<Ts>...> m_callbacks;

            template<class T>
            auto GetDenseIndex(HandleTraits::index_type sparseIndex) -> HandleTraits::index_type;

            template<class T>
            void SetDenseIndex(HandleTraits::index_type sparseIndex, HandleTraits::index_type value);

    };

    template<class... Ts>
    Registry<Ts...>::Registry(size_t maxEntities)
        : m_sparse{maxEntities},
          m_pools{storage_type<Ts>{}...}
    {}

    template<class... Ts>
    template<class T, class... Args>
    T* Registry<Ts...>::AddComponent(EntityHandle handle, Args&&... args)
    {
        if(HasComponent<T>(handle))
            throw std::runtime_error("Registry::Emplace - Cannot add multiple components of the same type");
        
        auto& pool = std::get<storage_type<T>>(m_pools);
        auto* ptr = pool.Emplace(handle, std::forward<Args>(args)...);
        auto index = pool.Size() - 1;

        SetDenseIndex<T>(handle.Index(), index);
        
        auto& cb = std::get<Callbacks<T>>(m_callbacks).OnAdd;
        if(cb)
            cb(*ptr);
        
        return ptr;
    }

    template<class... Ts>
    template<class T>
    auto Registry<Ts...>::GetDenseIndex(HandleTraits::index_type sparseIndex) -> HandleTraits::index_type
    {
        auto& typeSequence = m_sparse.at(sparseIndex);
        return std::get<typename TypedSequence::TypedIndex<T>>(typeSequence.indices).value;
    }

    template<class... Ts>
    template<class T>
    void Registry<Ts...>::SetDenseIndex(HandleTraits::index_type sparseIndex, HandleTraits::index_type value)
    {
        auto& typeSequence = m_sparse.at(sparseIndex);
        std::get<typename TypedSequence::TypedIndex<T>>(typeSequence.indices).value = value;
    }

    template<class... Ts>
    template<class T>
    bool Registry<Ts...>::RemoveComponent(EntityHandle handle)
    {
        auto sparseIndex = handle.Index();
        auto denseIndex = GetDenseIndex<T>(sparseIndex);
        if(denseIndex == NullIndex)
            return false;
        
        auto& pool = std::get<storage_type<T>>(m_pools);
        auto movedHandle = pool.Remove(denseIndex);

        if(movedHandle != handle)
        {
            auto movedSparseIndex = movedHandle.Index();
            SetDenseIndex<T>(movedSparseIndex, denseIndex);
        }

        SetDenseIndex<T>(sparseIndex, NullIndex);

        auto& cb = std::get<Callbacks<T>>(m_callbacks).OnRemove;
        if(cb)
            cb(handle);

        return true;
    }

    template<class... Ts>
    template<class T>
    bool Registry<Ts...>::HasComponent(EntityHandle handle)
    {
        auto sparseIndex = handle.Index();
        auto denseIndex = GetDenseIndex<T>(sparseIndex);
        return denseIndex == NullIndex ? false : true;
    }

    template<class... Ts>
    template<class T>
    T* Registry<Ts...>::GetComponent(EntityHandle handle)
    {
        auto denseIndex = GetDenseIndex<T>(handle.Index());
        return denseIndex == NullIndex ? nullptr : std::get<storage_type<T>>(m_pools).Get(denseIndex);
    }

    template<class... Ts>
    template<class T>
    std::span<T> Registry<Ts...>::ViewAll()
    {
        auto& pool = std::get<storage_type<T>>(m_pools);
        return pool.View();
    }

    template<class... Ts>
    void Registry<Ts...>::Clear()
    {
        std::apply([](auto&&... pools)
        {
            (pools.Clear(), ...);
        }, m_pools);

        std::ranges::fill(m_sparse, TypedSequence{});
    }
}
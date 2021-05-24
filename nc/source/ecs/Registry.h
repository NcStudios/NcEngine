#pragma once

#include "component/Component.h"
#include "entity/EntityHandle.h"

#include <algorithm>
#include <concepts>
#include <span>
#include <vector>

#include "component/ComponentView.h"
//#include "Ecs.h"

namespace nc::ecs
{
    // doesn't have to be component?
    // should be moveable?

    template<class T>
    class UnorderedPool
    {
        public:
            UnorderedPool()
                : m_data{}
            {}

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


    template<std::derived_from<ComponentBase> T>
    class Registry
    {
        using index_type = HandleTraits::index_type;
        static constexpr index_type NullIndex = std::numeric_limits<index_type>::max();

        public:
            Registry(size_t maxCount);

            template<class... Args>
            T* Emplace(EntityHandle handle, Args&&... args);

            bool Remove(EntityHandle handle);
            bool Contains(EntityHandle handle) const;
            T* Get(EntityHandle handle);
            void Clear();
            std::span<T> View();

        private:
            std::vector<index_type> m_sparse;
            UnorderedPool<T> m_dense;
            //std::vector<T> m_dense;
    };

    template<std::derived_from<ComponentBase> T>
    Registry<T>::Registry(size_t maxCount)
        : m_sparse(maxCount, NullIndex),
          m_dense{}
    {}

    template<std::derived_from<ComponentBase> T>
    template<class... Args>
    T* Registry<T>::Emplace(EntityHandle handle, Args&&... args)
    {
        if(Contains(handle))
            throw std::runtime_error("Registry::Emplace - Registry already has a component for this entity");

        auto* ptr = m_dense.Emplace(handle, std::forward<Args>(args)...);
        auto index = m_dense.Size() - 1;

        m_sparse.at(handle.Index()) = index;
        return ptr;
    }

    template<std::derived_from<ComponentBase> T>
    T* Registry<T>::Get(EntityHandle handle)
    {
        auto denseIndex = m_sparse.at(handle.Index());
        return denseIndex == NullIndex ? nullptr : m_dense.Get(denseIndex);
    }

    template<std::derived_from<ComponentBase> T>
    bool Registry<T>::Contains(EntityHandle handle) const
    {
        return m_sparse.at(handle.Index()) != NullIndex;
    }

    template<std::derived_from<ComponentBase> T>
    bool Registry<T>::Remove(EntityHandle handle)
    {
        const auto sparseIndex = handle.Index();
        const auto denseIndex = m_sparse.at(sparseIndex);
        if(denseIndex == NullIndex)
            return false;

        m_sparse.at(sparseIndex) = NullIndex;
        
        auto movedHandle = m_dense.Remove(denseIndex);
        
        if(movedHandle != handle)
            m_sparse.at(movedHandle.Index()) = denseIndex;

        return true;
    }

    template<std::derived_from<ComponentBase> T>
    void Registry<T>::Clear()
    {
        m_dense.Clear();
        //m_dense.clear();
        //m_dense.shrink_to_fit();
        std::ranges::fill(m_sparse, NullIndex);
    }

    template<std::derived_from<ComponentBase> T>
    std::span<T> Registry<T>::View()
    {
        return m_dense.View();
        //return std::span<T>(m_dense);
    }
} // namespace nc::ecs
#pragma once

#include "component/Component.h"
#include "entity/EntityHandle.h"

#include <algorithm>
#include <concepts>
#include <span>
#include <vector>

namespace nc::ecs
{
    // doesn't have to be component?
    // should be moveable?

    template<std::derived_from<Component> T>
    class Registry
    {
        using index_type = EntityHandle::Handle_t;
        static constexpr index_type NullIndex = static_cast<index_type>(EntityHandle::Invalid());
        static constexpr index_type ToIndex(EntityHandle h) { return static_cast<index_type>(h); }

        public:
            Registry(index_type maxCount);

            template<class... Args>
            T* Emplace(EntityHandle handle, Args&&... args);

            bool Remove(EntityHandle handle);
            bool Contains(EntityHandle handle);
            T* Get(EntityHandle handle);
            void Clear();
            std::span<T> View();

        private:
            std::vector<index_type> m_sparse;
            std::vector<T> m_dense;
    };

    template<std::derived_from<Component> T>
    Registry<T>::Registry(index_type maxCount)
        : m_sparse(maxCount, NullIndex),
          m_dense{}
    {}

    template<std::derived_from<Component> T>
    template<class... Args>
    T* Registry<T>::Emplace(EntityHandle handle, Args&&... args)
    {
        // what if already has?
        if(Contains(handle))
            throw std::runtime_error("Registry::Emplace - Registry already has a component for this entity");

        auto& ref = m_dense.emplace_back(handle, std::forward<Args>(args)...);
        auto index = m_dense.size() - 1;
        m_sparse.at(ToIndex(handle)) = index;
        return &ref;
    }

    template<std::derived_from<Component> T>
    T* Registry<T>::Get(EntityHandle handle)
    {
        auto denseIndex = m_sparse.at(ToIndex(handle));
        return denseIndex == NullIndex ? nullptr : &(m_dense.at(denseIndex));
    }

    template<std::derived_from<Component> T>
    bool Registry<T>::Contains(EntityHandle handle)
    {
        return m_sparse.at(ToIndex(handle)) != NullIndex;
    }

    template<std::derived_from<Component> T>
    bool Registry<T>::Remove(EntityHandle handle)
    {
        const auto sparseIndex = ToIndex(handle);
        const auto denseIndex = m_sparse.at(sparseIndex);
        if(denseIndex == NullIndex)
            return false;

        m_sparse.at(sparseIndex) = NullIndex;
        m_dense.at(denseIndex) = std::move(m_dense.back());
        m_dense.pop_back();

        auto movedHandle = m_dense[denseIndex].GetParentHandle();
        if(movedHandle != handle)
            m_sparse.at(ToIndex(movedHandle)) = denseIndex;

        return true;
    }

    template<std::derived_from<Component> T>
    void Registry<T>::Clear()
    {
        m_dense.clear();
        m_dense.shrink_to_fit();
        std::ranges::fill(m_sparse, NullIndex);
    }

    template<std::derived_from<Component> T>
    std::span<T> Registry<T>::View()
    {
        return std::span<T>(m_dense);
    }
} // namespace nc::ecs
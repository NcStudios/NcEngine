#pragma once

#include "alloc/PoolAdapter.h"
#include "entity/EntityHandle.h"

namespace nc::ecs
{
    template<class T>
    class ComponentSystem
    {
        public:
            ComponentSystem(uint32_t count);
            virtual ~ComponentSystem() = default;

            template<class ... Args> T* Add(EntityHandle parentHandle, Args&& ... args);
            bool Remove(EntityHandle parentHandle);
            bool Contains(EntityHandle parentHandle) const;
            T* GetPointerTo(EntityHandle parentHandle);
            std::span<T*> GetComponents();
            void Clear();

        private:
            alloc::PoolAdapter<T> m_pool;
    };

    template<class T>
    ComponentSystem<T>::ComponentSystem(uint32_t count)
        : m_pool{count}
    {
    }

    template<class T>
    std::span<T*> ComponentSystem<T>::GetComponents()
    {
        return m_pool.GetActiveRange();
    }

    template<class T>
    void ComponentSystem<T>::Clear()
    {
        m_pool.Clear();
    }

    template<class T>
    template<class ... Args>
    T* ComponentSystem<T>::Add(EntityHandle handle, Args&& ... args)
    {
        return m_pool.Add(handle, std::forward<Args>(args)...);
    }

    template<class T>
    bool ComponentSystem<T>::Remove(EntityHandle handle)
    {
        return m_pool.RemoveIf([handle](auto* item) { return handle == item->GetParentHandle(); });
    }

    template<class T>
    bool ComponentSystem<T>::Contains(EntityHandle handle) const
    {
        return m_pool.Contains([handle](auto* item) { return handle == item->GetParentHandle(); });
    }

    template<class T>
    T* ComponentSystem<T>::GetPointerTo(EntityHandle handle)
    {
        return m_pool.Get([handle](auto* item) { return handle == item->GetParentHandle(); });
    }
} // namespace nc::ecs
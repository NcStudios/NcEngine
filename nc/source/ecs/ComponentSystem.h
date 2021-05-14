#pragma once

#include "entity/EntityHandle.h"
#include "alloc/Utility.h"
#include "debug/Utils.h"
#include "debug/Profiler.h"

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

namespace nc::ecs
{
    template<class T>
    class ComponentSystem
    {
        using Allocator = alloc::PoolAllocator<T>;

        public:
            using ContainerType = std::vector<std::unique_ptr<T, alloc::basic_deleter<Allocator>>>;

            ComponentSystem(uint32_t count);
            virtual ~ComponentSystem() noexcept;

            template<class ... Args> T* Add(EntityHandle parentHandle, Args&& ... args);
            bool Remove(EntityHandle parentHandle);
            bool Contains(EntityHandle parentHandle) const;
            T* GetPointerTo(EntityHandle parentHandle);
            ContainerType& GetComponents();
            void Clear();

        private:
            ContainerType m_components;
    };

    template<class T>
    ComponentSystem<T>::ComponentSystem(uint32_t count)
        : m_components{}
    {
        Allocator::create_memory_resource(count * sizeof(T));
    }

    template<class T>
    ComponentSystem<T>::~ComponentSystem() noexcept
    {
        Allocator::destroy_memory_resource();
    }

    template<class T>
    ComponentSystem<T>::ContainerType& ComponentSystem<T>::GetComponents()
    {
        return m_components;
    }

    template<class T>
    void ComponentSystem<T>::Clear()
    {
        m_components.clear();
        m_components.shrink_to_fit();
        Allocator().clear_memory_resource();
    }

    template<class T>
    template<class ... Args>
    T* ComponentSystem<T>::Add(EntityHandle handle, Args&& ... args)
    {
        if(Contains(handle))
            return nullptr;

        auto ptr = alloc::make_unique<T, Allocator>(handle, std::forward<Args>(args)...);
        auto pos = m_components.insert
        (
            std::upper_bound(m_components.begin(), m_components.end(), ptr.get(), [](T* toAdd, auto& existing) { return toAdd < existing.get(); }),
            std::move(ptr)
        );

        return pos->get();
    }

    template<class T>
    bool ComponentSystem<T>::Remove(EntityHandle handle)
    {
        if (!Contains(handle))
            return false;

        std::erase_if(m_components, [handle](auto& c)
        {
            return handle == c->GetParentHandle();
        });

        return true;
    }

    template<class T>
    bool ComponentSystem<T>::Contains(EntityHandle handle) const
    {
        return m_components.cend() != std::ranges::find_if(m_components, [handle](auto& c)
        {
            return handle == c->GetParentHandle();
        });
    }

    template<class T>
    T* ComponentSystem<T>::GetPointerTo(EntityHandle handle)
    {
        for(auto& c : m_components)
        {
            if(handle == c->GetParentHandle())
                return c.get();
        }

        return nullptr;
    }
} // namespace nc::ecs
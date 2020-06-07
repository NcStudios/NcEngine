#pragma once

#include "Common.h"
#include "HandleManager.h"
#include "views/EntityView.h"

#include <vector>
#include <unordered_map>
#include <memory>

namespace nc::engine
{
    template<class T>
    class ComponentSystem
    {
        public:
            ComponentSystem(const unsigned int reserveSize = 50);
            virtual ~ComponentSystem() = default;
            
            template<class ... Args>
            ComponentHandle Add(const EntityView parentView, Args&& ... args);
            virtual bool Remove(const ComponentHandle handle);
            virtual bool Contains(const ComponentHandle handle) const;
            virtual std::vector<T>& GetVector();
            T* GetPointerTo(const ComponentHandle handle);

            ComponentHandle GetCurrentHandle();

        protected:
            ComponentIndex GetIndexFromHandle(const ComponentHandle handle) const;
            void MapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex);
            void RemapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex);

        private:
            std::vector<T> m_components;
            std::unordered_map<ComponentHandle, ComponentIndex> m_indexMap;
            HandleManager<ComponentHandle> m_handleManager;
    };

    template<class T>
    ComponentSystem<T>::ComponentSystem(const unsigned int reserveSize)
    {
        m_components.reserve(reserveSize);
    }

    template<class T>
    template<class ... Args>
    ComponentHandle ComponentSystem<T>::Add(const EntityView parentView, Args&& ... args)
    {
        ComponentHandle handle = m_handleManager.GenerateNewHandle();
        T newComponent((args)...);
        newComponent.Initialize(handle, parentView);
        m_components.push_back(std::move(newComponent));
        ComponentIndex lastIndex = m_components.size() - 1;
        MapHandleToIndex(handle, lastIndex);
        return handle;
    }

    template<class T>
    bool ComponentSystem<T>::Remove(const ComponentHandle handle)
    {
        if (!Contains(handle)) { return false; }

        ComponentIndex toRemove = GetIndexFromHandle(handle);
        ComponentIndex lastIndex = m_components.size() - 1;
        ComponentHandle lastElementHandle = m_components.at(lastIndex).GetHandle(); //no good

        m_components.at(toRemove) = std::move(m_components.at(lastIndex));
        m_components.pop_back();
        if(m_indexMap.erase(handle) != 1) { return false; }
        RemapHandleToIndex(lastElementHandle, toRemove);
        
        return true;
    }

    template<class T>
    bool ComponentSystem<T>::Contains(const ComponentHandle handle) const
    {
        return m_indexMap.count(handle) > 0;
    }

    template<class T>
    std::vector<T>& ComponentSystem<T>::GetVector()
    {
        return m_components;
    }

    template<class T>
    T* ComponentSystem<T>::GetPointerTo(const ComponentHandle handle)
    {
        if (!Contains(handle)) return nullptr;
        ComponentIndex index = GetIndexFromHandle(handle);
        return &m_components.at(index);
    }

    template<class T>
    ComponentHandle ComponentSystem<T>::GetCurrentHandle()
    {
        return m_handleManager.GetCurrent();
    }

    template<class T>
    ComponentIndex ComponentSystem<T>::GetIndexFromHandle(const ComponentHandle handle) const
    {
        if (!Contains(handle)) return 0; //need value for unsuccessful
        return m_indexMap.at(handle);
    }

    template<class T>
    void ComponentSystem<T>::MapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex)
    {
        if (!Contains(handle)) { m_indexMap.emplace(handle, targetIndex); }
    }

    template<class T>
    void ComponentSystem<T>::RemapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex)
    {
        if (Contains(handle)) { m_indexMap.at(handle) = targetIndex; }
    }

}
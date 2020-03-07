#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include "Common.h"
#include "HandleManager.h"

namespace nc::engine
{
    template<class T>
    class ComponentManager
    {
        public:
            ComponentManager();
            virtual ~ComponentManager() = default;
            
            ComponentHandle virtual Add(const EntityHandle parentHandle);
            bool virtual Remove(const ComponentHandle handle);
            bool virtual Contains(const ComponentHandle handle) const;
            const virtual std::vector<T>& GetVector() const;
            T* GetPointerTo(const ComponentHandle handle);

        protected:
            std::vector<T> m_components;
            std::unordered_map<ComponentHandle, ComponentIndex> m_indexMap;
            HandleManager<ComponentHandle> handleManager;

            ComponentIndex GetIndexFromHandle(const ComponentHandle handle) const;
            void MapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex);
            void RemapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex);
    };

    template<class T>
    ComponentManager<T>::ComponentManager()
    {
        m_components.reserve(50);
    } 

    template<class T>
    ComponentHandle ComponentManager<T>::Add(const EntityHandle parentHandle)
    {
        ComponentHandle handle = handleManager.GenerateNewHandle();
        m_components.push_back(T(handle, parentHandle));
        ComponentIndex lastIndex = m_components.size() - 1;
        MapHandleToIndex(handle, lastIndex);
        return handle;
    }

    template<class T>
    bool ComponentManager<T>::Remove(const ComponentHandle handle)
    {
        if (!Contains(handle)) { return false; }

        ComponentIndex toRemove = GetIndexFromHandle(handle);
        ComponentIndex lastIndex = m_components.size() - 1;
        ComponentHandle lastElementHandle = m_components.at(lastIndex).GetHandle(); //no good

        m_components.at(toRemove) = m_components.at(lastIndex);
        m_components.pop_back();
        if(m_indexMap.erase(handle) != 1) { return false; }
        RemapHandleToIndex(lastElementHandle, toRemove);
        
        return true;
    }

    template<class T>
    bool ComponentManager<T>::Contains(const ComponentHandle handle) const
    {
        return m_indexMap.count(handle) > 0;
    }

    template<class T>
    const std::vector<T>& ComponentManager<T>::GetVector() const
    {
        return m_components;
    }

    template<class T>
    T* ComponentManager<T>::GetPointerTo(const ComponentHandle handle)
    {
        if (!Contains(handle)) return nullptr;
        ComponentIndex index = GetIndexFromHandle(handle);
        return &m_components.at(index);
    }

    template<class T>
    ComponentIndex ComponentManager<T>::GetIndexFromHandle(const ComponentHandle handle) const
    {
        if (!Contains(handle)) return 0; //need value for unsuccessful
        return m_indexMap.at(handle);
    }

    template<class T>
    void ComponentManager<T>::MapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex)
    {
        if (!Contains(handle)) { m_indexMap.emplace(handle, targetIndex); }
    }

    template<class T>
    void ComponentManager<T>::RemapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex)
    {
        if (Contains(handle)) { m_indexMap.at(handle) = targetIndex; }
    }

}
#pragma once

#include "NcCommonTypes.h"
#include "HandleManager.h"
#include "engine/alloc/Pool.h"
#include "DebugUtils.h"

#include <vector>
#include <unordered_map>
#include <memory>

namespace nc::engine
{
    struct ComponentIndexPair
    {
        uint32_t indexInPoolCollection;
        uint32_t indexInPool;
    };
}

namespace nc::ecs
{

template<class T>
class ComponentSystem
{
    public:
        ComponentSystem(const uint32_t reserveSize = 100u, bool isReserveSizeMaxSize = false);
        virtual ~ComponentSystem() = default;
        
        ComponentHandle GetCurrentHandle();

        template<class ... Args>
        ComponentHandle Add(const EntityHandle parentHandle, Args&& ... args);

        bool Remove(const ComponentHandle handle);

        bool Contains(const ComponentHandle handle) const;

        T * GetPointerTo(const ComponentHandle handle);

        template<class Func>
        void ForEach(Func func);

        void Clear();

    private:
        engine::ComponentIndexPair GetIndexPairFromHandle(const ComponentHandle handle) const;
        void MapHandleToIndexPair(const ComponentHandle handle, const engine::ComponentIndexPair targetIndex);
        engine::ComponentIndexPair AllocateNew(T ** newItemOut);

        bool m_isReserveSizeMaxSize;
        uint32_t m_poolSize;
        std::vector<engine::alloc::Pool<T>> m_poolArray;
        std::unordered_map<ComponentHandle, engine::ComponentIndexPair> m_indexMap;
        HandleManager<ComponentHandle> m_handleManager;
};

template<class T>
ComponentSystem<T>::ComponentSystem(const uint32_t reserveSize, bool isReserveSizeMaxSize)
    : m_isReserveSizeMaxSize { isReserveSizeMaxSize },
      m_poolSize{ reserveSize },
      m_poolArray {}
{
    m_poolArray.emplace_back(engine::alloc::Pool<T>(m_poolSize));
}

template<class T>
void ComponentSystem<T>::Clear()
{
    m_poolArray.clear();
    m_poolArray.shrink_to_fit();
    m_poolArray.emplace_back(engine::alloc::Pool<T>(m_poolSize));
    m_indexMap.clear();
    m_handleManager.Reset();
}

template<class T>
template<class Func>
void ComponentSystem<T>::ForEach(Func func)
{
    for(auto & pool : m_poolArray)
    {
        pool.ForEach(func);
    }
}

template<class T>
engine::ComponentIndexPair ComponentSystem<T>::AllocateNew(T ** newItemOut)
{
    for(uint32_t i = 0; i < m_poolArray.size(); ++i)
    {
        if (!m_poolArray[i].IsFull())
        {
            auto freePos = m_poolArray[i].Alloc(newItemOut);
            return { i, freePos };
        }
    }

    // The first pool in the pool array is full, and we have set a hard limit on total items.
    if (m_isReserveSizeMaxSize)
    {
        throw std::runtime_error("ComponentSystem::AllocateNew - Pool is at max capacity and an additional item add was attempted.");
    }

    m_poolArray.push_back(engine::alloc::Pool<T>(m_poolSize));
    uint32_t poolIndex = m_poolArray.size() - 1;
    uint32_t freePos = m_poolArray.back().Alloc(newItemOut);
    return { poolIndex, freePos };
}

template<class T>
template<class ... Args>
ComponentHandle ComponentSystem<T>::Add(const EntityHandle parentHandle, Args&& ... args)
{
    T * component = nullptr;
    auto indexPair = AllocateNew(&component);
    auto handle = m_handleManager.GenerateNewHandle();
    *component = T(handle, parentHandle, (args)...);
    MapHandleToIndexPair(handle, indexPair);
    return handle;
}

template<class T>
bool ComponentSystem<T>::Remove(const ComponentHandle handle)
{
    if (!Contains(handle))
    {
        return false;
    }

    auto removePair = GetIndexPairFromHandle(handle);
    auto & owningPool = m_poolArray[removePair.indexInPoolCollection];
    owningPool.Free(removePair.indexInPool);

    if (m_indexMap.erase(handle) != 1)
    {
        throw std::runtime_error("ComponentSystem::Remove - unexpected erase result");
    }

    return true;
}

template<class T>
bool ComponentSystem<T>::Contains(const ComponentHandle handle) const
{
    return m_indexMap.count(handle) > 0;
}

template<class T>
T* ComponentSystem<T>::GetPointerTo(const ComponentHandle handle)
{
    if (!Contains(handle))
    {
        return nullptr;
    }

    engine::ComponentIndexPair pair = GetIndexPairFromHandle(handle);
    return m_poolArray[pair.indexInPoolCollection].GetPtrTo(pair.indexInPool);
}

template<class T>
ComponentHandle ComponentSystem<T>::GetCurrentHandle()
{
    return m_handleManager.GetCurrent();
}

template<class T>
engine::ComponentIndexPair ComponentSystem<T>::GetIndexPairFromHandle(const ComponentHandle handle) const
{
    if (!Contains(handle))
    {
        throw std::runtime_error("ComponentSystem::GetIndexPairFromHandle - bad handle");
    }
    return m_indexMap.at(handle);
}

template<class T>
void ComponentSystem<T>::MapHandleToIndexPair(const ComponentHandle handle, const engine::ComponentIndexPair pair)
{
    if (!Contains(handle))
    {
        m_indexMap.emplace(handle, pair);
    }
}
} // end namespace nc::ecs
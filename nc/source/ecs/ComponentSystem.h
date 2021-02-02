#pragma once

#include "EntityHandle.h"
#include "engine/alloc/Pool.h"
#include "DebugUtils.h"
#include "Profiler.h"

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

        template<class ... Args> T* Add(EntityHandle parentHandle, Args&& ... args);
        bool Remove(EntityHandle parentHandle);
        bool Contains(EntityHandle parentHandle) const;
        T* GetPointerTo(EntityHandle parentHandle);
        template<class Func> void ForEach(Func func);
        void Clear();

    private:
        engine::ComponentIndexPair GetIndexPairFromHandle(EntityHandle parentHandle) const;
        void MapHandleToIndexPair(EntityHandle parentHandle, engine::ComponentIndexPair targetIndex);
        void AddPool();

        bool m_isReserveSizeMaxSize;
        uint32_t m_poolSize;
        std::vector<engine::alloc::Pool<T>> m_poolArray;
        std::unordered_map<EntityHandle, engine::ComponentIndexPair, EntityHandle::Hash> m_indexMap;
};

template<class T>
ComponentSystem<T>::ComponentSystem(const uint32_t reserveSize, bool isReserveSizeMaxSize)
    : m_isReserveSizeMaxSize { isReserveSizeMaxSize },
      m_poolSize{ reserveSize },
      m_poolArray {},
      m_indexMap{10u, EntityHandle::Hash()}
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
}

template<class T>
template<class Func>
void ComponentSystem<T>::ForEach(Func func)
{
    NC_PROFILE_BEGIN(debug::profiler::Filter::Engine);
    for(auto & pool : m_poolArray)
    {
        pool.ForEach(func);
    }
    NC_PROFILE_END();
}

template<class T>
template<class ... Args>
T* ComponentSystem<T>::Add(EntityHandle parentHandle, Args&& ... args)
{
    auto pool = std::find_if_not(m_poolArray.begin(), m_poolArray.end(), [](auto& pool)
    {
        return pool.IsFull();
    });

    if(pool == m_poolArray.end())
    {
        AddPool();
        pool = m_poolArray.end() - 1;
    }

    T* component = nullptr;
    uint32_t posInPool = pool->Alloc(&component, parentHandle, std::forward<Args>(args)...);
    uint32_t poolIndex = pool - m_poolArray.begin();
    MapHandleToIndexPair(parentHandle, {poolIndex, posInPool});
    return component;
}

template<class T>
bool ComponentSystem<T>::Remove(EntityHandle handle)
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
bool ComponentSystem<T>::Contains(EntityHandle handle) const
{
    return m_indexMap.count(handle) > 0;
}

template<class T>
T* ComponentSystem<T>::GetPointerTo(EntityHandle handle)
{
    if (!Contains(handle))
    {
        return nullptr;
    }

    engine::ComponentIndexPair pair = GetIndexPairFromHandle(handle);
    return m_poolArray[pair.indexInPoolCollection].GetPtrTo(pair.indexInPool);
}

template<class T>
engine::ComponentIndexPair ComponentSystem<T>::GetIndexPairFromHandle(EntityHandle handle) const
{
    if (!Contains(handle))
    {
        throw std::runtime_error("ComponentSystem::GetIndexPairFromHandle - bad handle");
    }
    return m_indexMap.at(handle);
}

template<class T>
void ComponentSystem<T>::MapHandleToIndexPair(EntityHandle handle, engine::ComponentIndexPair pair)
{
    if (!Contains(handle))
    {
        m_indexMap.emplace(handle, pair);
    }
}

template<class T>
void ComponentSystem<T>::AddPool()
{
    if (m_isReserveSizeMaxSize)
    {
        throw std::runtime_error("ComponentSystem::AllocateNew - Pool is at max capacity and an additional item add was attempted.");
    }

    m_poolArray.push_back(engine::alloc::Pool<T>(m_poolSize));
}
} // end namespace nc::ecs
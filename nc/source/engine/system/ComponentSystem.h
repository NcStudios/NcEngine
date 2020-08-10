#pragma once

#include "NcCommonTypes.h"
#include "HandleManager.h"
#include "engine/alloc/PoolArray.h"
#include "NcDebug.h"

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

namespace system
{

template<class T>
class ComponentSystem
{
    public:
        ComponentSystem(const uint32_t reserveSize = 100u);
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

    protected:
        ComponentIndexPair GetIndexPairFromHandle(const ComponentHandle handle) const;
        void MapHandleToIndexPair(const ComponentHandle handle, const ComponentIndexPair targetIndex);
        ComponentIndexPair AllocateNew(T ** newItemOut);

    private:
        uint32_t m_poolSize;
        std::vector<alloc::PoolArray<T>> m_poolArrays;
        std::unordered_map<ComponentHandle, ComponentIndexPair> m_indexMap;
        HandleManager<ComponentHandle> m_handleManager;      
};

template<class T>
ComponentSystem<T>::ComponentSystem(const uint32_t reserveSize)
    : m_poolSize{ reserveSize },
        m_poolArrays {}
{
    m_poolArrays.emplace_back(alloc::PoolArray<T>(m_poolSize));
}

template<class T>
void ComponentSystem<T>::Clear()
{
    m_poolArrays.clear();
    m_poolArrays.shrink_to_fit();
    m_poolArrays.emplace_back(alloc::PoolArray<T>(m_poolSize));
    m_indexMap.clear();
    m_handleManager.Reset();
}

template<class T>
template<class Func>
void ComponentSystem<T>::ForEach(Func func)
{
    for(auto & pool : m_poolArrays)
    {
        pool.ForEach(func);
    }
}

template<class T>
ComponentIndexPair ComponentSystem<T>::AllocateNew(T ** newItemOut)
{
    for(uint32_t i = 0; i < m_poolArrays.size(); ++i)
    {
        if (!m_poolArrays[i].IsFull())
        {
            auto freePos = m_poolArrays[i].Alloc(newItemOut);
            return { i, freePos };
        }
    }

    m_poolArrays.push_back(alloc::PoolArray<T>(m_poolSize));
    uint32_t poolIndex = m_poolArrays.size() - 1;
    uint32_t freePos = m_poolArrays.back().Alloc(newItemOut);
    return { poolIndex, freePos };
}

template<class T>
template<class ... Args>
ComponentHandle ComponentSystem<T>::Add(const EntityHandle parentHandle, Args&& ... args)
{
    T * component = nullptr;
    auto indexPair = AllocateNew(&component);
    *component = T((args)...);
    component->SetMemoryState(MemoryState::Valid);
    auto handle = m_handleManager.GenerateNewHandle();
    component->Register(handle, parentHandle);
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
    auto & owningPool = m_poolArrays[removePair.indexInPoolCollection];
    owningPool.GetPtrTo(removePair.indexInPool)->SetMemoryState(MemoryState::Invalid);
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

    ComponentIndexPair pair = GetIndexPairFromHandle(handle);
    return m_poolArrays[pair.indexInPoolCollection].GetPtrTo(pair.indexInPool);
}

template<class T>
ComponentHandle ComponentSystem<T>::GetCurrentHandle()
{
    return m_handleManager.GetCurrent();
}

template<class T>
ComponentIndexPair ComponentSystem<T>::GetIndexPairFromHandle(const ComponentHandle handle) const
{
    if (!Contains(handle))
    {
        throw std::runtime_error("ComponentSystem::GetIndexPairFromHandle - bad handle");
    }
    return m_indexMap.at(handle);
}

template<class T>
void ComponentSystem<T>::MapHandleToIndexPair(const ComponentHandle handle, const ComponentIndexPair pair)
{
    if (!Contains(handle))
    {
        m_indexMap.emplace(handle, pair);
    }
}

} //end namespace system 
} //end namespace nc::engine
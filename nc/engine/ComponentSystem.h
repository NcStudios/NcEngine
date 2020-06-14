#pragma once

#include "Common.h"
#include "HandleManager.h"
#include "views/EntityView.h"
#include "alloc/BlockAllocator.h"
#include "debug/NcException.h"

#include <vector>
#include <unordered_map>
#include <memory>

namespace nc::engine
{

struct ComponentIndexPair
{
    uint32_t indexInBlockCollection;
    uint32_t indexInBlock;
};

template<class T>
class ComponentSystem
{
    public:
        ComponentSystem(const uint32_t reserveSize = 100u);
        virtual ~ComponentSystem() = default;
        
        ComponentHandle GetCurrentHandle();

        template<class ... Args>
        ComponentHandle Add(const EntityView parentView, Args&& ... args);

        bool Remove(const ComponentHandle handle);

        bool Contains(const ComponentHandle handle) const;

        T * GetPointerTo(const ComponentHandle handle);

        template<class Func>
        void ForEach(Func func);        

    protected:
        ComponentIndexPair GetIndexPairFromHandle(const ComponentHandle handle) const;
        void MapHandleToIndexPair(const ComponentHandle handle, const ComponentIndexPair targetIndex);
        ComponentIndexPair AllocateNew(T ** newItemOut);

    private:
        uint32_t m_blockSize;
        std::vector<alloc::Block<T>> m_blocks;
        std::unordered_map<ComponentHandle, ComponentIndexPair> m_indexMap;
        HandleManager<ComponentHandle> m_handleManager;      
};

template<class T>
ComponentSystem<T>::ComponentSystem(const uint32_t reserveSize)
    : m_blockSize{ reserveSize },
        m_blocks {}
{
    m_blocks.emplace_back(alloc::Block<T>(m_blockSize));
}

template<class T>
template<class Func>
void ComponentSystem<T>::ForEach(Func func)
{
    for(auto & block : m_blocks)
    {
        block.ForEach(func);
    }
}

template<class T>
ComponentIndexPair ComponentSystem<T>::AllocateNew(T ** newItemOut)
{
    for(uint32_t i = 0; i < m_blocks.size(); ++i)
    {
        if (!m_blocks[i].IsFull())
        {
            auto freePos = m_blocks[i].GetNextFree(newItemOut);
            return { i, freePos };
        }
    }

    m_blocks.push_back(alloc::Block<T>(m_blockSize));
    uint32_t blockIndex = m_blocks.size() - 1;
    uint32_t freePos = m_blocks.back().GetNextFree(newItemOut);
    return { blockIndex, freePos };
}

template<class T>
template<class ... Args>
ComponentHandle ComponentSystem<T>::Add(const EntityView parentView, Args&& ... args)
{
    T * component = nullptr;
    auto indexPair = AllocateNew(&component);
    //T newComp = T((args)...);
    //*component = std::move(newComp);
    *component = T((args)...);
    
    
    auto handle = m_handleManager.GenerateNewHandle();
    component->Register(handle, parentView);
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

    auto toRemove = GetIndexPairFromHandle(handle);
    m_blocks[toRemove.indexInBlockCollection].Free(toRemove.indexInBlock);

    if (m_indexMap.erase(handle) != 1)
    {
        throw NcException("ComponentSystem::Remove - unexpected erase result");
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


    return m_blocks[pair.indexInBlockCollection].GetPtrTo(pair.indexInBlock);
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
        throw NcException("ComponentSystem::GetIndexPairFromHandle - bad handle");
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

}
#pragma once

#include "NcCommonTypes.h"
#include "NCDebug.h"

#include <memory>
#include <vector>
#include <stdlib.h>
#include <algorithm>

namespace nc::engine::alloc
{

template<class T>
class PoolArray
{
    public:
        PoolArray(uint32_t maxItemCount);
        ~PoolArray();
        PoolArray(PoolArray&& other);
        PoolArray & operator=(PoolArray&& other);

        uint32_t Alloc(T ** out);
        void Free(uint32_t pos);
        T * GetPtrTo(uint32_t pos);
        bool IsFull() const;

        template<class Func>
        void ForEach(Func func);

    private:
        uint32_t nextFree;
        uint32_t maxItems;
        std::vector<uint32_t> unused;
        std::unique_ptr<T[]> data;
};


template<class T>
PoolArray<T>::PoolArray(unsigned int maxItemCount)
    : nextFree{ 0u },
      maxItems{ maxItemCount },
      data { std::make_unique<T[]>(maxItemCount) }
{
}

template<class T>
PoolArray<T>::~PoolArray()
{
}

template<class T>
PoolArray<T>::PoolArray(PoolArray&& other)
    : nextFree{ std::exchange(other.nextFree, 0u) },
      maxItems{ std::exchange(other.maxItems, 0u) },
      data{ std::move(other.data) }
{
}

template<class T>
PoolArray<T> & PoolArray<T>::operator=(PoolArray<T>&& other)
{
    nextFree = std::exchange(other.nextFree, 0u);
    maxItems = std::exchange(other.maxItems, 0u);
    data = std::move(other.data);
}

template<class T>
uint32_t PoolArray<T>::Alloc(T ** out)
{
    if (IsFull())
    {
        throw std::runtime_error("PoolArray::Alloc - PoolArray full");
    }

    if(!unused.empty())
    {
        auto index = unused.back();
        unused.pop_back();
        *out = &data[index];
        return index;
    }

    *out = &data[nextFree];
    return nextFree++;
}

template<class T>
void PoolArray<T>::Free(uint32_t pos)
{
    /** Not currently doing a valid memory check as GetPtrTo
     *  is called right before this and performs a check. Also,
     *  the mem state is set to invalid in that call so the 
     *  standard if state==invalid check fails. */

    if (nextFree - 1 == pos)
    {
        --nextFree;
        return;
    }

    unused.push_back(pos);
}

template<class T>
T * PoolArray<T>::GetPtrTo(uint32_t pos)
{
    if (data[pos].GetMemoryState() == MemoryState::Invalid)
    {
        throw std::runtime_error("PoolArray::GetPtrTo - attempt to get unowned component");
    }

    return &data[pos];
}

template<class T>
bool PoolArray<T>::IsFull() const
{
    return (unused.empty() && nextFree == maxItems);
}

template<class T>
template<class Func>
void PoolArray<T>::ForEach(Func func)
{
    unsigned int current = 0u;

    while(current < nextFree)
    {
        if (data[current].GetMemoryState() == MemoryState::Invalid)
        {
            ++current; continue;
        }

        func(data[current]);
        ++current;
    }
}

}
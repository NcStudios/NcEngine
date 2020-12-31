#pragma once

#include "NcCommonTypes.h"
#include "DebugUtils.h"

#include <memory>
#include <vector>
#include <stdlib.h>
#include <algorithm>

namespace nc::engine::alloc
{

template<class T>
class Pool
{
    public:
        Pool(uint32_t maxItemCount);
        ~Pool();
        Pool(Pool&& other);
        Pool & operator=(Pool&& other);

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
Pool<T>::Pool(unsigned int maxItemCount)
    : nextFree{ 0u },
      maxItems{ maxItemCount },
      data { std::make_unique<T[]>(maxItemCount) }
{
}

template<class T>
Pool<T>::~Pool()
{
}

template<class T>
Pool<T>::Pool(Pool&& other)
    : nextFree{ std::exchange(other.nextFree, 0u) },
      maxItems{ std::exchange(other.maxItems, 0u) },
      data{ std::move(other.data) }
{
}

template<class T>
Pool<T> & Pool<T>::operator=(Pool<T>&& other)
{
    nextFree = std::exchange(other.nextFree, 0u);
    maxItems = std::exchange(other.maxItems, 0u);
    data = std::move(other.data);
}

template<class T>
uint32_t Pool<T>::Alloc(T ** out)
{
    if (IsFull())
    {
        throw std::runtime_error("Pool::Alloc - Pool full");
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
void Pool<T>::Free(uint32_t pos)
{
    if (data[pos].m_memoryState == MemoryState::Invalid)
    {
        throw std::runtime_error("Pool::GetPtrTo - attempt to get unowned component");
    }

    data[pos].m_memoryState = MemoryState::Invalid;

    if (nextFree - 1 == pos)
    {
        --nextFree;
        return;
    }

    unused.push_back(pos);
}

template<class T>
T * Pool<T>::GetPtrTo(uint32_t pos)
{
    if (data[pos].m_memoryState == MemoryState::Invalid)
    {
        throw std::runtime_error("Pool::GetPtrTo - attempt to get unowned component");
    }

    return &data[pos];
}

template<class T>
bool Pool<T>::IsFull() const
{
    return (unused.empty() && nextFree == maxItems);
}

template<class T>
template<class Func>
void Pool<T>::ForEach(Func func)
{
    unsigned int current = 0u;

    while(current < nextFree)
    {
        if (data[current].m_memoryState == MemoryState::Invalid)
        {
            ++current; continue;
        }

        func(data[current]);
        ++current;
    }
}

}
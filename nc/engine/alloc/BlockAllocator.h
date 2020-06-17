#pragma once

#include "Common.h"
#include "debug/NCException.h"

#include <memory>
#include <vector>
#include <stdlib.h>
#include <algorithm>

namespace nc::engine::alloc
{

template<class T>
class Block
{
    public:
        Block(uint32_t maxItemCount);
        ~Block();
        Block(Block&& other);
        Block & operator=(Block&& other);

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
Block<T>::Block(unsigned int maxItemCount)
    : nextFree{ 0u },
      maxItems{ maxItemCount },
      data { std::make_unique<T[]>(maxItemCount) }
{
}

template<class T>
Block<T>::~Block()
{
}

template<class T>
Block<T>::Block(Block&& other)
    : nextFree{ std::exchange(other.nextFree, 0u) },
      maxItems{ std::exchange(other.maxItems, 0u) },
      data{ std::move(other.data) }
{
}

template<class T>
Block<T> & Block<T>::operator=(Block<T>&& other)
{
    nextFree = std::exchange(other.nextFree, 0u);
    maxItems = std::exchange(other.maxItems, 0u);
    data = std::move(other.data);
}

template<class T>
uint32_t Block<T>::Alloc(T ** out)
{
    if (IsFull())
    {
        throw NcException("Block::Alloc - block full");
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
void Block<T>::Free(uint32_t pos)
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
T * Block<T>::GetPtrTo(uint32_t pos)
{
    if (data[pos].GetMemoryState() == MemoryState::Invalid)
    {
        throw NcException("Block::GetPtrTo - attempt to get unowned component");
    }

    return &data[pos];
}

template<class T>
bool Block<T>::IsFull() const
{
    return (unused.empty() && nextFree == maxItems);
}

template<class T>
template<class Func>
void Block<T>::ForEach(Func func)
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
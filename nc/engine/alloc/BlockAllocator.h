#pragma once

#include "Common.h"

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
        Block(unsigned int maxItemCount);
        ~Block();
        Block(Block&& other);
        Block & operator=(Block&& other);

        unsigned int GetNextFree(T ** out);
        void Free(unsigned int pos);

        T * GetPtrTo(unsigned int pos);

        bool IsFull() const;

        template<class Func>
        void ForEach(Func func);

    private:
        unsigned int nextFree;
        unsigned int maxItems;
        std::vector<unsigned int> unused;
        std::unique_ptr<T[]> data;
        //void * data;
};


    template<class T>
    Block<T>::Block(unsigned int maxItemCount)
        : nextFree{ 0 },
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
        : nextFree{ std::exchange(other.nextFree, 0) },
          maxItems{ std::exchange(other.maxItems, 0) },
          data{ std::move(other.data) }
    {
    }

    template<class T>
    Block<T> & Block<T>::operator=(Block<T>&& other)
    {
        nextFree = std::exchange(other.nextFree, 0);
        maxItems = std::exchange(other.maxItems, 0);
        data = std::move(other.data);
    }

    template<class T>
    unsigned int Block<T>::GetNextFree(T ** out)
    {
        if (IsFull())
        {
            *out = nullptr;
            return 0; //doesn't make sense to do this
        }

        if(!unused.empty())
        {
            auto index = unused.back();
            unused.pop_back();
            *out = &data[index];
            //*out = &static_cast<T*>(data)[index];
            return index;
        }
        *out = &data[nextFree];
        //*out = &static_cast<T*>(data)[nextFree];
        return nextFree++;
    }

    template<class T>
    void Block<T>::Free(unsigned int pos)
    {
        data[pos].m_handle = NullHandle;

        //dont clear any mem?
        if (nextFree - 1 == pos)
            --nextFree;
        else
            unused.push_back(pos);
    }

    template<class T>
    T * Block<T>::GetPtrTo(unsigned int pos)
    {
        //debug, maybe don't need to stay
        if (nextFree < pos)
            throw ("BlockAllocator::GetPtrTo - bad pos arg");
        for(auto& val : unused)
        {
            if (val == pos)
                throw("BlockAllocator::GetPtrTo - bad pos arg");
        }
        return &data[pos];
        //return &static_cast<T*>(data)[pos];
        //return static_cast<T*>(&data[pos]);
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
            // if (unused.cend() != std::find(unused.cbegin(), unused.cend(), current))
            // {
            //     ++current;
            //     continue;
            // }
            
            if (data[current].m_handle == NullHandle)
            {
                ++current; continue;
            }

            func(data[current]);
            ++current;
        }
    }

}
#pragma once

#include "NcCommonTypes.h"
#include "DebugUtils.h"

#include <algorithm>
#include <memory>
#include <type_traits>
#include <vector>
#include <stdlib.h>

namespace nc::engine::alloc
{
    template<class T>
    class Pool
    {
        using Align_t = std::aligned_storage_t<sizeof(T), alignof(T)>;

        public:
            Pool(uint32_t maxItemCount);

            template<class ... Args>
            uint32_t Alloc(Args&& ...args);

            void Free(uint32_t pos);
            T * GetPtrTo(uint32_t pos);
            bool IsFull() const;

            template<class Func>
            void ForEach(Func func);

        private:
            bool IsAllocated(uint32_t pos);

            uint32_t nextFree;
            uint32_t maxItems;
            std::vector<uint32_t> gaps;
            std::unique_ptr<Align_t[]> data;
    };


    template<class T>
    Pool<T>::Pool(unsigned int maxItemCount)
        : nextFree{ 0u },
          maxItems{ maxItemCount },
          gaps{},
          data{ std::make_unique<Align_t[]>(maxItemCount) }
    {
    }

    template<class T>
    template<class ... Args>
    uint32_t Pool<T>::Alloc(Args&& ...args)
    {
        IF_THROW(IsFull(), "Pool::Alloc - Pool full");

        uint32_t freePos;

        if(gaps.empty())
        {
            freePos = nextFree++;
        }
        else
        {
            freePos = gaps.back();
            gaps.pop_back();
        }

        T* component = reinterpret_cast<T*>(&data[freePos]);
        component = new(component) T(std::forward<Args>(args)...);
        return freePos;
    }

    template<class T>
    void Pool<T>::Free(uint32_t pos)
    {
        IF_THROW(!IsAllocated(pos), "Pool::Free - attempt to free unallocated component");

        reinterpret_cast<T*>(&data[pos])->~T();

        if (nextFree - 1 == pos)
        {
            --nextFree; return;
        }

        gaps.push_back(pos);
        std::sort(gaps.begin(), gaps.end());
    }

    template<class T>
    T * Pool<T>::GetPtrTo(uint32_t pos)
    {
        IF_THROW(!IsAllocated(pos), "Pool::GetPtrTo - attempt to get unowned component");
        return reinterpret_cast<T*>(&data[pos]);
    }

    template<class T>
    bool Pool<T>::IsFull() const
    {
        return (nextFree == maxItems && gaps.empty());
    }

    template<class T>
    template<class Func>
    void Pool<T>::ForEach(Func func)
    {
        unsigned dataIndex = 0u;
        auto gapCurrent = gaps.cbegin();
        auto gapEnd = gaps.cend();

        while(dataIndex < nextFree)
        {
            if(gapCurrent != gapEnd && dataIndex == *gapCurrent)
            {
                ++gapCurrent; ++dataIndex; continue;
            }

            func(*reinterpret_cast<T*>(&data[dataIndex++]));
        }
    }

    template<class T>
    bool Pool<T>::IsAllocated(uint32_t pos)
    {
        return pos < nextFree && !std::binary_search(gaps.cbegin(), gaps.cend(), pos);
    }
}
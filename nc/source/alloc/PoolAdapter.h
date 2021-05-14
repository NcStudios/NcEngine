#pragma once

#include "Allocator.h"

#include <algorithm>
#include <span>

namespace nc::alloc
{
    /** potential todos
     *  -could expose iterators
     *  -could change from sorted/unsorted based on template parameter */

    /** Helper base class for managing pool memory resource lifetime. */
    template<class T>
    class PoolResourceScope
    {
        protected:
            using allocator_type = alloc::PoolAllocator<T>;
            PoolResourceScope(size_t count) { allocator_type::create_memory_resource(sizeof(T) * count); }
            ~PoolResourceScope() { allocator_type::destroy_memory_resource(); }
    };

    /** Provides a more container-like interface for memory pools. */
    template<class T>
    class PoolAdapter final : private PoolResourceScope<T>
    {
        public:
            explicit PoolAdapter(size_t count);

            template<class... Args>
            T* Add(Args&&... args);

            template<class Predicate>
            bool RemoveIf(Predicate predicate);

            template<class Predicate>
            bool Contains(Predicate predicate) const;

            template<class Predicate>
            T* Get(Predicate predicate);
            
            std::span<T*> GetActiveRange() noexcept;
            
            void Clear();

        private:
            std::vector<T*> m_data;
            PoolResourceScope<T>::allocator_type m_allocator;
    };

    template<class T>
    PoolAdapter<T>::PoolAdapter(size_t count)
        : PoolResourceScope<T>{count},
          m_data{},
          m_allocator{}
    {
    }

    template<class T>
    std::span<T*> PoolAdapter<T>::GetActiveRange() noexcept
    {
        return std::span<T*>{m_data};
    }

    template<class T>
    void PoolAdapter<T>::Clear()
    {
        for(auto* item : m_data)
            m_allocator.deallocate(item, 1u);

        m_data.clear();
        m_data.shrink_to_fit();
        m_allocator.clear_memory_resource();
    }

    template<class T>
    template<class ... Args>
    T* PoolAdapter<T>::Add(Args&& ... args)
    {
        auto* ptr = m_allocator.allocate(1);
        ptr = new(ptr) T{std::forward<Args>(args)...};
        auto pos = std::ranges::upper_bound(m_data, ptr, [](T* add, T* existing) { return add < existing; });
        m_data.insert(pos, ptr);
        return ptr;
    }

    template<class T>
    template<class Predicate>
    bool PoolAdapter<T>::RemoveIf(Predicate predicate)
    {
        if(auto* ptr = Get(predicate))
        {
            m_allocator.deallocate(ptr, 1);
            std::erase(m_data, ptr);
            return true;
        }

        return false;
    }

    template<class T>
    template<class Predicate>
    T* PoolAdapter<T>::Get(Predicate predicate)
    {
        auto pos = std::ranges::find_if(m_data, predicate);
        return pos == m_data.end() ? nullptr : *pos;
    }

    template<class T>
    template<class Predicate>
    bool PoolAdapter<T>::Contains(Predicate predicate) const
    {
        return m_data.end() != std::ranges::find_if(m_data, predicate);
    }
} // namespace nc::alloc
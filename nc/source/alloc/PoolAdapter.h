#pragma once

#include "Allocator.h"
#include "component/Component.h"

#include <algorithm>
#include <span>

namespace nc::alloc
{
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
            using storage_policy = StoragePolicy<T>;

            explicit PoolAdapter(size_t count);
            
            template<class... Args>
            T* Add(Args&&... args);

            template<class Predicate>
            bool RemoveIf(Predicate predicate);

            template<class Predicate>
            bool Contains(Predicate predicate) const;

            template<class Predicate>
            T* Get(Predicate predicate);
            
            template<class Predicate>
            T Extract(Predicate predicate); // throws on failure

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
        if constexpr(!storage_policy::allow_trivial_destruction::value)
        {
            for(auto* item : m_data)
                m_allocator.deallocate(item, 1u);
        }

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

        if constexpr(storage_policy::sort_dense_storage_by_address::value)
        {
            auto pos = std::ranges::upper_bound(m_data, ptr, std::less<T*>());
            m_data.insert(pos, ptr);
        }
        else
        {
            m_data.push_back(ptr);
        }

        return ptr;
    }

    template<class T>
    template<class Predicate>
    bool PoolAdapter<T>::RemoveIf(Predicate predicate)
    {
        auto pos = std::ranges::find_if(m_data, predicate);
        if(pos == m_data.end())
            return false;
        
        m_allocator.deallocate(*pos, 1);

        if constexpr(storage_policy::sort_dense_storage_by_address::value)
        {
            std::erase(m_data, *pos);
        }
        else
        {
            *pos = m_data.back();
            m_data.pop_back();
        }

        return true;
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
    T PoolAdapter<T>::Extract(Predicate predicate)
    {
        auto pos = std::ranges::find_if(m_data, predicate);
        if(pos == m_data.end())
            throw std::runtime_error("PoolAdapter::Extract - Item does not exist");

        T out{std::move(**pos)};
        m_allocator.deallocate(*pos, 1);

        if constexpr(storage_policy::sort_dense_storage_by_address::value)
        {
            m_data.erase(pos);
        }
        else
        {
            *pos = m_data.back();
            m_data.pop_back();
        }

        return out;
    }

    template<class T>
    template<class Predicate>
    bool PoolAdapter<T>::Contains(Predicate predicate) const
    {
        return m_data.end() != std::ranges::find_if(m_data, predicate);
    }
} // namespace nc::alloc
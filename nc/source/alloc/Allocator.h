#pragma once

#include "MemoryResource.h"

namespace nc::alloc
{
    /** Allocator for a single type. Not valid with containers. Size parameters
     *  for allocate/deallocate are not used. Calling clear_memory_resource or
     *  destroy_memory_resource will not call destructors. If this is needed,
     *  explicitly call deallocate. */
    template<class T, class Tag = void>
    class PoolAllocator
    {
        public:
            using value_type = T;
            using propagate_on_container_copy_assignment = std::false_type;
            using propagate_on_container_move_assignment = std::false_type;
            using propagate_on_container_swap = std::false_type;
            using memory_resource_type = StaticPoolResource<T, Tag>;

            PoolAllocator();

            value_type* allocate(size_t n);
            void deallocate(value_type* ptr, size_t n) noexcept;
            void clear_memory_resource();

            static void create_memory_resource(size_t size);
            static void destroy_memory_resource();

        private:
            memory_resource_type* m_memoryResource;
    };

    /** An allocator intended for temporary (e.g. per-frame) allocations. Calling
     *  clear_memory_resource will deallocate everything in O(1). Note that
     *  deallocate does nothing and destructors will only be called when destroy
     *  is explicitly called. */
    template<class T, class Tag = void>
    class LinearAllocator
    {
        template<class U, class UTag>
        friend class LinearAllocator;

        public:
            using value_type = T;
            using propagate_on_container_copy_assignment = std::true_type;
            using propagate_on_container_move_assignment = std::true_type;
            using propagate_on_container_swap = std::true_type;
            using memory_resource_type = LinearBufferResource<Tag>;

            template<class U>
            struct rebind { using other = LinearAllocator<U, Tag>; };

            LinearAllocator();
            ~LinearAllocator() = default;
            LinearAllocator(const LinearAllocator& other) = default;
            LinearAllocator(LinearAllocator&& other) = default;
            template<class U> LinearAllocator(const LinearAllocator<U>& other);

            value_type* allocate(size_t count);
            void deallocate(value_type* ptr, size_t n) noexcept;
            template<class U, class... Args> void construct(U* ptr, Args&&... args);
            void destroy(value_type* ptr);
            void clear_memory_resource();

            static void create_memory_resource(size_t size);
            static void destroy_memory_resource();

        private:
            memory_resource_type* m_memoryResource;
    };

    template<class T, class U, class Tag>
    bool operator==(const LinearAllocator<T, Tag>&, const LinearAllocator<U, Tag>&) noexcept { return true; }

    template<class T, class U, class Tag>
    bool operator!=(const LinearAllocator<T, Tag>& lhs, const LinearAllocator<U, Tag>& rhs) noexcept { return !(lhs == rhs); }

    template<class T, class Tag>
    bool operator==(const PoolAllocator<T, Tag>&, const PoolAllocator<T, Tag>&) noexcept { return true; }

    template<class T, class Tag>
    bool operator!=(const PoolAllocator<T, Tag>& lhs, const PoolAllocator<T, Tag>& rhs) noexcept { return !(lhs == rhs); }

    template<class T, class U, class TTag, class UTag>
    bool operator==(const PoolAllocator<T, TTag>&, const PoolAllocator<U, UTag>&) noexcept { return false; }

    template<class T, class U, class TTag, class UTag>
    bool operator!=(const PoolAllocator<T, TTag>& lhs, const PoolAllocator<U, UTag>& rhs) noexcept { return !(lhs == rhs); }

    /** PoolAllocator */
    template<class T, class Tag>
    PoolAllocator<T, Tag>::PoolAllocator()
        : m_memoryResource{GetMemoryResource<memory_resource_type>()}
    {
    }

    template<class T, class Tag>
    PoolAllocator<T, Tag>::value_type* PoolAllocator<T, Tag>::allocate(size_t)
    {
        return m_memoryResource->allocate(1u, alignof(value_type));
    }

    template<class T, class Tag>
    void PoolAllocator<T, Tag>::deallocate(PoolAllocator::value_type* ptr, size_t) noexcept
    {
        ptr->~T();
        m_memoryResource->deallocate(ptr, 1u);
    }

    template<class T, class Tag>
    void PoolAllocator<T, Tag>::clear_memory_resource()
    {
        m_memoryResource->free_all();
    }

    template<class T, class Tag>
    void PoolAllocator<T, Tag>::create_memory_resource(size_t size)
    {
        nc::alloc::CreateMemoryResource<memory_resource_type>(size);
    }

    template<class T, class Tag>
    void PoolAllocator<T, Tag>::destroy_memory_resource()
    {
        nc::alloc::DestroyMemoryResource<memory_resource_type>();
    }

    /** LinearAllocator */
    template<class T, class Tag>
    LinearAllocator<T, Tag>::LinearAllocator()
        : m_memoryResource{GetMemoryResource<memory_resource_type>()}
    {
    }

    template<class T, class Tag>
    template<class U>
    LinearAllocator<T, Tag>::LinearAllocator(const LinearAllocator<U>& other)
        : m_memoryResource{other.m_memoryResource}
    {
    }

    template<class T, class Tag>
    LinearAllocator<T, Tag>::value_type* LinearAllocator<T, Tag>::allocate(size_t count)
    {
        return static_cast<value_type*>(m_memoryResource->allocate(count * sizeof(value_type), 16u));
    }

    template<class T, class Tag>
    void LinearAllocator<T, Tag>::deallocate(value_type*, size_t) noexcept
    {
    }

    template<class T, class Tag>
    template<class U, class... Args>
    void LinearAllocator<T, Tag>::construct(U* ptr, Args&&... args)
    {
        new(ptr) U(std::forward<Args>(args)...);
    }

    template<class T, class Tag>
    void LinearAllocator<T, Tag>::destroy(value_type* ptr)
    {
        ptr->~T();
    }

    template<class T, class Tag>
    void LinearAllocator<T, Tag>::clear_memory_resource()
    {
        m_memoryResource->free_all();
    }

    template<class T, class Tag>
    void LinearAllocator<T, Tag>::create_memory_resource(size_t size)
    {
        nc::alloc::CreateMemoryResource<memory_resource_type>(size);
    }

    template<class T, class Tag>
    void LinearAllocator<T, Tag>::destroy_memory_resource()
    {
        nc::alloc::DestroyMemoryResource<memory_resource_type>();
    }
} // namespace nc::alloc
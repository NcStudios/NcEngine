#pragma once

#include "MemoryResource.h"

namespace nc::alloc
{
    /** Allocator for a single type. Not valid with containers.
     *  @todo Two allocators templated on the same resource will share that
     *  resource. Not sure if this is what we want. This could create its own
     *  resource, or it could take a ptr in c'tor? */
    template<class Resource>
        requires MemoryResource<Resource>
    class FixedTypeAllocator
    {
        public:
            using value_type = Resource::value_type;
            using propagate_on_container_copy_assignment = std::false_type;
            using propagate_on_container_move_assignment = std::false_type;
            using propagate_on_container_swap = std::false_type;

            value_type* allocate(size_t n);
            void deallocate(value_type* ptr, size_t n) noexcept;
            void initialize_memory_resource(size_t size);
            void clear_memory_resource();
            void release_memory_resource() noexcept;

        private:
            static std::unique_ptr<Resource> m_memoryResource;
    };

    template<class T>
    bool operator==(const FixedTypeAllocator<T>&, const FixedTypeAllocator<T>&) noexcept { return true; }

    template<class T>
    bool operator!=(const FixedTypeAllocator<T>&, const FixedTypeAllocator<T>&) noexcept { return false; }

    template<class T, class U>
    bool operator==(const FixedTypeAllocator<T>&, const FixedTypeAllocator<U>&) noexcept { return false; }

    template<class T, class U>
    bool operator!=(const FixedTypeAllocator<T>&, const FixedTypeAllocator<U>&) noexcept { return true; }

    template<class Resource>
    std::unique_ptr<Resource> FixedTypeAllocator<Resource>::m_memoryResource = nullptr;

    template<class Resource>
    FixedTypeAllocator<Resource>::value_type* FixedTypeAllocator<Resource>::allocate(size_t n)
    {
        return m_memoryResource->allocate(n);
    }

    template<class Resource>
    void FixedTypeAllocator<Resource>::deallocate(FixedTypeAllocator::value_type* ptr, size_t n) noexcept
    {
        m_memoryResource->deallocate(ptr, n);
    }

    template<class Resource>
    void FixedTypeAllocator<Resource>::initialize_memory_resource(size_t size)
    {
        if(m_memoryResource)
            throw std::runtime_error("FixedTypeAllocator::initialize_memory_resource - Resource already exists");
        
        m_memoryResource = std::make_unique<Resource>(size);
    }

    template<class Resource>
    void FixedTypeAllocator<Resource>::clear_memory_resource()
    {
        m_memoryResource->free_all();
    }

    template<class Resource>
    void FixedTypeAllocator<Resource>::release_memory_resource() noexcept
    {
        m_memoryResource = nullptr;
    }
} // namespace nc::alloc
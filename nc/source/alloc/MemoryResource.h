#pragma once

#include "debug/Utils.h"

#include <memory>
#include <new>
#include <type_traits>
#include <vector>

namespace nc::alloc
{
    template<class Resource_t>
    class MemoryResource
    {
        public:
            static void Create(size_t size)
            {
                if(m_resource)
                    throw std::runtime_error("MemoryResource - Resource already exists");

                m_resource = std::make_unique<Resource_t>(size);
            }

            static void Destroy()
            {
                m_resource = nullptr;
            }

            static Resource_t* Get()
            {
                if(!m_resource)
                    throw std::runtime_error("MemoryResource - Resource not initialized");

                return m_resource.get();
            }

        private:
            inline static std::unique_ptr<Resource_t> m_resource = nullptr;
    };

    struct MemoryResourceBadAlloc : public std::bad_alloc
    {
        const char* what() const noexcept override
        {
            return "MemoryResource::alloc - available memory exceeded";
        }
    };

    /** Fixed-size, single-type buffer. Allocates by first backfilling any
     *  holes from deallocation, then continues sequentially. Size and
     *  alignment parameters are not used. */
    template<class T, class Tag = void>
    class StaticPoolResource
    {
        public:
            using value_type = T;
            using storage_type = std::aligned_storage_t<sizeof(T), alignof(T)>;

            StaticPoolResource(size_t size);
            ~StaticPoolResource() = default;
            StaticPoolResource(const StaticPoolResource&) = delete;
            StaticPoolResource& operator=(const StaticPoolResource&) = delete;

            T* allocate(size_t size, size_t alignment);
            void deallocate(T* ptr, size_t size) noexcept;
            void free_all();

        private:
            std::unique_ptr<storage_type[]> m_data;
            std::vector<size_t> m_free;
            size_t m_next;
            const size_t m_maxCount;
    };

    /** Fixed-size buffer for multiple types. Allocates sequentially from the
     *  buffer and only reclaims memory upon calling free_all. Calls to deallocate
     *  do nothing. */
    template<class Tag = void>
    class LinearBufferResource
    {
        public:
            LinearBufferResource(size_t size);
            ~LinearBufferResource() = default;
            LinearBufferResource(const LinearBufferResource&) = delete;
            LinearBufferResource& operator=(const LinearBufferResource&) = delete;

            void* allocate(size_t size, size_t alignment);
            void deallocate(void* ptr, size_t n) noexcept;
            void free_all();

        private:
            std::unique_ptr<char[]> m_data;
            size_t m_totalBytes;
            size_t m_freeBytes;
    };

    template<class T, class Tag>
    StaticPoolResource<T, Tag>::StaticPoolResource(size_t size)
        : m_data{std::make_unique<storage_type[]>(size / sizeof(T))},
          m_free{},
          m_next{0u},
          m_maxCount{size / sizeof(T)}
    {
    }

    /** StaticPoolResource */
    template<class T, class Tag>
    T* StaticPoolResource<T, Tag>::allocate(size_t, size_t)
    {
        if(m_next >= m_maxCount)
            throw MemoryResourceBadAlloc();

        auto index = [this]()
        {
            if(m_free.empty())
                return m_next++;

            auto out = m_free.back();
            m_free.pop_back();
            return out;
        }();

        return reinterpret_cast<T*>(&m_data[index]);
    }

    template<class T, class Tag>
    void StaticPoolResource<T, Tag>::deallocate(T* ptr, size_t) noexcept
    {
        auto index = ptr - reinterpret_cast<T*>(&m_data[0]);
        m_free.emplace_back(index);
    }

    template<class T, class Tag>
    void StaticPoolResource<T, Tag>::free_all()
    {
        m_free.clear();
        m_free.shrink_to_fit();
        m_next = 0u;
    }

    /** LinearBufferResource */
    template<class Tag>
    LinearBufferResource<Tag>::LinearBufferResource(size_t size)
        : m_data{std::make_unique<char[]>(size)},
          m_totalBytes{size},
          m_freeBytes{size}
    {
    }

    template<class Tag>
    void* LinearBufferResource<Tag>::allocate(size_t size, size_t alignment)
    {
        auto current = static_cast<void*>(m_data.get() + (m_totalBytes - m_freeBytes));
        auto alignedResult = std::align(alignment, size, current, m_freeBytes);

        if(!alignedResult)
            throw MemoryResourceBadAlloc();
        
        m_freeBytes -= size;
        return alignedResult;
    }

    template<class Tag>
    void LinearBufferResource<Tag>::deallocate(void* ptr, size_t n) noexcept
    {
    }

    template<class Tag>
    void LinearBufferResource<Tag>::free_all()
    {
        m_freeBytes = m_totalBytes;
    }
} // namespace nc::alloc
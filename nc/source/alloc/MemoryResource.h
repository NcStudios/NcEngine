#pragma once

#include <memory>
#include <new>
#include <type_traits>

namespace nc::alloc
{
    struct MemoryResourceBadAlloc : public std::bad_alloc
    {
        const char* what() const noexcept override
        {
            return "MemoryResource::alloc - available memory exceeded";
        }
    };

    template<class T>
    concept MemoryResource = requires (T t)
    {
        typename T::value_type;
        { t.allocate(size_t{1u}) } -> std::same_as<typename T::value_type*>;
        { t.deallocate(nullptr, size_t{1u}) } -> std::same_as<void>;
        { t.free_all() } -> std::same_as<void>;
    };

    /** Fixed-size, single-type buffer
     *  @note destructor/free_all will not call destructors of allocated
     *  objects. If this is required, explicity call deallocate. */
    template<class T, class Tag = void>
    class StaticPool
    {
        public:
            using value_type = T;
            using storage_type = std::aligned_storage_t<sizeof(T), alignof(T)>;

            StaticPool(size_t maxCount);
            ~StaticPool() = default;
            StaticPool(const StaticPool&) = delete;
            StaticPool& operator=(const StaticPool&) = delete;

            T* allocate(std::size_t n);
            void deallocate(T* ptr, size_t n) noexcept;
            void free_all();

        private:
            std::unique_ptr<storage_type[]> m_data;
            std::vector<size_t> m_free;
            size_t m_next;
            const size_t m_maxCount;
    };

    template<class T, class Tag>
    StaticPool<T, Tag>::StaticPool(size_t maxCount)
        : m_data{std::make_unique<storage_type[]>(maxCount)},
          m_free{},
          m_next{0u},
          m_maxCount{maxCount}
    {
    }

    template<class T, class Tag>
    T* StaticPool<T, Tag>::allocate(std::size_t)
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
    void StaticPool<T, Tag>::deallocate(T* ptr, size_t) noexcept
    {
        ptr->~T();
        auto index = ptr - reinterpret_cast<T*>(&m_data[0]);
        m_free.emplace_back(index);
    }

    template<class T, class Tag>
    void StaticPool<T, Tag>::free_all()
    {
        m_free.clear();
        m_free.shrink_to_fit();
        m_next = 0u;
    }
} // namespace nc::alloc
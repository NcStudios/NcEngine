#pragma once

#include <memory>

namespace nc::alloc
{
    template<class T, class Tag = void>
    class Pool
    {
        public:
            using value_type = T;
            using Align_t = std::aligned_storage_t<sizeof(T), alignof(T)>;

            Pool(size_t blockSize);
            Pool(const Pool&) = delete;
            Pool& operator=(const Pool&) = delete;

            T* allocate(std::size_t n);
            void deallocate(T* ptr, size_t n) noexcept;

        private:
            std::unique_ptr<Align_t[]> m_data;
            std::vector<size_t> m_free;
            size_t m_next;
    };

    template<class T, class Tag>
    Pool<T, Tag>::Pool(size_t blockSize)
        : m_data{std::make_unique<Align_t[]>(blockSize)},
          m_free{},
          m_next{0u}
    {
    }

    template<class T, class Tag>
    T* Pool<T, Tag>::allocate(std::size_t n)
    {
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
    void Pool<T, Tag>::deallocate(T* ptr, size_t n) noexcept
    {
        ptr->~T();
        size_t index = (ptr - reinterpret_cast<T*>(&m_data[0])) / sizeof(T);
        m_free.emplace_back(index);
    }
} // namespace nc::alloc
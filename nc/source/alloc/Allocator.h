#pragma once

namespace nc::alloc
{
    template<class Alloc>
    struct basic_deleter : private Alloc
    {
        void operator()(Alloc::value_type* ptr)
        {
            Alloc::deallocate(ptr, 1);
        }
    };

    template<class Resource>
    class Allocator
    {
        public:
            using value_type = Resource::value_type;
            using deleter = basic_deleter<Allocator<Resource>>;

            value_type* allocate(size_t n);
            void deallocate(value_type* ptr, size_t n) noexcept;

        private:
            static Resource m_buffer;
    };

    const size_t DefaultBlockSize = 1000u;

    template<class Resource>
    Resource Allocator<Resource>::m_buffer{DefaultBlockSize};

    template<class Resource>
    Allocator<Resource>::value_type* Allocator<Resource>::allocate(size_t n)
    {
        return m_buffer.allocate(n);
    }

    template<class Resource>
    void Allocator<Resource>::deallocate(Allocator::value_type* ptr, size_t n) noexcept
    {
        m_buffer.deallocate(ptr, n);
    }
} // namespace nc::alloc
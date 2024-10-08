#pragma once

#include "ncutility/NcError.h"

#include "Jolt/Jolt.h"
#include "Jolt/Core/TempAllocator.h"

#include <cstdlib>

#ifndef NC_CUSTOM_ALLOC_HOOKS
#define NC_ALLOC_HOOK(ptr, size)
#define NC_REALLOC_HOOK(ptr, size)
#define NC_ALIGNED_ALLOC_HOOK(ptr, size)
#define NC_FREE_HOOK(ptr)
#define NC_ALIGNED_FREE_HOOK(ptr)
#endif

namespace nc::physics
{
inline auto AllocateImpl(size_t size) -> void*
{
    auto ptr = std::malloc(size);
    NC_ALLOC_HOOK(ptr, size);
    return ptr;
}

inline auto ReallocateImpl(void* block, size_t, size_t newSize) -> void*
{
    auto ptr = std::realloc(block, newSize);
    NC_REALLOC_HOOK(ptr, size);
    return ptr;
}

inline auto AlignedAllocateImpl(size_t size, size_t alignment) -> void*
{
#if defined(JPH_PLATFORM_WINDOWS)
    auto ptr = _aligned_malloc(size, alignment);
#else
    void* ptr = nullptr;
    JPH_SUPPRESS_WARNING_PUSH
    JPH_GCC_SUPPRESS_WARNING("-Wunused-result")
    JPH_CLANG_SUPPRESS_WARNING("-Wunused-result")
    posix_memalign(&ptr, alignment, size);
    JPH_SUPPRESS_WARNING_POP
#endif
    NC_ALIGNED_ALLOC_HOOK(ptr, size);
    return ptr;
}

inline void FreeImpl(void* ptr)
{
    NC_FREE_HOOK(ptr);
    std::free(ptr);
}

inline void AlignedFreeImpl(void* ptr)
{
    NC_ALIGNED_FREE_HOOK(ptr);
#if defined(JPH_PLATFORM_WINDOWS)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

inline void RegisterAllocator()
{
    JPH::Allocate = AllocateImpl;
    JPH::Reallocate = ReallocateImpl;
    JPH::Free = FreeImpl;
    JPH::AlignedAllocate = AlignedAllocateImpl;
    JPH::AlignedFree = AlignedFreeImpl;
}

class TempAllocator final : public JPH::TempAllocator
{
    static constexpr auto Alignment = 16ull;

public:
    JPH_OVERRIDE_NEW_DELETE

    explicit TempAllocator(uint32_t size)
        : m_buffer(static_cast<uint8_t*>(AlignedAllocateImpl(size, Alignment))),
          m_size(size)
    {
    }

    ~TempAllocator() noexcept override
    {
        AlignedFreeImpl(m_buffer);
    }

    TempAllocator(const TempAllocator&) = delete;
    TempAllocator(TempAllocator&&) = delete;
    TempAllocator& operator=(const TempAllocator&) = delete;
    TempAllocator& operator=(TempAllocator&&) = delete;

    auto Allocate(uint32_t size) -> void* override
    {
        if (size == 0)
        {
            return nullptr;
        }

        const auto newTop = m_top + JPH::AlignUp(size, Alignment);
        if (newTop > m_size)
        {
            throw NcError("TempAllocator out of memory");
        }

        auto address = m_buffer + m_top;
        m_top = newTop;
        return static_cast<void*>(address);
    }

    void Free(void* address, uint32_t size) override
    {
        if (address == nullptr)
        {
            NC_ASSERT(size == 0, "Attempt to free nullptr with nonzero size");
        }
        else
        {
            m_top -= JPH::AlignUp(size, Alignment);
            NC_ASSERT(m_buffer + m_top == address, "Unexpected free order");
        }
    }

    auto IsEmpty() const -> bool { return m_top == 0; }
    auto GetSize() const -> uint32_t { return m_size; }
    auto GetUsage() const -> uint32_t { return m_top; }
    auto CanAllocate(uint32_t size) const -> bool { return m_top + JPH::AlignUp(size, Alignment) <= m_size; }
    auto OwnsMemory(const void* address) const -> bool { return address >= m_buffer && address < m_buffer + m_size; }

private:
    uint8_t* m_buffer;
    uint32_t m_size;
    uint32_t m_top = 0;
};
} // namespace nc::physics

#pragma once

#include "ncengine/ecs/Component.h"

namespace nc::detail
{
class AnyImplStorage;

struct AnyImplBase
{
    virtual ~AnyImplBase() = default;
    virtual auto Clone(AnyImplStorage& storage) const noexcept -> AnyImplBase* = 0;
    virtual auto MoveTo(AnyImplStorage& storage) noexcept -> AnyImplBase* = 0;
    virtual void DrawUI() = 0;
};

template<PooledComponent T>
class AnyImplConcrete : public AnyImplBase
{
    public:
        explicit AnyImplConcrete(T* instance, ComponentHandler<T>* handler) noexcept
            : m_instance{instance}, m_handler{handler} {}

        auto Clone(AnyImplStorage& storage) const noexcept -> AnyImplBase* override;
        auto MoveTo(AnyImplStorage& storage) noexcept -> AnyImplBase* override;
        void DrawUI() override;

    private:
        T* m_instance;
        ComponentHandler<T>* m_handler;
};

class AnyImplStorage
{
    public:
        constexpr AnyImplStorage() noexcept
            : buffer{} {}

        template<PooledComponent T>
        AnyImplStorage(T* instance, ComponentHandler<T>* handler);

        ~AnyImplStorage() noexcept;
        AnyImplStorage(const AnyImplStorage& other) noexcept;
        AnyImplStorage(AnyImplStorage&& other) noexcept;
        auto operator=(const AnyImplStorage& other) noexcept -> AnyImplStorage&;
        auto operator=(AnyImplStorage&& other) noexcept -> AnyImplStorage&;

        auto operator==(const AnyImplStorage& other) const noexcept
        {
            return std::ranges::equal(buffer, other.buffer);
        }

        auto HasValue() const noexcept -> bool
        {
            return std::ranges::any_of(buffer, [](auto b) { return b != std::byte{}; });
        }

        auto AsImpl() noexcept -> AnyImplBase*
        {
            return reinterpret_cast<AnyImplBase*>(buffer);
        }

        auto AsImpl() const noexcept -> const AnyImplBase*
        {
            return reinterpret_cast<const AnyImplBase*>(buffer);
        }

        auto AsStorage() noexcept -> void*
        {
            return buffer;
        }

    private:
        struct Any{};
        using Placeholder = detail::AnyImplConcrete<Any>;
        static constexpr auto align = alignof(Placeholder);
        static constexpr auto size = sizeof(Placeholder);

        alignas(align) std::byte buffer[size];

        void Clear() noexcept;
};

template<PooledComponent T>
auto AnyImplConcrete<T>::Clone(AnyImplStorage& storage) const noexcept -> AnyImplBase*
{
    return new (storage.AsStorage()) AnyImplConcrete<T>{*this};
}

template<PooledComponent T>
auto AnyImplConcrete<T>::MoveTo(AnyImplStorage& storage) noexcept -> AnyImplBase*
{
    return new (storage.AsStorage()) AnyImplConcrete<T>{std::move(*this)};
}

template<PooledComponent T>
void AnyImplConcrete<T>::DrawUI()
{
    m_handler->drawUI(*m_instance);
}

template<PooledComponent T>
AnyImplStorage::AnyImplStorage(T* instance, ComponentHandler<T>* handler)
{
    NC_ASSERT(instance && handler, "AnyComponent params must not be null, use AnyComponent() instead.")
    using Impl = detail::AnyImplConcrete<T>;

    // Only place we know the derived Impl type - validate all of our assumptions.
    static_assert(sizeof(Impl) == size &&
                  alignof(Impl) == align &&
                  std::has_virtual_destructor_v<Impl> &&
                  std::is_nothrow_destructible_v<Impl> &&
                  std::is_nothrow_copy_constructible_v<Impl> &&
                  std::is_nothrow_move_constructible_v<Impl> &&
                  std::is_nothrow_copy_assignable_v<Impl> &&
                  std::is_nothrow_move_assignable_v<Impl>);

    new (buffer) Impl{instance, handler};
}

inline AnyImplStorage::~AnyImplStorage() noexcept
{
    if (HasValue())
    {
        AsImpl()->~AnyImplBase();
    }
}

inline AnyImplStorage::AnyImplStorage(const AnyImplStorage& other) noexcept
    : buffer{}
{
    if (other.HasValue())
    {
        other.AsImpl()->Clone(*this);
    }
}

inline AnyImplStorage::AnyImplStorage(AnyImplStorage&& other) noexcept
    : buffer{}
{
    if (other.HasValue())
    {
        other.AsImpl()->MoveTo(*this);
    }
}

inline auto AnyImplStorage::operator=(const AnyImplStorage& other) noexcept -> AnyImplStorage&
{
    Clear();
    if (other.HasValue())
    {
        other.AsImpl()->Clone(*this);
    }

    return *this;
}

inline auto AnyImplStorage::operator=(AnyImplStorage&& other) noexcept -> AnyImplStorage&
{
    Clear();
    if (other.HasValue())
    {
        other.AsImpl()->MoveTo(*this);
    }

    return *this;
}

inline void AnyImplStorage::Clear() noexcept
{
    if (HasValue())
    {
        AsImpl()->~AnyImplBase();
        std::memset(buffer, 0, size);
    }
}
} // namespace nc::detail

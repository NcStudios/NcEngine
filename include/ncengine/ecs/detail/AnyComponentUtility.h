#pragma once

#include "ncengine/ecs/Component.h"

#include <string_view>

namespace nc::detail
{
struct AnyImplBase
{
    virtual ~AnyImplBase() = default;
    virtual auto Name() -> std::string_view = 0;
    virtual auto HasDrawUI() const -> bool = 0;
    virtual void DrawUI() = 0;
};

template<PooledComponent T>
class AnyImplConcrete : public AnyImplBase
{
    public:
        explicit AnyImplConcrete(T* instance, ComponentHandler<T>* handler) noexcept
            : m_instance{instance}, m_handler{handler}
        {
        }

        auto Name() -> std::string_view override
        {
            return m_handler->name;
        }

        auto HasDrawUI() const -> bool override
        {
            return m_handler->drawUI != nullptr;
        }

        void DrawUI() override
        {
            if (m_handler->drawUI)
            {
                m_handler->drawUI(*m_instance);
            }
        }

    private:
        T* m_instance;
        ComponentHandler<T>* m_handler;
};
} // namespace nc::detail

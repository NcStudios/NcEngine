#pragma once

#include "ncengine/ecs/Component.h"

namespace nc::detail
{
struct AnyImplBase
{
    virtual ~AnyImplBase() = default;
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

        void DrawUI() override
        {
            m_handler->drawUI(*m_instance);
        }

    private:
        T* m_instance;
        ComponentHandler<T>* m_handler;
};
} // namespace nc::detail

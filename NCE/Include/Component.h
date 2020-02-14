#ifndef COMPONENT_H
#define COMPONENT_H

#include "Common.h"

class Component
{
    public:
        Component(ComponentHandle handle, EntityHandle parentEntityHandle) noexcept : m_handle(handle), m_entityHandle(parentEntityHandle) {}
        virtual ~Component() {}

        ComponentHandle GetHandle() const noexcept { return m_handle; }
        EntityHandle GetEntityHandle() const noexcept { return m_entityHandle; }

        virtual void FrameUpdate() {}
        virtual void FixedUpdate() {}
        virtual void OnInitialize() {}
        virtual void OnDestroy() {}
        virtual void OnCollisionEnter(const EntityHandle other) {}
        virtual void OnCollisionStay() {}
        virtual void OnCollisionExit() {}

    private:
        ComponentHandle m_handle;
        EntityHandle m_entityHandle;
};

#endif
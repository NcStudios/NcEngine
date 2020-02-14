#ifndef COMPONENT
#define COMPONENT

#include "Common.h"
//#include "NCE.h"

class Component
{
    private:
        ComponentHandle m_handle;
        EntityHandle m_entityHandle;
    public:
        Component(ComponentHandle handle, EntityHandle parentEntityHandle) : m_handle(handle), m_entityHandle(parentEntityHandle) {}
        virtual ~Component() {}

        ComponentHandle GetHandle() const { return m_handle; }
        EntityHandle GetEntityHandle() const { return m_entityHandle; }

        virtual void FrameUpdate() {}
        virtual void FixedUpdate() {}
        virtual void OnInitialize() {}
        virtual void OnDestroy() {}
        virtual void OnCollisionEnter(const EntityHandle other) {}
        virtual void OnCollisionStay() {}
        virtual void OnCollisionExit() {}
};

#endif
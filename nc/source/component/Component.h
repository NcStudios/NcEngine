#pragma once
#include "EntityHandle.h"

namespace nc
{
    class ComponentBase
    {
        public:
            ComponentBase(EntityHandle handle) noexcept
                : m_parentHandle{handle} {}

            virtual ~ComponentBase() = default;
            ComponentBase(const ComponentBase&) = delete;
            ComponentBase(ComponentBase&&) = delete;
            ComponentBase& operator=(const ComponentBase&) = delete;
            ComponentBase& operator=(ComponentBase&&) = delete;

            EntityHandle GetParentHandle() noexcept { return m_parentHandle; }

            #ifdef NC_EDITOR_ENABLED
            virtual void EditorGuiElement();
            #endif

        protected:
            EntityHandle m_parentHandle;
    };

    class Component : public ComponentBase
    {
        public:
            Component(EntityHandle handle) noexcept
                : ComponentBase{handle} {}

            virtual void FrameUpdate(float) {}
            virtual void FixedUpdate() {}
            virtual void OnDestroy() {}
            virtual void OnCollisionEnter(EntityHandle) {}
            virtual void OnCollisionStay() {};
            virtual void OnCollisionExit() {};
    };
} //end namespace nc
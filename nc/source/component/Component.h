#pragma once
#include "NcCommonTypes.h"

namespace nc
{
    class Component
    {
        public:
            Component(ComponentHandle handle, EntityHandle parentHandle) noexcept
                : m_handle{handle}, m_parentHandle{parentHandle}
            {}
            virtual ~Component() = default;
            Component(const Component&) = delete;
            Component(Component&&) = delete;
            Component& operator=(const Component&) = delete;
            Component& operator=(Component&&) = delete;

            ComponentHandle GetHandle() const noexcept { return m_handle; }
            EntityHandle GetParentHandle() noexcept { return m_parentHandle; }

            virtual void FrameUpdate(float dt) { (void) dt; }
            virtual void FixedUpdate() {}
            virtual void OnDestroy() {}
            virtual void OnCollisionEnter(EntityHandle other) { (void)other; }
            virtual void OnCollisionStay() {};
            virtual void OnCollisionExit() {};
        
            #ifdef NC_EDITOR_ENABLED
            virtual void EditorGuiElement();
            #endif

        protected:
            ComponentHandle m_handle = NullHandle;
            EntityHandle m_parentHandle = NullHandle;
    };
} //end namespace nc
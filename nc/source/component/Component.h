#pragma once
#include "NcCommonTypes.h"

namespace nc
{
    class Component
    {
        public:
            Component(EntityHandle handle) noexcept
                : m_parentHandle{handle}
            {}
            virtual ~Component() = default;
            Component(const Component&) = delete;
            Component(Component&&) = delete;
            Component& operator=(const Component&) = delete;
            Component& operator=(Component&&) = delete;

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
            EntityHandle m_parentHandle;
    };
} //end namespace nc
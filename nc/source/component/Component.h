#pragma once
#include "NcCommonTypes.h"

namespace nc
{
    namespace engine::alloc { template<class T> class Pool; }

    class Component
    {
        public:
            Component() = default;
            Component(const Component&) = delete;
            virtual ~Component() = default;
            Component(Component&&) = default;
            Component& operator=(const Component&) = delete;
            Component& operator=(Component&&) = default;
            Component(ComponentHandle handle, EntityHandle parentHandle) noexcept
                : m_handle{handle}, m_parentHandle{parentHandle}, m_memoryState{MemoryState::Valid}
            {
            }

            ComponentHandle GetHandle() const noexcept { return m_handle; }
            EntityHandle GetParentHandle() noexcept { return m_parentHandle; }

            // MemoryState GetMemoryState() const { return m_memoryState; }
            // void SetMemoryState(MemoryState state) { m_memoryState = state; }

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
        
        private:
            template<class T> friend class engine::alloc::Pool;
            MemoryState m_memoryState = MemoryState::Invalid;
    };
} //end namespace nc
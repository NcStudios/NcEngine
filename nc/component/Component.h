#pragma once
#include "Common.h"
#include <string>

namespace nc
{
    namespace engine::alloc { template<class T> class Block; }

    class Component
    {

        template<class T>
        friend class engine::alloc::Block;

        public:
            Component() = default;
            Component(const Component&) = default;
            Component(Component&&) = default;
            Component& operator=(const Component&) = default;
            Component& operator=(Component&&) = default;
            virtual ~Component() = default;

            /**
             * Called by owning Entity after construction.
             */
            void Register(const ComponentHandle componentHandle, const EntityHandle parentHandle) noexcept;

            ComponentHandle GetHandle() const noexcept;
            EntityHandle GetParentHandle() noexcept;

            MemoryState GetMemoryState() const;
            void SetMemoryState(const MemoryState state);

            virtual void FrameUpdate(float dt);
            virtual void FixedUpdate();
            virtual void OnDestroy();
            virtual void OnCollisionEnter(const EntityHandle other);
            virtual void OnCollisionStay();
            virtual void OnCollisionExit();
        
        protected:
            ComponentHandle m_handle = NullHandle;
            EntityHandle m_parentHandle = NullHandle;
            MemoryState m_MemoryState;

#ifdef NC_EDITOR_ENABLED
        public:
            virtual void EditorGuiElement();
#endif
    };

} //end namespace nc
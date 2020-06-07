#pragma once
#include "Common.h"
#include "views/EntityView.h"
#include <string>

namespace nc
{
    class Component
    {
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
            void Initialize(const ComponentHandle componentHandle, const EntityView parentView) noexcept;

            ComponentHandle GetHandle() const noexcept;
            EntityView* GetParentView() noexcept;

            virtual void FrameUpdate(float dt);
            virtual void FixedUpdate();
            virtual void OnInitialize();
            virtual void OnDestroy();
            virtual void OnCollisionEnter(const EntityHandle other);
            virtual void OnCollisionStay();
            virtual void OnCollisionExit();
        
        protected:
            ComponentHandle m_handle;
            EntityView m_parentView;          

#ifdef NC_DEBUG
        public:
            virtual void EditorGuiElement();
#endif
    };

} //end namespace nc
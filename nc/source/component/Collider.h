#pragma once

#include "component/Component.h"
#include "component/Transform.h"
#include "math/Vector3.h"
#include "graphics/Model.h"

namespace nc
{
    struct ColliderData
    {
        Transform* transform;
        Vector3 scale;
        graphics::Model model;
    };

    class ColliderBase : public Component
    {
        public:
            ColliderBase(ComponentHandle handle, EntityHandle parentHandle, Vector3 scale, graphics::Model model);
            void Update(graphics::FrameManager& frame);
            DirectX::FXMMATRIX GetTransformationMatrix() const;

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif

        protected:
            ColliderData m_data;
    };

    class BoxCollider : public ColliderBase
    {
        public:
            BoxCollider(ComponentHandle handle, EntityHandle parentHandle, Vector3 scale);
            ~BoxCollider();
    };

    class SphereCollider : public ColliderBase
    {
        public:
            SphereCollider(ComponentHandle handle, EntityHandle parentHandle, Vector3 scale);
            ~SphereCollider();
    };
}
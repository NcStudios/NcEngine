#pragma once

#include "component/Component.h"
#include "math/Vector3.h"
#include "graphics/Model.h"

namespace nc
{
    enum class ColliderType : uint8_t
    {
        Box = 0u, Sphere = 1u
    };

    class Collider final : public ComponentBase
    {
        public:
            Collider(EntityHandle handle, ColliderType type, Vector3 scale);
            ~Collider();

            ColliderType GetType() const;
            DirectX::FXMMATRIX GetTransformationMatrix() const;
            void UpdateTransformationMatrix();

            #ifdef NC_EDITOR_ENABLED
            void UpdateWidget(graphics::FrameManager& frame);
            void EditorGuiElement() override;
            void SetEditorSelection(bool state);
            #endif

        protected:
            DirectX::XMMATRIX m_colliderMatrix;
            DirectX::FXMMATRIX m_transformMatrix;
            Vector3 m_scale;
            const ColliderType m_type;

            #ifdef NC_EDITOR_ENABLED
            graphics::Model m_widgetModel;
            bool m_selectedInEditor;
            #endif
    };
}
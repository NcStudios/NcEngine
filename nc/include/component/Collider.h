#pragma once

#include "component/Component.h"
#include "math/Vector3.h"
#include "graphics/Model.h"

namespace nc
{
    class Collider final : public ComponentBase
    {
        public:
            Collider(EntityHandle handle, Vector3 scale);
            ~Collider();

            void UpdateTransformationMatrix();
            DirectX::FXMMATRIX GetTransformationMatrix() const;

            #ifdef NC_EDITOR_ENABLED
            void UpdateModel(graphics::FrameManager& frame);
            void EditorGuiElement() override;
            void SetEditorSelection(bool state);
            #endif

        protected:
            DirectX::XMMATRIX m_colliderMatrix;
            DirectX::FXMMATRIX m_transformMatrix;
            Vector3 m_scale;

            #ifdef NC_EDITOR_ENABLED
            graphics::Model m_model;
            bool m_selectedInEditor;
            #endif
    };
}
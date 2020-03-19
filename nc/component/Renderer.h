#pragma once
#include <memory>
#include "Common.h"
#include "Component.h"
#include "Model.h"
#include "DirectXMath.h"
#include "GraphicsResourcePipeline.h"


namespace nc
{
    class Renderer : public Component
    {
        public:
            Renderer(ComponentHandle handle, EntityView parentView);
            Renderer(const Renderer&) = delete;
            Renderer(Renderer&&);
            Renderer& operator=(const Renderer&) = delete;
            Renderer& operator=(Renderer&&);

            void EditorGuiElement() override;
            void SyncMaterialData(graphics::Graphics&);

            void SetModel(graphics::Graphics& graphics, graphics::Mesh& mesh, DirectX::XMFLOAT3& materialColor);

            void Update(graphics::Graphics& graphics);

        private:
            std::unique_ptr<graphics::Model> m_model;
    };
    
}
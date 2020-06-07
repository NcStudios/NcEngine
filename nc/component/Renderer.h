#pragma once
#include "Component.h"
#include "graphics/DirectXMath/Inc/DirectXMath.h"

#include <memory>

namespace nc::graphics { class Model; class Mesh; class Graphics; }

namespace nc
{
    class Renderer : public Component
    {
        public:
            Renderer();
            Renderer(const Renderer&) = delete;
            Renderer(Renderer&&);
            Renderer& operator=(const Renderer&) = delete;
            Renderer& operator=(Renderer&&);

            #ifdef NC_DEBUG
            void EditorGuiElement() override;
            void SyncMaterialData();
            #endif

            void SetModel(graphics::Mesh& mesh, DirectX::XMFLOAT3& materialColor);

            void Update();

        private:
            std::unique_ptr<graphics::Model> m_model;
    };
    
}
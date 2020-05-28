#pragma once
#include <memory>
#include "Common.h"
#include "Component.h"
#include "DirectXMath.h"

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

            void SetModel(graphics::Graphics& graphics, graphics::Mesh& mesh, DirectX::XMFLOAT3& materialColor);

            void Update(graphics::Graphics& graphics);

        private:
            std::unique_ptr<graphics::Model> m_model;
    };
    
}
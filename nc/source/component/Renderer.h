#pragma once

#include <memory>
#include "Component.h"
#include "directx/math/DirectXMath.h"

#include <memory>

namespace nc::graphics { class Model; class Mesh; class Material; class Graphics; class PBRMaterial;}

namespace nc
{
    class Transform;

    class Renderer : public Component
    {
        public:
            Renderer();
            Renderer(graphics::Mesh& mesh, graphics::PBRMaterial& material);
            Renderer(const Renderer&) = delete;
            Renderer(Renderer&&);
            Renderer& operator=(const Renderer&) = delete;
            Renderer& operator=(Renderer&&);
            ~Renderer() = default;

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            void SyncMaterialData();
            #endif

            void SetMesh(graphics::Mesh& mesh);
            void SetMaterial(graphics::PBRMaterial& material);
            void Update(graphics::Graphics * gfx);

        private:
            std::unique_ptr<graphics::Model> m_model;
            Transform * m_transform;
    };
    
}
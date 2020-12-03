#pragma once

#include "Component.h"
#include "graphics/Mesh.h"
#include "graphics/PBRMaterial.h"

namespace nc::graphics
{
    class Graphics;
    class Model;
}

namespace nc
{
    class Transform;

    class Renderer final: public Component
    {
        public:
            Renderer();
            Renderer(graphics::Mesh& mesh, graphics::PBRMaterial& material);
            Renderer(const Renderer&) = delete;
            Renderer(Renderer&&);
            Renderer& operator=(const Renderer&) = delete;
            Renderer& operator=(Renderer&&);
            ~Renderer();

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
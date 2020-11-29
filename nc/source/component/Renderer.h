#pragma once

#include "Component.h"
#include "graphics/Mesh.h"
#include "graphics/materials/Material.h"

namespace nc::graphics
{
    class Graphics;
    class Model;
}

namespace nc
{
    class Transform;

    class Renderer : public Component
    {
        public:
            Renderer();
            Renderer(graphics::Mesh& mesh, graphics::Material& material);
            Renderer(const Renderer&) = delete;
            Renderer(Renderer&&);
            Renderer& operator=(const Renderer&) = delete;
            Renderer& operator=(Renderer&&);
            ~Renderer();

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif

            void SetMesh(graphics::Mesh& mesh);
            void SetMaterial(graphics::Material& material);
            void Update(graphics::FrameManager& frame);

        private:
            std::unique_ptr<graphics::Model> m_model;
            Transform * m_transform;
    };
    
}
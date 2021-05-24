#pragma once

#include "Component.h"
#include "graphics/Mesh.h"
#include "graphics/Material.h"

namespace nc::graphics
{
    class Graphics;
    class Model;
}

namespace nc
{
    class Transform;

    class Renderer final: public ComponentBase
    {
        public:
            Renderer(EntityHandle handle, graphics::Mesh mesh, graphics::Material material) noexcept;
            ~Renderer() noexcept;
            Renderer(const Renderer&) = delete;
            Renderer(Renderer&&) = default;
            Renderer& operator=(const Renderer&) = delete;
            Renderer& operator=(Renderer&&) = default;

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif

            void SetMesh(const graphics::Mesh& mesh);
            void SetMaterial(const graphics::Material& material);
            void Update(graphics::FrameManager* frame);

        private:
            std::unique_ptr<graphics::Model> m_model;
            //Transform * m_transform;
    };
    
}
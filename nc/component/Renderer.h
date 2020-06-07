#pragma once

#include <memory>
#include "Common.h"
#include "graphics/Mesh.h"
#include "graphics/Material.h"
#include "Component.h"
#include "graphics/DirectXMath/Inc/DirectXMath.h"

#include <memory>

namespace nc::graphics { class Model; class Mesh; class Graphics; }

namespace nc
{
    class Renderer : public Component
    {
        public:
            Renderer(graphics::Graphics * graphics, graphics::Mesh& mesh); // TODO: Remove req to pass in graphics
            Renderer(const Renderer&) = delete;
            Renderer(Renderer&&);
            Renderer& operator=(const Renderer&) = delete;
            Renderer& operator=(Renderer&&);

            #ifdef NC_DEBUG
            void EditorGuiElement() override;
            void SyncMaterialData();
            #endif

            void SetMesh(graphics::Mesh& mesh);
            void SetMaterial(graphics::Material& material);
            void Update(graphics::Graphics * graphics);

        private:
            std::unique_ptr<graphics::Model> m_model;
    };
    
}
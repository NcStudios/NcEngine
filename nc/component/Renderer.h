#pragma once

#include <memory>
#include "Common.h"
#include "Mesh.h"
#include "Material.h"
#include "Component.h"
#include "graphics/DirectXMath/Inc/DirectXMath.h"

#include <memory>

namespace nc::graphics { class Model; class Mesh; class Graphics; }

namespace nc
{
    class Renderer : public Component
    {
        public:
            Renderer(graphics::Mesh& mesh); // TODO: enforce model has mesh
            Renderer(const Renderer&) = delete;
            Renderer(Renderer&&);
            Renderer& operator=(const Renderer&) = delete;
            Renderer& operator=(Renderer&&);

            #ifdef NC_DEBUG
            void EditorGuiElement() override;
            void SyncMaterialData();
            #endif

            void SetMesh(graphics::Graphics * graphics, graphics::Mesh& mesh);
            void SetMaterial(graphics::Graphics * graphics, graphics::Material&);
            void Update(graphics::Graphics * graphics);

        private:
            std::unique_ptr<graphics::Model> m_model;
    };
    
}
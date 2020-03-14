#pragma once

#include "Common.h"
#include "Component.h"


#include "Model.h"
#include "Mesh.h"
#include "MaterialType.h"

#include "DirectXMath.h"
#include "Drawable.h"
#include "GraphicsResourcePipeline.h"
#include <memory>
//#include "NCE.h"


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

            template<typename ModelType>
            void SetModel(graphics::Graphics& graphics, DirectX::XMFLOAT3 materialColor);

            void Update(graphics::Graphics& graphics, float dt);

            void SpawnControlWindow(int id, graphics::Graphics& graphics);
            void SyncMaterialData(graphics::Graphics& graphics);

        private:
            std::unique_ptr<nc::graphics::d3dresource::Drawable> m_model;
    };


    template<typename MeshType>
    void Renderer::SetModel(graphics::Graphics& graphics, DirectX::XMFLOAT3 materialColor)
    {
        m_model = std::make_unique<graphics::Model<MeshType>>(graphics, materialColor);
    }
}
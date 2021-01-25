#include "Renderer.h"
#include "graphics/Graphics.h"
#include "graphics/Model.h"
#include "graphics/d3dresource/ConstantBufferResources.h"
#include "graphics/rendergraph/FrameManager.h"
#include "Ecs.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    Renderer::~Renderer() = default;

    Renderer::Renderer(EntityHandle handle, graphics::Mesh mesh, graphics::Material material) noexcept
        : ComponentBase(handle),
          m_model{ std::make_unique<graphics::Model>(std::move(mesh), std::move(material)) },
          m_transform{ Ecs::GetComponent<Transform>(handle) }
    {
    }

    #ifdef NC_EDITOR_ENABLED
    void Renderer::EditorGuiElement()
    {
        ImGui::Text("Renderer");
        m_model->GetMaterial()->EditorGuiElement();
    }
    #endif
    
    void Renderer::Update(graphics::FrameManager& frame)
    {
        IF_THROW(!m_transform, "Renderer::Update - Bad Transform Ptr");
        m_model->SetTransformationMatrix(m_transform->GetTransformationMatrix());
        m_model->Submit(frame);
    }

    void Renderer::SetMesh(const graphics::Mesh& mesh)
    {
        m_model->SetMesh(mesh);
    }
    
    void Renderer::SetMaterial(const graphics::Material& material) 
    {
        m_model->SetMaterial(material);
    }

}
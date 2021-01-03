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

    Renderer::Renderer(ComponentHandle handle, EntityHandle parentHandle, graphics::Mesh& mesh, graphics::Material& material) noexcept
        : Component(handle, parentHandle),
          m_model{ std::make_unique<graphics::Model>(mesh, material) },
          m_transform{ Ecs::GetComponent<Transform>(m_parentHandle) }
    {
    }

    #ifdef NC_EDITOR_ENABLED
    void Renderer::EditorGuiElement()
    {
        ImGui::PushItemWidth(80.0f);
            ImGui::Spacing();
            ImGui::Separator();
                ImGui::Text("Renderer");
                m_model->GetMaterial()->EditorGuiElement();
            ImGui::Separator();
        ImGui::PopItemWidth();
    }
    #endif
    
    void Renderer::Update(graphics::FrameManager& frame)
    {
        IF_THROW(!m_transform, "Renderer::Update - Bad Transform Ptr");
        m_model->UpdateTransformationMatrix(m_transform);
        m_model->Submit(frame);
    }

    void Renderer::SetMesh(graphics::Mesh& mesh)
    {
        m_model->SetMesh(mesh);
    }
    
    void Renderer::SetMaterial(graphics::Material& material) 
    {
        m_model->SetMaterial(material);
    }

}
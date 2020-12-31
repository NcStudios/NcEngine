#include "Renderer.h"
#include "graphics/Graphics.h"
#include "graphics/Model.h"
#include "graphics/d3dresource/GraphicsResource.h"
#include "graphics/d3dresource/ConstantBufferResources.h"
#include "graphics/rendergraph/FrameManager.h"
#include "Ecs.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    Renderer::Renderer()
        : m_model{ nullptr },
          m_transform{ nullptr }
    {
    }
    
    Renderer::Renderer(graphics::Mesh& mesh, graphics::Material& material)
        : m_model{ std::make_unique<graphics::Model>(mesh, material) },
          m_transform{ nullptr }
    {
    }

    Renderer::Renderer(Renderer&& other)
    {
        m_handle = other.GetHandle();
        m_parentHandle = other.GetParentHandle();
        m_model = std::move(other.m_model);
    }

    Renderer::~Renderer()
    {
    }

    Renderer& Renderer::operator=(Renderer&& other)
    {
        m_handle = other.GetHandle();
        m_parentHandle = other.GetParentHandle();
        m_model = std::move(other.m_model);
        return *this;
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
        if (!m_transform)
        {
            m_transform = Ecs::GetComponent<Transform>(m_parentHandle);
        }

        if (!m_transform)
        {
            throw std::runtime_error("Renderer::Update - bad trans ptr");
        }

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
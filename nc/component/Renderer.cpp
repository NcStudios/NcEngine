#include "Renderer.h"
#include "Graphics.h"
#include "Model.h"
#include "EditorManager.h"
#include "NCE.h"

namespace nc
{
    Renderer::Renderer(ComponentHandle handle, EntityView parentView)
        : Component(handle, parentView), m_model(nullptr)
    {}

    Renderer::Renderer(Renderer&& other)
        : Component(other.GetHandle(), *View<EntityView>(&other))
    {
        m_model = std::move(other.m_model);
    }

    Renderer& Renderer::operator=(Renderer&& other)
    {
        m_model = std::move(other.m_model);
        return *this;
    }

    void Renderer::EditorGuiElement()
    {
        std::string str = std::to_string(GetHandle());

        auto mat = m_model->GetMaterial();

        ImGui::PushItemWidth(40.0f);
            ImGui::Spacing();
            ImGui::Separator();
                ImGui::Text("Renderer");
                ImGui::Indent();
                    ImGui::Text("ID: ");  ImGui::SameLine();    ImGui::Text(str.c_str());
                    ImGui::Text("Material Color");  ImGui::SameLine();  bool mcDirty = ImGui::ColorEdit3("##mc", &(mat->color.x), ImGuiColorEditFlags_NoInputs);
                    ImGui::Text("Specular");
                    ImGui::Indent();
                    ImGui::Text("Intensity");  ImGui::SameLine();  bool siDirty = ImGui::DragFloat("##si",  &(mat->specularIntensity), 0.75f, 0.05f,   4.0f, "%.2f", 2);
                    ImGui::Text("Power    ");  ImGui::SameLine();  bool spDirty = ImGui::DragFloat("##sp",  &(mat->specularPower),     0.75f,  1.0f, 200.0f, "%.2f", 2);
                    ImGui::Unindent();
                ImGui::Unindent();
            ImGui::Separator();
        ImGui::PopItemWidth();

        if(mcDirty || siDirty || spDirty)
        {
            //SyncMaterialData();
        }
    }

    void Renderer::SyncMaterialData(graphics::Graphics& graphics)
    {
        if(!m_model) return;

        using namespace nc::graphics;
        //auto pConstPS = m_model->QueryBindable<d3dresource::PixelConstantBuffer<PSMaterialConstants>>();
	    //assert(pConstPS != nullptr);
	    //pConstPS->Update(graphics, m_model->m_materialData);
    }

    void Renderer::Update(graphics::Graphics& graphics)
    {
        m_model->UpdateTransformationMatrix(View<Transform>(this));
        m_model->Draw(graphics);
    }

    void Renderer::SetModel(graphics::Graphics& graphics, graphics::Mesh& mesh, DirectX::XMFLOAT3& materialColor)
    {
        m_model = std::make_unique<graphics::Model>(graphics, mesh, materialColor);
    }
    

}
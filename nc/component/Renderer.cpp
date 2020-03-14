#include "Renderer.h"

#include "EditorManager.h"

#include "NCE.h"

namespace nc
{
    Renderer::Renderer(ComponentHandle handle, EntityView parentView)
        : Component(handle, parentView), m_model(nullptr)
    {}

    Renderer::Renderer(Renderer&& other)
        //: Component(other.GetHandle(), *other.View<EntityView>())
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

        ImGui::PushItemWidth(60.0f);
            ImGui::Spacing();
            ImGui::Separator();
                ImGui::Text("Renderer");
                ImGui::Indent();
                    ImGui::Text("ID: ");  ImGui::SameLine();    ImGui::Text(str.c_str());
                    ImGui::PushItemWidth(120.0f);
                    ImGui::Text("Material Color");      ImGui::SameLine();  bool mcDirty = ImGui::ColorEdit3("##mc", &(mat->color.x));
                    ImGui::Text("Specular Intensity");  ImGui::SameLine();  bool siDirty = ImGui::DragFloat("##si", &(mat->specularIntensity), 0.75f, 0.05f, 4.0f, "%.2f", 2);
                    ImGui::Text("Specular Power");      ImGui::SameLine();  bool spDirty = ImGui::DragFloat("##sp", &(mat->specularPower), 0.75f, 1.0f, 200.0f, "%.2f", 2);
                    ImGui::PopItemWidth();
                ImGui::Unindent();
            ImGui::Separator();
        ImGui::PopItemWidth();

        if(mcDirty || siDirty || spDirty)
        {
            //SyncMaterialData();
        }
    }

    void Renderer::Update(graphics::Graphics& graphics, float dt)
    {
        m_model->UpdateTransformationMatrix(View<Transform>(this));
        m_model->Draw(graphics);
    }

    void Renderer::SpawnControlWindow(int id, graphics::Graphics& graphics)
    {
        if(!m_model) return;
        
        auto material = m_model->GetMaterial();

        bool needSync = utils::editor::EditorManager::BoxControl(id, &material->color.x, &material->specularIntensity, &material->specularPower);

        if(needSync) SyncMaterialData(graphics);
    }

    void Renderer::SyncMaterialData(graphics::Graphics& graphics)
    {
        if(!m_model) return;

        //using namespace nc::graphics;
        //auto pConstPS = m_model->QueryBindable<d3dresource::PixelConstantBuffer<Model<BaseModelType>::PSMaterialConstants>>();
	    //assert(pConstPS != nullptr);
	    //pConstPS->Update(graphics, m_model->m_materialData);
    }

}
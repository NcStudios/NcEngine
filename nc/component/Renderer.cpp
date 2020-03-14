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
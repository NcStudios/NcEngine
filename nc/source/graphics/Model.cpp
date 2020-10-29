#include "Model.h"
#include "PBRMaterial.h"
#include "Graphics.h"
#include "component/Transform.h"
#include "d3dresource/GraphicsResourceManager.h"
#include "ResourceGroup.h"
#include <time.h>
#include <string>

namespace nc::graphics
{
    Model::Model()
    {
    }

    Model::Model(const Mesh& mesh, const PBRMaterial& material)
        : m_mesh(mesh), m_material(material)
    {
        InitializeGraphicsPipeline();
    }

    void Model::InitializeGraphicsPipeline() 
    {
        using namespace nc::graphics::d3dresource;
        auto bufferId = std::to_string(GraphicsResourceManager::AssignId());
        AddGraphicsResource(TransformConstBufferVertexPixel::AcquireUnique(bufferId, *this, 0u, 2u));
        m_indexBuffer = m_mesh.QueryGraphicsResource<d3dresource::IndexBuffer>();
    }

    void Model::SetMaterial(const PBRMaterial& material) noexcept
    {
        m_material = material;
        // TODO: Set material
    }
    
    void Model::SetMesh(const Mesh& mesh) noexcept
    {
        m_mesh = mesh;
    }

    void Model::Draw(Graphics* gfx) const noexcept
    {
        m_material.BindGraphicsResources();
        m_mesh.BindGraphicsResources();

        for(auto& res : m_resources)
        {
            res->Bind();
        }
        gfx->DrawIndexed(m_indexBuffer->GetCount());
    }

    void Model::UpdateTransformationMatrix(Transform* transform) noexcept
    {
        m_transformationMatrix = transform->GetMatrixXM();
    }

    DirectX::XMMATRIX Model::GetTransformXM() const noexcept
    {
        return m_transformationMatrix;
    }

    PBRMaterial* Model::GetMaterial() noexcept
    {
        return &m_material;
    }

    void Model::AddGraphicsResource(std::shared_ptr<d3dresource::GraphicsResource> res)
    {
        if(typeid(*res) == typeid(d3dresource::IndexBuffer))
        {
            assert("Binding multiple index buffers is not allowed" && m_indexBuffer == nullptr);
            m_indexBuffer = &static_cast<d3dresource::IndexBuffer&>(*res);
        }
        m_resources.push_back(std::move(res));
    }
}
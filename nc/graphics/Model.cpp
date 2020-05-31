#include "Model.h"
#include "ObjLoader.h"
#include "NCVector.h"
#include "Transform.h"
#include "Graphics.h"

#include "GraphicsResourceManager.h"

namespace nc::graphics
{
    Model::Model(Graphics * graphics, Mesh& mesh, DirectX::XMFLOAT3& materialColor)
    {
        using namespace nc::graphics::d3dresource;

        m_material.color = materialColor;
        m_mesh = mesh;
        utils::ObjLoader loader;
        loader.Load(&m_mesh);

        auto pvs = GraphicsResourceManager::Acquire<VertexShader>(graphics, m_mesh.VertexShaderPath);
        auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
        AddGraphicsResource(std::move(pvs));
        AddGraphicsResource(GraphicsResourceManager::Acquire<PixelShader> (graphics, m_mesh.PixelShaderPath));

        AddGraphicsResource(GraphicsResourceManager::Acquire<VertexBuffer>(graphics, m_mesh.Vertices, m_mesh.MeshPath));
        AddGraphicsResource(GraphicsResourceManager::Acquire<IndexBuffer> (graphics, m_mesh.Indices, m_mesh.MeshPath));
        AddGraphicsResource(GraphicsResourceManager::Acquire<InputLayout> (graphics, m_mesh.MeshPath, m_mesh.InputElementDesc, pvsbc));
        AddGraphicsResource(GraphicsResourceManager::Acquire<Topology>    (graphics, m_mesh.PrimitiveTopology));

        AddGraphicsResource(TransformCbuf::AcquireUnique(graphics, m_mesh.MeshPath, *this, 0u));
        AddGraphicsResource(PixelConstantBuffer<Material>::AcquireUnique(graphics, m_material, 1u));
    }

    void Model::Draw(Graphics * graphics) const noexcept
    {
        for(auto& res : m_resources)
        {
            res->Bind(graphics);
        }
        graphics->DrawIndexed(m_indexBuffer->GetCount());
    }

    void Model::UpdateTransformationMatrix(Transform* transform) noexcept
    {
        m_transformationMatrix = transform->GetMatrixXM();
    }

    DirectX::XMMATRIX Model::GetTransformXM() const noexcept
    {
        return m_transformationMatrix;
    }

    Material* Model::GetMaterial() noexcept
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
#include "Model.h"
#include "ObjLoader.h"
#include "NCVector.h"
#include "Transform.h"
#include "GraphicsResourcePipeline.h"
#include "IndexBuffer.h"
#include "Transform.h"

namespace nc::graphics
{
    Model::Model(Graphics& graphics, Mesh& mesh, DirectX::XMFLOAT3& materialColor)
    {
        using namespace nc::graphics::d3dresource;

        m_mesh = mesh;
        utils::ObjLoader loader;
        loader.Load(&m_mesh);

        m_material.color = materialColor;
        
        std::cout << "mesh path: " << m_mesh.MeshPath << std::endl;

        AddGraphicsResource(VertexBuffer::Aquire(graphics, m_mesh.Vertices, m_mesh.MeshPath));

        std::cout << "after add vertex buffer" << std:: endl;

        std::cout << "Model.cpp" << std::endl;
        std::cout << "indices size: " << m_mesh.Indices.size() << std::endl;
        if(m_mesh.Indices.data() == nullptr) { std::cout << "nullptr" << std::endl; }


        auto pvs = VertexShader::Aquire(graphics, m_mesh.VertexShaderPath);
        auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
        AddGraphicsResource(std::move(pvs));
        AddGraphicsResource(PixelShader::Aquire(graphics, m_mesh.PixelShaderPath));
        AddGraphicsResource(IndexBuffer::Aquire(graphics, m_mesh.Indices, m_mesh.MeshPath));
        AddGraphicsResource(InputLayout::Aquire(graphics, m_mesh.MeshPath, m_mesh.InputElementDesc, pvsbc));
        AddGraphicsResource(Topology::Aquire(graphics, m_mesh.PrimitiveTopology));
        AddGraphicsResource(TransformCbuf::AquireUnique(graphics, m_mesh.MeshPath, *this, 0u));
        AddGraphicsResource(PixelConstantBuffer<Material>::AquireUnique(graphics, m_material, 1u));
    }

    void Model::Draw(Graphics& graphics) const noexcept
    {
        for(auto& res : m_resources)
        {
            res->Bind(graphics);
        }
        graphics.DrawIndexed(m_indexBuffer->GetCount());
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
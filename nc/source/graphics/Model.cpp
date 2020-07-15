#include "Model.h"
#include "PBRMaterial.h"
#include "Graphics.h"
#include "component/Transform.h"
#include "d3dresource/GraphicsResourceManager.h"

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

        // Create and bind the texture data.
        uint32_t shaderIndex = 0;
        for (const auto& texturePathRef : m_material.GetTexturePaths()) 
        {
            AddGraphicsResource(GraphicsResourceManager::Acquire<Texture>(texturePathRef, shaderIndex++));
        }

        // Create and bind the Vertex Shader
        auto pvs = GraphicsResourceManager::Acquire<VertexShader>(m_material.GetVertexShaderPath());
        auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
        AddGraphicsResource(std::move(pvs));

        // Create and bind the Pixel Shader
        AddGraphicsResource(GraphicsResourceManager::Acquire<PixelShader>(m_material.GetPixelShaderPath()));

        // Create and bind the texture sample.
        AddGraphicsResource(GraphicsResourceManager::Acquire<Sampler>("PBRSampler"));

        // Create and bind the Vertex Buffer, Index buffer, Input Layout and Topology from mesh
        MeshData meshData = m_mesh.GetMeshData();
        AddGraphicsResource(GraphicsResourceManager::Acquire<VertexBuffer>(meshData.Vertices, meshData.MeshPath));
        AddGraphicsResource(GraphicsResourceManager::Acquire<IndexBuffer> (meshData.Indices, meshData.MeshPath));
        AddGraphicsResource(GraphicsResourceManager::Acquire<InputLayout> (meshData.MeshPath, meshData.InputElementDesc, pvsbc));
        AddGraphicsResource(GraphicsResourceManager::Acquire<Topology>    (meshData.PrimitiveTopology));
        AddGraphicsResource(TransformConstBufferVertexPixel::AcquireUnique(meshData.MeshPath, *this, 0u, 2u));
        AddGraphicsResource(PixelConstantBuffer<MaterialProperties>::AcquireUnique(m_material.properties, 1u));
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
#include "Model.h"
#include "NCE.h"
#include "math/NCVector.h"
#include "component/Transform.h"
#include "Graphics.h"
#include "d3dresource/GraphicsResourceManager.h"
#include "PBRMaterial.h"
#include "Material.h"


namespace nc::graphics
{
    Model::Model()
    {
    }

    Model::Model(const Mesh& mesh)
        : Model(mesh, PBRMaterial{})
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
        using namespace nc::graphics;

        AddGraphicsResource(GraphicsResourceManager::Acquire<Texture>("D:\\NC\\NCEngine\\nc\\graphics\\DefaultTexture.png"));
        
        auto pvs = GraphicsResourceManager::Acquire<VertexShader>("project\\shaders\\compiled\\pbrvertexshader.cso");
        auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
        AddGraphicsResource(std::move(pvs));

        AddGraphicsResource(GraphicsResourceManager::Acquire<PixelShader>("project\\shaders\\compiled\\pbrpixelshader.cso"));

        MeshData meshData = m_mesh.GetMeshData();
        AddGraphicsResource(GraphicsResourceManager::Acquire<VertexBuffer>(meshData.Vertices, meshData.MeshPath));
        AddGraphicsResource(GraphicsResourceManager::Acquire<IndexBuffer> (meshData.Indices, meshData.MeshPath));
        AddGraphicsResource(GraphicsResourceManager::Acquire<InputLayout> (meshData.MeshPath, meshData.InputElementDesc, pvsbc));
        AddGraphicsResource(GraphicsResourceManager::Acquire<Topology>    (meshData.PrimitiveTopology));
        AddGraphicsResource(TransformConstBuffer::AcquireUnique(meshData.MeshPath, *this, 0u));
    }

    void Model::SetMaterial(const PBRMaterial& material) noexcept
    {
        m_material = material;

        // using namespace nc::graphics;
        // auto pConstPS = this->QueryGraphicsResource<d3dresource::PixelConstBuffer<PBRMaterial>>();
	    // assert(pConstPS != nullptr);
	    // pConstPS->Update(m_material);
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
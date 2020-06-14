#include "Model.h"
#include "NCE.h"
#include "utils/objloader/ObjLoader.h"
#include "math/NCVector.h"
#include "component/Transform.h"
#include "Graphics.h"
#include "d3dresource/GraphicsResourceManager.h"


namespace nc::graphics
{
    Model::Model(const Mesh& mesh)
        : Model(mesh, Material{})
    {
    }

    Model::Model(const Mesh& mesh, const Material& material)
        : m_mesh(mesh), m_material(material)
    {
        InitializeGraphicsPipeline();
    }

    void Model::InitializeGraphicsPipeline() 
    {
        using namespace nc::graphics::d3dresource;
        using namespace nc::graphics;

        MeshData meshData = m_mesh.GetMeshData();
        auto pvs = GraphicsResourceManager::Acquire<VertexShader>("project\\shaders\\compiled\\litvertexshader.cso");
        auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();

        AddGraphicsResource(std::move(pvs));
        AddGraphicsResource(GraphicsResourceManager::Acquire<PixelShader> ("project\\shaders\\compiled\\litpixelshader.cso"));
        AddGraphicsResource(GraphicsResourceManager::Acquire<VertexBuffer>(meshData.Vertices, meshData.MeshPath));
        AddGraphicsResource(GraphicsResourceManager::Acquire<IndexBuffer> (meshData.Indices, meshData.MeshPath));
        AddGraphicsResource(GraphicsResourceManager::Acquire<InputLayout> (meshData.MeshPath, meshData.InputElementDesc, pvsbc));
        AddGraphicsResource(GraphicsResourceManager::Acquire<Topology>    (meshData.PrimitiveTopology));
        AddGraphicsResource(TransformConstBuffer::AcquireUnique(meshData.MeshPath, *this, 0u));
        AddGraphicsResource(PixelConstBuffer<Material>::AcquireUnique(m_material, 1u));
    }

    void Model::SetMaterial(const Material& material) noexcept
    {
        m_material = material;

        using namespace nc::graphics;
        auto pConstPS = this->QueryGraphicsResource<d3dresource::PixelConstBuffer<Material>>();
	    assert(pConstPS != nullptr);
	    pConstPS->Update(m_material);
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
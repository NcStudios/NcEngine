#include "Model.h"
#include "component/Transform.h"
#include "d3dresource/GraphicsResourceManager.h"
#include "Graphics.h"
#include "materials/Material.h"

namespace nc::graphics
{
    Model::Model()
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
        auto bufferId = std::to_string(GraphicsResourceManager::AssignId());
        m_transformConstantBuffer = TransformConstBufferVertexPixel::AcquireUnique(bufferId, *this, 0u, 2u);
        m_indexBuffer = m_mesh.QueryGraphicsResource<d3dresource::IndexBuffer>();
    }

    void Model::SetMaterial(const Material& material) noexcept
    {
        m_material = material;
        // TODO: Set material
    }
    
    void Model::SetMesh(const Mesh& mesh) noexcept
    {
        m_mesh = mesh;
    }

    void Model::Submit(FrameManager& frame) const noexcept
    {
        m_material.Submit(frame, *this);
    }

    void Model::Bind() const
    {
        m_mesh.Bind();
        m_transformConstantBuffer->Bind();
    }

    uint32_t Model::GetIndexCount() const noexcept
    {
        return m_indexBuffer->GetCount();
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
}
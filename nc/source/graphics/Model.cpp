#include "graphics/Model.h"
#include "d3dresource/GraphicsResourceManager.h"
#include "d3dresource/MeshResources.h"
#include "d3dresource/ConstantBufferResources.h"

namespace nc::graphics
{
    Model::Model(Mesh&& mesh, Material&& material)
        : m_mesh{mesh},
          m_material{material},
          m_indexBuffer{nullptr},
          m_transformConstantBuffer{nullptr}
    {
        InitializeGraphicsPipeline();
    }

    Model::~Model() = default;

    void Model::InitializeGraphicsPipeline() 
    {
        using namespace nc::graphics::d3dresource;
        auto bufferId = std::to_string(GraphicsResourceManager::AssignId());
        m_transformConstantBuffer = std::make_unique<TransformConstBufferVertexPixel>(bufferId, *this, 0u, 2u);
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
        m_material.Submit(frame, this);
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

    void Model::SetTransformationMatrix(DirectX::FXMMATRIX matrix) noexcept
    {
        m_transformationMatrix = matrix;
    }

    DirectX::FXMMATRIX Model::GetTransformationMatrix() const noexcept
    {
        return m_transformationMatrix;
    }

    Material* Model::GetMaterial() noexcept
    {
        return &m_material;
    }
}
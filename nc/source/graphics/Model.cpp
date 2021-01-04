#include "Model.h"
#include "component/Transform.h"
#include "d3dresource/GraphicsResourceManager.h"
#include "d3dresource/MeshResources.h"
#include "d3dresource/ConstantBufferResources.h"
#include "Graphics.h"
#include "materials/Material.h"

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

    void Model::UpdateTransformationMatrixHack(Transform* transform, const Vector3& scale) noexcept
    {
        auto v_rot = DirectX::XMMatrixRotationRollPitchYaw(transform->Pitch(), transform->Yaw(), transform->Roll());
        auto v_scl = DirectX::XMMatrixScaling(transform->SclX(), transform->SclY(), transform->SclZ());
        auto v_trn = DirectX::XMMatrixTranslation(transform->PosX(), transform->PosY(), transform->PosZ());
        auto v_scl2 = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
        m_transformationMatrix = v_scl * v_scl2 * v_rot * v_trn;
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
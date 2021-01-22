#include "Transform.h"
#include "directx/math/DirectXMath.h"
#include <limits>

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    Transform::Transform(EntityHandle handle, const Vector3& pos, const Vector3& rot, const Vector3& scl) noexcept
        : ComponentBase(handle),
          m_position { pos },
          m_rotation { rot },
          m_scale { scl }
    {
    }

    #ifdef NC_EDITOR_ENABLED
    void Transform::EditorGuiElement()
    {
        const float itemWidth = 80.0f;
        const float dragSpeed = 1.0f;

        ImGui::PushItemWidth(itemWidth);
            ImGui::Spacing();  ImGui::Separator();  ImGui::Text("Transform");
            ImGui::BeginGroup();
                ImGui::Indent();    ImGui::Text("Position");
                ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderFloat( "##transformxpos", &m_position.x, -80.0f, 80.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderFloat( "##transformypos", &m_position.y, -80.0f, 80.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderFloat( "##transformzpos", &m_position.z,  -80.0f, 80.0f, "%.1f", dragSpeed);
            ImGui::EndGroup();
            ImGui::BeginGroup();
                ImGui::Indent();    ImGui::Text("Rotation");
                ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderAngle("##transformxrot", &m_rotation.x, -180.0f, 180.0f);  ImGui::SameLine();
                ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderAngle("##transformyrot", &m_rotation.y, -180.0f, 180.0f);  ImGui::SameLine();
                ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderAngle("##transformzrot", &m_rotation.z, -180.0f, 180.0f);
            ImGui::EndGroup();
            ImGui::BeginGroup();
                ImGui::Indent();    ImGui::Text("Scale");
                ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderFloat("##transformxscale", &m_scale.x, 0.01f, 100.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderFloat("##transformyscale", &m_scale.y, 0.01f, 100.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderFloat("##transformzscale", &m_scale.z, 0.01f, 100.0f, "%.1f", dragSpeed);
            ImGui::EndGroup();
            ImGui::Separator();
        ImGui::PopItemWidth();
    }
    #endif
    
    DirectX::XMMATRIX Transform::GetTransformationMatrix() const
    {
        return DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
               DirectX::XMMatrixRotationRollPitchYaw(Pitch(), Yaw(), Roll()) *
               DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
    }

    DirectX::XMMATRIX Transform::GetTransformationMatrixEx(Vector3 additionalScale) const
    {
        return DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
               DirectX::XMMatrixScaling(additionalScale.x, additionalScale.y, additionalScale.z) *
               DirectX::XMMatrixRotationRollPitchYaw(Pitch(), Yaw(), Roll()) *
               DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
    }

    DirectX::XMMATRIX Transform::GetViewMatrix() const
    {
        using xmf3 = DirectX::XMFLOAT3;
        using xmv  = DirectX::XMVECTOR;

        const xmv forwardBaseVec = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        const xmv v_lookVector   = XMVector3Transform(forwardBaseVec, DirectX::XMMatrixRotationRollPitchYaw(Pitch(), Yaw(), Roll()));
        xmf3      camPosition    = m_position.ToXMFloat3();
        const xmv v_camPosition  = XMLoadFloat3(&camPosition);
        const xmv v_camTarget    = v_camPosition + v_lookVector;

        return DirectX::XMMatrixLookAtLH(v_camPosition, v_camTarget, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    }

    void Transform::Translate(Vector3 translation, Space space) noexcept
    {
        if(space == Space::World)
        {
            m_position = m_position + translation;
            return;
        }

        auto trans       = translation.ToXMFloat3();
        auto v_trans     = DirectX::XMLoadFloat3(&trans);
        auto v_rotMatrix = DirectX::XMMatrixRotationRollPitchYaw(Pitch(), Yaw(), Roll());
        XMStoreFloat3(&trans, DirectX::XMVector3Transform(v_trans, v_rotMatrix));
        m_position = m_position + Vector3(trans);
    }

    void Transform::Rotate(float xAngle, float yAngle, float zAngle, float speed) noexcept
    {
        m_rotation = Vector3
        {
            (float)math::WrapAngle(m_rotation.x + xAngle * speed),
            (float)math::WrapAngle(m_rotation.y + yAngle * speed),
            (float)math::WrapAngle(m_rotation.z + zAngle * speed)
        };
    }

    void Transform::RotateClamped(float xAngle, float yAngle, float zAngle, float speed, float min, float max) noexcept
    {
        m_rotation = Vector3
        {
            (float)math::Clamp(m_rotation.x + xAngle * speed, min, max),
            (float)math::Clamp(m_rotation.y + yAngle * speed, min, max),
            (float)math::Clamp(m_rotation.z + zAngle * speed, min, max)
        };
    }
}
#include "Transform.h"
#include "directx/math/DirectXMath.h"
#include <limits>

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    Transform::Transform() noexcept
        : Transform({},{},{})
    {
    }

    Transform::Transform(const Vector3& pos, const Vector3& rot, const Vector3& scl) noexcept
        : m_position { pos }, m_rotation { rot }, m_scale { scl }
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
                ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderFloat( "##xpos", &m_position.m_x, -80.0f, 80.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderFloat( "##ypos", &m_position.m_y, -80.0f, 80.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderFloat( "##zpos", &m_position.m_z,  -80.0f, 80.0f, "%.1f", dragSpeed);
            ImGui::EndGroup();
            ImGui::BeginGroup();
                ImGui::Indent();    ImGui::Text("Rotation");
                ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderAngle("##xrot", &m_rotation.m_x, -180.0f, 180.0f);  ImGui::SameLine();
                ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderAngle("##yrot", &m_rotation.m_y, -180.0f, 180.0f);  ImGui::SameLine();
                ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderAngle("##zrot", &m_rotation.m_z, -180.0f, 180.0f);
            ImGui::EndGroup();
            ImGui::BeginGroup();
                ImGui::Indent();    ImGui::Text("Scale");
                ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderFloat("##xscale", &m_scale.m_x, 0.01f, 100.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderFloat("##yscale", &m_scale.m_y, 0.01f, 100.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderFloat("##zscale", &m_scale.m_z, 0.01f, 100.0f, "%.1f", dragSpeed);
            ImGui::EndGroup();
            ImGui::Separator();
        ImGui::PopItemWidth();
    }
    #endif
    
    DirectX::XMMATRIX Transform::GetMatrixXM() const
    {
        auto v_rot = DirectX::XMMatrixRotationRollPitchYaw(Pitch(), Yaw(), Roll());
        auto v_scl = DirectX::XMMatrixScaling(m_scale.X(), m_scale.Y(), m_scale.Z());
        auto v_trn = DirectX::XMMatrixTranslation(m_position.X(), m_position.Y(), m_position.Z());

        return v_rot * v_scl * v_trn;
    }

    DirectX::XMMATRIX Transform::CamGetMatrix() const
    {
        using xmf3 = DirectX::XMFLOAT3;
        using xmv  = DirectX::XMVECTOR;

        const xmv forwardBaseVec = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        const xmv v_lookVector   = XMVector3Transform(forwardBaseVec, DirectX::XMMatrixRotationRollPitchYaw(Pitch(), Yaw(), Roll()));
        xmf3      camPosition    = m_position.GetXMFloat3();
        const xmv v_camPosition  = XMLoadFloat3(&camPosition);
        const xmv v_camTarget    = v_camPosition + v_lookVector;

        return DirectX::XMMatrixLookAtLH(v_camPosition, v_camTarget, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    }

    void Transform::Translate(const Vector3& vec) noexcept
    {
        m_position = m_position + vec;
    }

    void Transform::CamTranslate(Vector3& translation, float factor) noexcept
    {
        auto trans       = translation.GetXMFloat3();
        auto v_trans     = DirectX::XMLoadFloat3(&trans);
        auto v_rotMatrix = DirectX::XMMatrixRotationRollPitchYaw(Pitch(), Yaw(), Roll());
        auto v_sclMatrix = DirectX::XMMatrixScaling(factor, factor, factor);
        XMStoreFloat3(&trans, DirectX::XMVector3Transform(v_trans, v_rotMatrix * v_sclMatrix));
        m_position = m_position + Vector3(trans);
    }

    void Transform::Rotate(float xAngle, float yAngle, float zAngle, float speed) noexcept
    {
        m_rotation = Vector3
        {
            (float)math::WrapAngle(m_rotation.X() + xAngle * speed),
            (float)math::WrapAngle(m_rotation.Y() + yAngle * speed),
            (float)math::WrapAngle(m_rotation.Z() + zAngle * speed)
        };
    }

    void Transform::RotateClamped(float xAngle, float yAngle, float zAngle, float speed, float min, float max) noexcept
    {
        m_rotation = Vector3
        {
            (float)math::Clamp(m_rotation.X() + xAngle * speed, min, max),
            (float)math::Clamp(m_rotation.Y() + yAngle * speed, min, max),
            (float)math::Clamp(m_rotation.Z() + zAngle * speed, min, max)
        };
    }
}
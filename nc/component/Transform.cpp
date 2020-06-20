#include "Transform.h"
#include "directx/math/DirectXMath.h"

#include <limits>

#ifdef NC_EDITOR_ENABLED
#include "external/imgui/imgui.h"
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
        const float itemWidth = 40.0f;
        const float dragSpeed = 0.75f;

        ImGui::PushItemWidth(itemWidth);
            ImGui::Spacing();  ImGui::Separator();  ImGui::Text("Transform");

            ImGui::BeginGroup();
                ImGui::Indent();    ImGui::Text("Position");
                ImGui::Text("X:");  ImGui::SameLine();  ImGui::DragFloat( "##xpos", &m_position.m_x, dragSpeed, -80.0f, 80.0f, "%.1f");  ImGui::SameLine();
                ImGui::Text("Y:");  ImGui::SameLine();  ImGui::DragFloat( "##ypos", &m_position.m_y, dragSpeed, -80.0f, 80.0f, "%.1f");  ImGui::SameLine();
                ImGui::Text("Z:");  ImGui::SameLine();  ImGui::DragFloat( "##zpos", &m_position.m_z, dragSpeed, -80.0f, 80.0f, "%.1f");
            ImGui::EndGroup();
            ImGui::BeginGroup();
                ImGui::Indent();    ImGui::Text("Rotation");
                ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderAngle("##xrot", &m_rotation.m_x, -180.0f, 180.0f);  ImGui::SameLine();
                ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderAngle("##yrot", &m_rotation.m_y, -180.0f, 180.0f);  ImGui::SameLine();
                ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderAngle("##zrot", &m_rotation.m_z, -180.0f, 180.0f);
            ImGui::EndGroup();
            ImGui::BeginGroup();
                ImGui::Indent();    ImGui::Text("Scale");
                ImGui::Text("X:");  ImGui::SameLine();  ImGui::DragFloat("##xscale", &m_scale.m_x, dragSpeed, 0.01f, 100.0f, "%.1f");  ImGui::SameLine();
                ImGui::Text("Y:");  ImGui::SameLine();  ImGui::DragFloat("##yscale", &m_scale.m_y, dragSpeed, 0.01f, 100.0f, "%.1f");  ImGui::SameLine();
                ImGui::Text("Z:");  ImGui::SameLine();  ImGui::DragFloat("##zscale", &m_scale.m_z, dragSpeed, 0.01f, 100.0f, "%.1f");
            ImGui::EndGroup();
            ImGui::Separator();
        ImGui::PopItemWidth();
    }
    #endif
    
    DirectX::XMMATRIX Transform::GetMatrixXM()
    {
        auto v_rot = DirectX::XMMatrixRotationRollPitchYaw(Pitch(), Yaw(), Roll());
        auto v_scl = DirectX::XMMatrixScaling(m_scale.X(), m_scale.Y(), m_scale.Z());
        auto v_trn = DirectX::XMMatrixTranslation(m_position.X(), m_rotation.X(), m_rotation.Z());

        return v_rot * v_scl * v_trn;
    }

    DirectX::XMMATRIX Transform::CamGetMatrix()
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
        m_position.TranslateBy(vec);
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


    // DirectX::XMMATRIX GetLookMatrix() const noexcept
    // {
    //     const auto zero = DirectX::XMVectorZero();
    //     const auto up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    //     const auto pos = DirectX::XMVectorSet(m_position.GetX(), m_position.GetY(), -m_position.GetZ(), 0.0f);
    //     const auto rot = DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0.0f);
    //     const auto transformedPos = DirectX::XMVector3Transform(pos, rot);

    //     return DirectX::XMMatrixLookAtLH(transformedPos, zero, up) *
    //            DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0);
    // }

    //void Rotate(const Vector3& vec)    noexcept { m_rotation.TranslateBy(vec); } 

    // void RotateAround(const Vector3& axis, const float angle)
    // {
    //     m_rotation = m_rotation + (axis * angle);
    // }

    // void RotateByVector(const Vector3& vec) noexcept
    // {
    //     auto cameraRotation   = m_rotation.GetXMFloat3();
    //     auto rotationVector   = vec.GetXMFloat3();
    //     auto v_cameraRotation = DirectX::XMLoadFloat3(&cameraRotation);//  m_rotation.GetXMVector();
    //     auto v_rotationVector = DirectX::XMLoadFloat3(&rotationVector);

    //     v_cameraRotation = DirectX::XMVector3Transform(v_cameraRotation, DirectX::XMMatrixRotationRollPitchYawFromVector(v_rotationVector));

    //     DirectX::XMStoreFloat3(&cameraRotation, v_cameraRotation);

    //     m_rotation = m_rotation + Vector3(cameraRotation);
    // }


}
#include "Transform.h"
#include <limits>
#include "DirectXMath.h"

namespace nc
{
    Transform::Transform(ComponentHandle handle, EntityView parentView) noexcept
        : Component(handle, parentView) 
    {}

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

    Vector3 Transform::GetPosition() const noexcept { return m_position;     }
    inline Vector3 Transform::GetRotation() const noexcept { return m_rotation;     }
    inline Vector3 Transform::GetScale()    const noexcept { return m_scale;        }
    inline float   Transform::Pitch()       const noexcept { return m_rotation.m_x; }
    inline float   Transform::Roll()        const noexcept { return m_rotation.m_z; }
    inline float   Transform::Yaw()         const noexcept { return m_rotation.m_y; }
    
    DirectX::XMMATRIX Transform::GetMatrixXM()
    {
        auto pos           = m_position.GetXMFloat3();
        //1auto rot           = m_rotation.GetXMFloat3();
        auto scl           = m_scale.GetXMFloat3();
        auto v_rotMatrix   = DirectX::XMMatrixRotationRollPitchYaw(Pitch(), Yaw(), Roll());
        auto v_scaleMatrix = DirectX::XMMatrixScaling(scl.x,scl.y,scl.z);
        auto v_transMatrix = DirectX::XMMatrixTranslation(pos.x,pos.y,pos.z);

        return v_rotMatrix * v_scaleMatrix * v_transMatrix;
    }

    DirectX::XMMATRIX Transform::CamGetMatrix()
    {
        using xmf3 = DirectX::XMFLOAT3; using xmv = DirectX::XMVECTOR;
        const xmv forwardBaseVec = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        const xmv v_lookVector   = XMVector3Transform(forwardBaseVec, DirectX::XMMatrixRotationRollPitchYaw(Pitch(), Yaw(), Roll()));
        xmf3      camPosition    = m_position.GetXMFloat3();
        const xmv v_camPosition  = XMLoadFloat3(&camPosition);
        const xmv v_camTarget    = v_camPosition + v_lookVector;
        return DirectX::XMMatrixLookAtLH(v_camPosition, v_camTarget, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    }

    void Transform::Set(const Vector3& pos, const Vector3& rot, const Vector3& scale) noexcept { m_position = pos; m_rotation = rot; m_scale = scale; }
    inline void Transform::SetPosition(const Vector3& pos)                                   noexcept { m_position = pos;   }
    inline void Transform::SetRotation(const Vector3& rot)                                   noexcept { m_rotation = rot;   }
    inline void Transform::SetScale(const Vector3& scale)                                    noexcept { m_scale    = scale; }

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
        float xRot = math::WrapAngle(m_rotation.X() + xAngle * speed);
        float yRot = math::WrapAngle(m_rotation.Y() + yAngle * speed);
        float zRot = math::WrapAngle(m_rotation.Z() + zAngle * speed);
        m_rotation = Vector3(xRot, yRot, zRot);
    }

    void Transform::RotateClamped(float xAngle, float yAngle, float zAngle, float speed, float min, float max) noexcept
    {
        float xRot = math::Clamp(m_rotation.X() + xAngle * speed, min, max);
        float yRot = math::Clamp(m_rotation.Y() + yAngle * speed, min, max);
        float zRot = math::Clamp(m_rotation.Z() + zAngle * speed, min, max);
        m_rotation = Vector3(xRot, yRot, zRot);
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
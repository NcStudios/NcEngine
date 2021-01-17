#include "Transform.h"
#include "directx/math/DirectXMath.h"
#include <limits>

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

// namespace
// {
//     DirectX::XMMATRIX ComposeMatrix(const Vector3& pos, const Vector3& rot, const Vector3& scale)
//     {
//         return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
//                DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) *
//                DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
//     }
// }

namespace nc
{
    Transform::Transform(EntityHandle handle, const Vector3& pos, const Vector3& rot, const Vector3& scl) noexcept
        : Component(handle)
    {
        // operator overloads are inefficient according to documentation? 
        // also try aligned functions (StoreFloat3A)

        auto v_rot = DirectX::XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z);
        DirectX::XMFLOAT4 rot4;
        DirectX::XMStoreFloat4(&rot4, v_rot);

        auto dec = DecomposedMatrix{ rot4, pos.ToXMFloat3(), scl.ToXMFloat3() };
        m_matrix = ComposeMatrix(dec);
    }

    DirectX::XMMATRIX Transform::ComposeMatrix(const DecomposedMatrix& vectors) const
    {
        auto v_rot = DirectX::XMVectorSet(vectors.rotation.x, vectors.rotation.y, vectors.rotation.z, vectors.rotation.w);

        return DirectX::XMMatrixScaling(vectors.scale.x, vectors.scale.y, vectors.scale.z) *
               DirectX::XMMatrixRotationQuaternion(v_rot) *
               DirectX::XMMatrixTranslation(vectors.position.x, vectors.position.y, vectors.position.z);
    }

    DecomposedMatrix Transform::DecomposeMatrix() const
    {
        DirectX::XMVECTOR v_pos;
        DirectX::XMVECTOR v_rot;
        DirectX::XMVECTOR v_scl;

        DirectX::XMMatrixDecompose(&v_scl, &v_rot, &v_pos, m_matrix);
        DecomposedMatrix out;
        DirectX::XMStoreFloat3(&out.position, v_pos);
        DirectX::XMStoreFloat4(&out.rotation, v_rot);
        DirectX::XMStoreFloat3(&out.scale, v_scl);
        return out;
    } 

    Vector3 Transform::GetPosition() const
    {
        DirectX::XMFLOAT3 row;
        DirectX::XMStoreFloat3(&row, m_matrix.r[3]);
        return {row};
    }

    Vector4 Transform::GetRotation() const
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        DirectX::XMFLOAT4 rot;
        DirectX::XMStoreFloat4(&rot, rot_v);
        return Vector4{rot};
    }

    Vector3 Transform::GetScale() const
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        DirectX::XMFLOAT3 scl;
        DirectX::XMStoreFloat3(&scl, scl_v);
        return Vector3{scl};
    }

    #ifdef NC_EDITOR_ENABLED
    void Transform::EditorGuiElement()
    {
        const float itemWidth = 80.0f;
        //const float dragSpeed = 1.0f;

        ImGui::PushItemWidth(itemWidth);
            ImGui::Spacing();  ImGui::Separator();  ImGui::Text("Transform");
            // ImGui::BeginGroup();
            //     ImGui::Indent();    ImGui::Text("Position");
            //     ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderFloat( "##transformxpos", &m_position.x, -80.0f, 80.0f, "%.1f", dragSpeed);  ImGui::SameLine();
            //     ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderFloat( "##transformypos", &m_position.y, -80.0f, 80.0f, "%.1f", dragSpeed);  ImGui::SameLine();
            //     ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderFloat( "##transformzpos", &m_position.z,  -80.0f, 80.0f, "%.1f", dragSpeed);
            // ImGui::EndGroup();
            // ImGui::BeginGroup();
            //     ImGui::Indent();    ImGui::Text("Rotation");
            //     ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderAngle("##transformxrot", &m_rotation.x, -180.0f, 180.0f);  ImGui::SameLine();
            //     ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderAngle("##transformyrot", &m_rotation.y, -180.0f, 180.0f);  ImGui::SameLine();
            //     ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderAngle("##transformzrot", &m_rotation.z, -180.0f, 180.0f);
            // ImGui::EndGroup();
            // ImGui::BeginGroup();
            //     ImGui::Indent();    ImGui::Text("Scale");
            //     ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderFloat("##transformxscale", &m_scale.x, 0.01f, 100.0f, "%.1f", dragSpeed);  ImGui::SameLine();
            //     ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderFloat("##transformyscale", &m_scale.y, 0.01f, 100.0f, "%.1f", dragSpeed);  ImGui::SameLine();
            //     ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderFloat("##transformzscale", &m_scale.z, 0.01f, 100.0f, "%.1f", dragSpeed);
            // ImGui::EndGroup();
            ImGui::Separator();
        ImGui::PopItemWidth();
    }
    #endif
    
    DirectX::XMMATRIX Transform::GetTransformationMatrix() const
    {
        return m_matrix;
    }

    DirectX::XMMATRIX Transform::GetTransformationMatrixEx(Vector3 additionalScale) const
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);

        return DirectX::XMMatrixScalingFromVector(scl_v) *
               DirectX::XMMatrixScaling(additionalScale.x, additionalScale.y, additionalScale.z) *
               DirectX::XMMatrixRotationQuaternion(rot_v) *
               DirectX::XMMatrixTranslationFromVector(pos_v);
    }

    DirectX::XMMATRIX Transform::GetViewMatrix() const
    {
        using up_v = DirectX::g_XMIdentityR1;
        using forward_v = DirectX::g_XMIdentityR2;

        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        auto look_v = DirectX::XMVector3Transform(up_v, DirectX::XMMatrixRotationQuaternion(rot_v));
        return DirectX::XMMatrixLookAtLH(pos_v, pos_v + look_v, forward_v);
    }

    void Transform::Set(const Vector3& pos, const Vector3& rot, const Vector3& scale) noexcept
    {
        m_matrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
                   DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) *
                   DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
    }

    void Transform::SetPosition(const Vector3& pos) noexcept
    {
        auto dec = DecomposeMatrix();
        dec.position = pos.ToXMFloat3();
        m_matrix = ComposeMatrix(dec);
    }

    void Transform::SetRotation(const Vector3& rot) noexcept
    {
        auto dec = DecomposeMatrix();
        auto rot_v = DirectX::XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z);
        DirectX::XMFLOAT4 rot4;
        DirectX::XMStoreFloat4(&rot4, rot_v);
        dec.rotation = rot4;
        m_matrix = ComposeMatrix(dec);
    }

    void Transform::SetScale(const Vector3& scale) noexcept
    {
        auto dec = DecomposeMatrix();
        dec.scale = scale.ToXMFloat3();
        m_matrix = ComposeMatrix(dec);
    }

    void Transform::Translate(Vector3 translation, Space space) noexcept
    {
        if(space == Space::World)
        {
            m_matrix = m_matrix * DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);
            return;
        }

        DirectX::XMVECTOR pos_v, rot_v, scl_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        auto rot_m = DirectX::XMMatrixRotationQuaternion(rot_v);
        auto trans_v = DirectX::XMVectorSet(translation.x, translation.y, translation.z, 0.0f);
        trans_v = DirectX::XMVector3Transform(trans_v, rot_m);
        pos_v = pos_v + trans_v;
        m_matrix = DirectX::XMMatrixScalingFromVector(scl_v) *
                   rot_m *
                   DirectX::XMMatrixTranslationFromVector(pos_v);            
    }

    void Transform::Rotate(Vector3 axis, float radians)
    {
        auto dec = DecomposeMatrix();
        auto rot_v = DirectX::XMQuaternionMultiply
        (
            DirectX::XMLoadFloat4(&dec.rotation),
            DirectX::XMQuaternionRotationAxis
            (
                DirectX::XMVectorSet(axis.x, axis.y, axis.z, 0.0f),
                radians
            )
        );
        DirectX::XMStoreFloat4(&dec.rotation, rot_v);
        m_matrix = ComposeMatrix(dec);
    }

    // void Transform::Rotate(float xAngle, float yAngle, float zAngle, float speed) noexcept
    // {
    //     m_rotation = Vector3
    //     {
    //         (float)math::WrapAngle(m_rotation.x + xAngle * speed),
    //         (float)math::WrapAngle(m_rotation.y + yAngle * speed),
    //         (float)math::WrapAngle(m_rotation.z + zAngle * speed)
    //     };
    // }

    // void Transform::RotateClamped(float xAngle, float yAngle, float zAngle, float speed, float min, float max) noexcept
    // {
    //     m_rotation = Vector3
    //     {
    //         (float)math::Clamp(m_rotation.x + xAngle * speed, min, max),
    //         (float)math::Clamp(m_rotation.y + yAngle * speed, min, max),
    //         (float)math::Clamp(m_rotation.z + zAngle * speed, min, max)
    //     };
    // }
}
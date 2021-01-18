#include "Transform.h"
#include "DebugUtils.h"
#include "directx/math/DirectXMath.h"
#include <limits>

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace
{
    using namespace DirectX;
    using namespace nc;

    XMVECTOR ToXMVector(const Vector3& v)     { return XMVectorSet(v.x, v.y, v.z, 0.0f); }
    XMVECTOR ToXMVector(const Quaternion& q)  { return XMVectorSet(q.x, q.y, q.z, q.w); }
    XMMATRIX ToTransMatrix(const Vector3& v)  { return XMMatrixTranslation(v.x, v.y, v.z); }
    XMMATRIX ToScaleMatrix(const Vector3& v)  { return XMMatrixScaling(v.x, v.y, v.z); }
    XMMATRIX ToRotMatrix(const Vector3& v)    { return XMMatrixRotationRollPitchYaw(v.x, v.y, v.z); }
    XMMATRIX ToRotMatrix(const Quaternion& q) { return XMMatrixRotationQuaternion(ToXMVector(q)); }

    XMMATRIX ComposeMatrix(const Vector3& scale, const Quaternion& rot, const Vector3& pos)
    {
        return ToScaleMatrix(scale) * ToRotMatrix(rot) * ToTransMatrix(pos);
    }

    XMMATRIX ComposeMatrix(const XMVECTOR& scale, const XMVECTOR& rot, const XMVECTOR& pos)
    {
        return XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(rot) * XMMatrixTranslationFromVector(pos);
    }
}

namespace nc
{
    Transform::Transform(EntityHandle handle, const Vector3& pos, const Quaternion& rot, const Vector3& scale) noexcept
        : Component(handle),
          m_matrix{ComposeMatrix(scale, rot, pos)}
    {}

    Vector3 Transform::GetPosition() const
    {
        DirectX::XMFLOAT3 row;
        DirectX::XMStoreFloat3(&row, m_matrix.r[3]);
        return Vector3{row};
    }

    Quaternion Transform::GetRotation() const
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        return Quaternion::FromXMVector(rot_v);
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
               additionalScale.ToXMScalingMatrix() *
               DirectX::XMMatrixRotationQuaternion(rot_v) *
               DirectX::XMMatrixTranslationFromVector(pos_v);
    }

    DirectX::XMMATRIX Transform::GetViewMatrix() const
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        auto look_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, DirectX::XMMatrixRotationQuaternion(rot_v));
        return DirectX::XMMatrixLookAtLH(pos_v, pos_v + look_v, DirectX::g_XMIdentityR1);
    }

    void Transform::Set(const Vector3& pos, const Quaternion& quat, const Vector3& scale)
    {
        IF_THROW(quat == Quaternion::Zero(), "Transform::Set - Invalid quaternion(Quaternion::Zero)");
        IF_THROW(scale == Vector3::Zero(), "Transform::Set - Invalid scale(Vector3::Zero)");
        m_matrix = scale.ToXMScalingMatrix() * quat.ToXMMatrix() * pos.ToXMTranslationMatrix();
    }

    void Transform::Set(const Vector3& pos, const Vector3& angles, const Vector3& scale)
    {
        IF_THROW(scale == Vector3::Zero(), "Ecs::CreateEntity - Invalid scale(Vector3::Zero)");
        m_matrix = scale.ToXMScalingMatrix() * angles.ToXMRotationMatrix() * pos.ToXMTranslationMatrix();
    }

    void Transform::SetPosition(const Vector3& pos)
    {
        m_matrix.r[3] = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 0.0f);
    }

    void Transform::SetRotation(const Quaternion& quat)
    {
        IF_THROW(quat == Vector3::Zero(), "Ecs::SetRotation - Invalid quaternion(Quaternion::Zero)");
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        m_matrix = DirectX::XMMatrixScalingFromVector(scl_v) *
                   quat.ToXMMatrix() *
                   DirectX::XMMatrixTranslationFromVector(pos_v);
    }

    void Transform::SetRotation(const Vector3& angles)
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        m_matrix = DirectX::XMMatrixScalingFromVector(scl_v) *
                   DirectX::XMMatrixRotationRollPitchYaw(angles.x, angles.y, angles.z) *
                   DirectX::XMMatrixTranslationFromVector(pos_v);
    }

    void Transform::SetScale(const Vector3& scale)
    {
        IF_THROW(scale == Vector3::Zero(), "Ecs::SetScale - Invalid scale(Vector3::Zero)");
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        m_matrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
                   DirectX::XMMatrixRotationQuaternion(rot_v) *
                   DirectX::XMMatrixTranslationFromVector(pos_v);
    }

    void Transform::Translate(Vector3 translation, Space space)
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
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        rot_v = DirectX::XMQuaternionMultiply
        (
            rot_v,
            DirectX::XMQuaternionRotationAxis
            (
                DirectX::XMVectorSet(axis.x, axis.y, axis.z, 0.0f),
                radians
            )
        );
        m_matrix = ComposeMatrix(scl_v, rot_v, pos_v);
    }

    void Transform::Rotate(const Quaternion& quat)
    {
        IF_THROW(quat == Quaternion::Zero(), "Transform::Rotate - Invalid quaternion(Quaternion::Zero)");
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        rot_v = DirectX::XMQuaternionMultiply(rot_v, quat.ToXMVector());
        m_matrix = ComposeMatrix(scl_v, rot_v, pos_v);
    }
}
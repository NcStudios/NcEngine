#include "Transform.h"
#include "DebugUtils.h"
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
        DirectX::XMFLOAT4 quat;
        DirectX::XMStoreFloat4(&quat, rot_v);
        return Quaternion{quat};
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
        const float dragSpeed = 0.3f;

        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        DirectX::XMFLOAT3 scl;
        DirectX::XMFLOAT4 rot;
        DirectX::XMFLOAT3 pos;
        DirectX::XMStoreFloat3(&scl, scl_v);
        DirectX::XMStoreFloat4(&rot, rot_v);
        DirectX::XMStoreFloat3(&pos, pos_v);

        ImGui::PushItemWidth(itemWidth);
            ImGui::Spacing();  ImGui::Separator();  ImGui::Text("Transform");
            ImGui::BeginGroup();
                ImGui::Indent();    ImGui::Text("Position");
                ImGui::Text("X:");  ImGui::SameLine();
                    auto xPosRes = ImGui::SliderFloat( "##transformxpos", &pos.x, -80.0f, 80.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Y:");  ImGui::SameLine();
                    auto yPosRes = ImGui::SliderFloat( "##transformypos", &pos.y, -80.0f, 80.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Z:");  ImGui::SameLine();
                    auto zPosRes = ImGui::SliderFloat( "##transformzpos", &pos.z,  -80.0f, 80.0f, "%.1f", dragSpeed);
            ImGui::EndGroup();
            // ImGui::BeginGroup();
            //     ImGui::Indent();    ImGui::Text("Rotation");
            //     ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderAngle("##transformxrot", &rot.x, -180.0f, 180.0f);  ImGui::SameLine();
            //     ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderAngle("##transformyrot", &rot.y, -180.0f, 180.0f);  ImGui::SameLine();
            //     ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderAngle("##transformzrot", &rot.z, -180.0f, 180.0f);  ImGui::SameLine();
            //     ImGui::Text("W:");  ImGui::SameLine();  ImGui::SliderAngle("##transformwrot", &rot.w, -180.0f, 180.0f);
            // ImGui::EndGroup();
            // ImGui::BeginGroup();
            //     ImGui::Indent();    ImGui::Text("Scale");
            //     ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderFloat("##transformxscale", &scl.x, 0.01f, 100.0f, "%.1f", dragSpeed);  ImGui::SameLine();
            //     ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderFloat("##transformyscale", &scl.y, 0.01f, 100.0f, "%.1f", dragSpeed);  ImGui::SameLine();
            //     ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderFloat("##transformzscale", &scl.z, 0.01f, 100.0f, "%.1f", dragSpeed);
            // ImGui::EndGroup();
            ImGui::Separator();
        ImGui::PopItemWidth();

        if(xPosRes || yPosRes || zPosRes)
        {
            m_matrix = ComposeMatrix({scl}, {rot}, {pos});
        }
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
               ToScaleMatrix(additionalScale) *
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
        m_matrix = ToScaleMatrix(scale) * ToRotMatrix(quat) * ToTransMatrix(pos);
    }

    void Transform::Set(const Vector3& pos, const Vector3& angles, const Vector3& scale)
    {
        IF_THROW(scale == Vector3::Zero(), "Transform::Set - Invalid scale(Vector3::Zero)");
        m_matrix = ToScaleMatrix(scale) * ToRotMatrix(angles) * ToTransMatrix(pos);
    }

    void Transform::SetPosition(const Vector3& pos)
    {
        m_matrix.r[3] = ToXMVector(pos);
    }

    void Transform::SetRotation(const Quaternion& quat)
    {
        IF_THROW(quat == Quaternion::Zero(), "Transform::SetRotation - Invalid quaternion(Quaternion::Zero)");
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        m_matrix = DirectX::XMMatrixScalingFromVector(scl_v) *
                   ToRotMatrix(quat) *
                   DirectX::XMMatrixTranslationFromVector(pos_v);
    }

    void Transform::SetRotation(const Vector3& angles)
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        m_matrix = DirectX::XMMatrixScalingFromVector(scl_v) *
                   ToRotMatrix(angles) *
                   DirectX::XMMatrixTranslationFromVector(pos_v);
    }

    void Transform::SetScale(const Vector3& scale)
    {
        IF_THROW(scale == Vector3::Zero(), "Transform::SetScale - Invalid scale(Vector3::Zero)");
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        m_matrix = ToScaleMatrix(scale) *
                   DirectX::XMMatrixRotationQuaternion(rot_v) *
                   DirectX::XMMatrixTranslationFromVector(pos_v);
    }

    void Transform::Translate(Vector3 translation, Space space)
    {
        if(space == Space::World)
        {
            m_matrix = m_matrix * ToTransMatrix(translation);
            return;
        }

        DirectX::XMVECTOR pos_v, rot_v, scl_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        auto rot_m = DirectX::XMMatrixRotationQuaternion(rot_v);
        pos_v += DirectX::XMVector3Transform(ToXMVector(translation), rot_m);
        m_matrix = DirectX::XMMatrixScalingFromVector(scl_v) *
                   rot_m *
                   DirectX::XMMatrixTranslationFromVector(pos_v);
    }

    void Transform::Rotate(const Quaternion& quat)
    {
        IF_THROW(quat == Quaternion::Zero(), "Transform::Rotate - Invalid quaternion(Quaternion::Zero)");
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        rot_v = DirectX::XMQuaternionMultiply(rot_v, ToXMVector(quat));
        m_matrix = ComposeMatrix(scl_v, rot_v, pos_v);
    }

    void Transform::Rotate(Vector3 axis, float radians)
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        rot_v = DirectX::XMQuaternionMultiply(rot_v, DirectX::XMQuaternionRotationAxis(ToXMVector(axis), radians));
        m_matrix = ComposeMatrix(scl_v, rot_v, pos_v);
    }
}
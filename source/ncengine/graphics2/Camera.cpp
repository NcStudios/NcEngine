#include "ncengine/graphics/Camera.h"
#include "ncengine/window/Window.h"
#include "ncmath/MatrixUtilities.h"

namespace nc
{
auto MakeDefaultViewMatrix() -> DirectX::XMMATRIX
{
    static const auto defaultView = DirectX::XMMatrixLookAtRH(
        DirectX::g_XMIdentityR3,
        DirectX::g_XMIdentityR2,
        DirectX::g_XMNegIdentityR1
    );

    return defaultView;
}

auto MakeDefaultProjectionMatrix() -> DirectX::XMMATRIX
{
    const auto [width, height] = nc::window::GetScreenExtent();
    return DirectX::XMMatrixPerspectiveFovRH(
        CameraProperties::DefaultFOV,
        width / height,
        CameraProperties::DefaultNearClip,
        CameraProperties::DefaultFarClip
    );
}

Camera::Camera(Entity entity, const CameraProperties& properties) noexcept
    : FreeComponent(entity),
      m_view{},
      m_projection{},
      m_inverseProjection{},
      m_properties{properties}
{
    auto [width, height] = window::GetScreenExtent();
    UpdateProjectionMatrix(width, height);
}

auto Camera::CastToNearAndFarPlanes(const Vector2& normalizedDeviceCoords) const -> NearFarPoints
{
    using namespace DirectX;
    const auto viewProj = XMMatrixMultiply(m_view, m_projection);
    const auto viewProjInv = XMMatrixInverse(nullptr, viewProj);
    const auto nearNDC = XMVectorSet(normalizedDeviceCoords.x, -normalizedDeviceCoords.y, 0.0f, 1.0f);
    const auto farNDC = XMVectorSet(normalizedDeviceCoords.x, -normalizedDeviceCoords.y, 1.0f, 1.0f);
    auto nearWorld = XMVector4Transform(nearNDC, viewProjInv);
    auto farWorld = XMVector4Transform(farNDC, viewProjInv);
    nearWorld = XMVectorDivide(nearWorld, XMVectorSplatW(nearWorld));
    farWorld = XMVectorDivide(farWorld, XMVectorSplatW(farWorld));
    return NearFarPoints{
        ToVector3(nearWorld),
        ToVector3(farWorld)
    };
}

void Camera::UpdateViewMatrix(DirectX::FXMMATRIX transformationMatrix)
{
    // Get the +z (forward) vector in world space of the camera.
    const auto look = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, transformationMatrix);

    // Generate the view matrix that transforms points from world space into the view space of the camera.
    m_view = DirectX::XMMatrixLookAtRH(transformationMatrix.r[3], look, DirectX::g_XMNegIdentityR1);
}

void Camera::UpdateProjectionMatrix(float width, float height)
{
    m_projection = DirectX::XMMatrixPerspectiveFovRH(m_properties.fov, width / height, m_properties.nearClip, m_properties.farClip);
    m_projection.r[1] = DirectX::XMVectorScale(m_projection.r[1], -1);
    m_inverseProjection = DirectX::XMMatrixInverse(nullptr, m_projection);
}

auto Camera::CalculateFrustum() const noexcept -> Frustum
{
    using namespace DirectX;
    Frustum out;
    const auto m = XMMatrixTranspose(m_view * m_projection);

    const auto left_v = XMPlaneNormalize(XMVectorAdd(m.r[3], m.r[0]));
    XMStoreVector3(&out.left.normal, left_v);
    out.left.d = -1.0f * XMVectorGetW(left_v);

    const auto right_v = XMPlaneNormalize(XMVectorSubtract(m.r[3], m.r[0]));
    XMStoreVector3(&out.right.normal, right_v);
    out.right.d = -1.0f * XMVectorGetW(right_v);

    const auto bottom_v = XMPlaneNormalize(XMVectorAdd(m.r[3], m.r[1]));
    XMStoreVector3(&out.bottom.normal, bottom_v);
    out.bottom.d = -1.0f * XMVectorGetW(bottom_v);

    const auto top_v = XMPlaneNormalize(XMVectorSubtract(m.r[3], m.r[1]));
    XMStoreVector3(&out.top.normal, top_v);
    out.top.d = -1.0f * XMVectorGetW(top_v);

    const auto front_v = XMPlaneNormalize(m.r[2]);
    XMStoreVector3(&out.front.normal, front_v);
    out.front.d = -1.0f * XMVectorGetW(front_v);

    const auto back_v = XMPlaneNormalize(XMVectorSubtract(m.r[3], m.r[2]));
    XMStoreVector3(&out.back.normal, back_v);
    out.back.d = -1.0f * XMVectorGetW(back_v);

    return out;
}

auto Camera::CalculateFrustumCorners() const noexcept -> FrustumCorners
{
    using namespace DirectX;

    const auto viewProj = XMMatrixMultiply(m_view, m_projection);
    const auto viewProjInv = XMMatrixInverse(nullptr, viewProj);

    auto unproject = [&](float x, float y, float z) -> Vector3
    {
        auto ndc = XMVectorSet(x, y, z, 1.0f);
        auto world = XMVector4Transform(ndc, viewProjInv);
        world = XMVectorDivide(world, XMVectorSplatW(world));
        return ToVector3(world);
    };

    // NDC corners: x,y in [-1,1], z=0 for near, z=1 for far
    FrustumCorners corners;
    corners.nearTopLeft     = unproject(-1.0f,  1.0f, 0.0f);
    corners.nearTopRight    = unproject( 1.0f,  1.0f, 0.0f);
    corners.nearBottomLeft  = unproject(-1.0f, -1.0f, 0.0f);
    corners.nearBottomRight = unproject( 1.0f, -1.0f, 0.0f);
    corners.farTopLeft      = unproject(-1.0f,  1.0f, 1.0f);
    corners.farTopRight     = unproject( 1.0f,  1.0f, 1.0f);
    corners.farBottomLeft   = unproject(-1.0f, -1.0f, 1.0f);
    corners.farBottomRight  = unproject( 1.0f, -1.0f, 1.0f);

    return corners;
}
} // namespace nc

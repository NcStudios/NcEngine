#include "ecs/component/SceneNavigationCamera.h"
#include "ecs/Registry.h"
#include "Input.h"

namespace
{
    constexpr auto KeyPan = nc::input::KeyCode::MiddleButton;
    constexpr auto KeyZoom = nc::input::KeyCode::MouseWheel;
    constexpr auto KeyLook = nc::input::KeyCode::RightButton;
    constexpr auto KeySpeed = nc::input::KeyCode::Shift;
}

namespace nc
{
    SceneNavigationCamera::SceneNavigationCamera(Entity entity, const SceneCameraConfig& config)
        : Camera{entity},
          m_config{config}
    {
    }

    void SceneNavigationCamera::Run(Entity self, Registry* registry, float dt)
    {
        auto* transform = registry->Get<Transform>(self);
        auto translation = Zoom(dt);
        if(HandlePanInput())
        {
            translation += Pan(dt);
        }

        transform->TranslateLocalSpace(translation);

        if(HandleLookInput())
        {
            transform->Rotate(Look(dt, transform->Right()));
        }
    }

    auto SceneNavigationCamera::HandlePanInput() -> bool
    {
        if(KeyDown(KeyPan))
        {
            m_panStartPos = input::MousePos();
            m_panUnitsTraveled = 0.0f;
            return false;
        }

        return KeyHeld(KeyPan);
    }

    auto SceneNavigationCamera::HandleLookInput() -> bool
    {
        if(KeyDown(KeyLook))
        {
            m_lookStartPos = input::MousePos();
            return false;
        }

        return KeyHeld(KeyLook);
    }

    auto SceneNavigationCamera::Pan(float dt) -> Vector3
    {
        const auto mousePos = input::MousePos();
        const auto mouseDelta = mousePos - m_panStartPos;
        const auto pixelsTraveled = Magnitude(mouseDelta);
        const auto unitsPerPixel = KeyHeld(KeySpeed) ? (1.0f / 100.0f) : (1.0f / 350.0f);
        const auto maxDistance = pixelsTraveled * unitsPerPixel;

        if(m_panUnitsTraveled > maxDistance)
        {
            m_panStartPos = mousePos;
            m_panUnitsTraveled = 0.0f;
            return Vector3::Zero();
        }

        const auto [xPan, yPan] = mouseDelta * m_config.panDamp * dt;
        m_panUnitsTraveled += std::sqrt(xPan * xPan + yPan * yPan);
        return Vector3{-xPan, yPan, 0.0f};
    }

    auto SceneNavigationCamera::Zoom(float dt) -> Vector3
    {
        auto& zoom = m_zoomVelocity;
        if(const auto wheel = input::MouseWheel(); wheel != 0)
        {
            const auto factor = KeyHeld(input::KeyCode::Shift) ? m_config.zoomSpeedMult : 1.0f;
            zoom += factor * m_config.zoomDamp * static_cast<float>(wheel);
        }

        zoom = math::Lerp(zoom, 0.0f, 0.05f);
        return Vector3{0.0f, 0.0f, zoom * dt};
    }

    auto SceneNavigationCamera::Look(float dt, const Vector3& transformRight) -> Quaternion
    {
        const auto mouseDelta = input::MousePos() - m_lookStartPos;
        const auto [horizontalLook, verticalLook] = mouseDelta * m_config.lookDamp * dt;
        return Multiply(Quaternion::FromAxisAngle(Vector3::Up(), horizontalLook),
                        Quaternion::FromAxisAngle(transformRight, verticalLook));
    }
}
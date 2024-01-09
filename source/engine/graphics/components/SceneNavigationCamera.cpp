#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/input/Input.h"
#include "ncengine/window/Window.h"

#include <iostream>

namespace
{
constexpr auto g_fineUnitsPerPixel = 1.0f / 30.0f;
constexpr auto g_courseUnitsPerPixel = 1.0f / 15.0f;

struct Key
{
    static constexpr auto Pan = nc::input::KeyCode::MiddleButton;
    static constexpr auto Look = nc::input::KeyCode::RightButton;
    static constexpr auto Speed = nc::input::KeyCode::LeftShift;
};

auto IsWithinWindow(const nc::Vector2& pos) -> bool
{
    const auto dimensions = nc::window::GetDimensions();
    return pos.x >= 0.0f &&
           pos.y >= 0.0f &&
           pos.x <= dimensions.x &&
           pos.y <= dimensions.y;
}
} // anonymous namespace

namespace nc::graphics
{
SceneNavigationCamera::SceneNavigationCamera(Entity entity,
                                             const CameraProperties& cameraProperties,
                                             const SceneCameraConfig& config)
    : Camera{entity, cameraProperties},
      m_fineSpeed{config.truckPedestalFine, config.panTiltFine, config.dollyFine},
      m_coarseSpeed{config.truckPedestalCoarse, config.panTiltCoarse, config.dollyCoarse}
{
}

void SceneNavigationCamera::Run(Entity self, Registry* registry, float dt)
{
    const auto useCoarseSpeed = KeyHeld(Key::Speed);
    const auto& [truckPedestalSpeed, panTiltSpeed, dollySpeed] = useCoarseSpeed ? m_coarseSpeed : m_fineSpeed;
    auto* transform = registry->Get<Transform>(self);
    auto translation = Dolly(dt, dollySpeed);

    if(HandlePanInput())
    {
        translation += TruckAndPedestal(dt, truckPedestalSpeed, useCoarseSpeed ? g_courseUnitsPerPixel : g_fineUnitsPerPixel);
    }

    transform->TranslateLocalSpace(translation);

    if(HandleLookInput())
    {
        transform->Rotate(PanAndTilt(dt, panTiltSpeed, transform->Right()));
    }
}

auto SceneNavigationCamera::HandlePanInput() -> bool
{
    if(KeyDown(Key::Pan))
    {
        m_slideReference = input::MousePos();
        m_unitsTraveled = 0.0f;
        return false;
    }

    return KeyHeld(Key::Pan);
}

auto SceneNavigationCamera::HandleLookInput() -> bool
{
    if(KeyDown(Key::Look))
    {
        m_pivotReference = input::MousePos();
        return false;
    }

    return KeyHeld(Key::Look);
}

auto SceneNavigationCamera::TruckAndPedestal(float dt, float speedMult, float unitsPerPixel) -> Vector3
{
    const auto mousePos = input::MousePos();
    if (!::IsWithinWindow(mousePos))
    {
        return Vector3::Zero();
    }

    const auto mouseDelta = (mousePos - m_slideReference);
    if(m_unitsTraveled > Magnitude(mouseDelta) * unitsPerPixel)
    {
        m_slideReference = mousePos;
        m_unitsTraveled = 0.0f;
        return Vector3::Zero();
    }

    auto translation = mouseDelta * unitsPerPixel * speedMult * dt;
    m_unitsTraveled += Magnitude(translation);
    return Vector3{-1.0f * translation.x, translation.y, 0.0f};
}

auto SceneNavigationCamera::PanAndTilt(float dt, float speedMult, const Vector3& tiltAxis) -> Quaternion
{
    const auto [pan, tilt] = (input::MousePos() - m_pivotReference) * speedMult * dt;
    return Multiply(Quaternion::FromAxisAngle(Vector3::Up(), pan),
                    Quaternion::FromAxisAngle(tiltAxis, tilt));
}

auto SceneNavigationCamera::Dolly(float dt, float speedMult) -> Vector3
{
    if(const auto wheel = input::MouseWheel())
    {
        m_dollyVelocity += speedMult * static_cast<float>(wheel);
    }

    m_dollyVelocity = Lerp(m_dollyVelocity, 0.0f, 5.0f * dt);
    return Vector3{0.0f, 0.0f, m_dollyVelocity};
}
} // namespace nc::graphics

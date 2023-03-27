#include "ecs/Registry.h"
#include "input/Input.h"
#include "graphics/SceneNavigationCamera.h"

namespace
{
struct Key
{
    static constexpr auto Pan = nc::input::KeyCode::MiddleButton;
    static constexpr auto Zoom = nc::input::KeyCode::MouseWheel;
    static constexpr auto Look = nc::input::KeyCode::RightButton;
    static constexpr auto Speed = nc::input::KeyCode::LeftShift;
};
} // anonymous namespace

namespace nc::graphics
{
SceneNavigationCamera::SceneNavigationCamera(Entity entity, const SceneCameraConfig& config)
    : Camera{entity},
        m_fineSpeed{config.truckPedestalFine, config.panTiltFine, config.dollyFine},
        m_coarseSpeed{config.truckPedestalCoarse, config.panTiltCoarse, config.dollyCoarse}
{
}

void SceneNavigationCamera::Run(Entity self, Registry* registry, float dt)
{
    const auto& [truckPedestalSpeed, panTiltSpeed, dollySpeed] = KeyHeld(Key::Speed) ? m_coarseSpeed : m_fineSpeed;
    auto* transform = registry->Get<Transform>(self);
    auto translation = Dolly(dt, dollySpeed);

    if(HandlePanInput())
    {
        translation += TruckAndPedestal(dt, truckPedestalSpeed);
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

auto SceneNavigationCamera::TruckAndPedestal(float dt, float speedMult) -> Vector3
{
    constexpr auto unitsPerPixel = 1.0f / 350.0f;
    const auto mousePos = input::MousePos();
    const auto mouseDelta = (mousePos - m_slideReference) * speedMult;

    if(m_unitsTraveled > Magnitude(mouseDelta) * unitsPerPixel)
    {
        m_slideReference = mousePos;
        m_unitsTraveled = 0.0f;
        return Vector3::Zero();
    }

    const auto translation = mouseDelta * dt;
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

    m_dollyVelocity = Lerp(m_dollyVelocity, 0.0f, 0.01f);
    return Vector3{0.0f, 0.0f, m_dollyVelocity * dt};
}
} // namespace nc::graphics

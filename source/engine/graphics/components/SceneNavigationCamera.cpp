#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/input/Input.h"
#include "ncengine/window/Window.h"

#include <iostream>

namespace
{
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


float smoothstep(float edge0, float edge1, float x)
{
    // Scale, bias and saturate x to 0..1 range
    x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Evaluate polynomial
    return x*x*(3 - 2 * x);
}

float smootherstep(float edge0, float edge1, float x) {
  // Scale, and clamp x to 0..1 range
  x = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);

  return x * x * x * (x * (6.0f * x - 15.0f) + 10.0f);
}

constexpr auto Lerp2(const Vector2& from, const Vector2& to, float factor)
{
    return (1.0f - factor) * from + factor * to;
}

constexpr auto Lerp3(const Vector3& from, const Vector3& to, float factor)
{
    return (1.0f - factor) * from + factor * to;
}

constexpr auto SmootherStep(const Vector2& from, const Vector2& to, float factor)
{
    return Vector2{smoothstep(from.x, to.x, factor), smoothstep(from.y, to.y, factor)};
}

void SceneNavigationCamera::Run(Entity self, Registry* registry, float dt)
{
    const auto useCoarseSpeed = KeyHeld(Key::Speed);
    const auto& [truckPedestalSpeed, panTiltSpeed, dollySpeed] = useCoarseSpeed ? m_coarseSpeed : m_fineSpeed;
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
        m_prevMousePos = input::MousePos();
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
    const auto mousePos = input::MousePos();
    if (!::IsWithinWindow(mousePos))
    {
        return Vector3::Zero();
    }

    auto delta = (m_prevMousePos - mousePos);
    delta.y *= -1.0f;
    m_prevMousePos = mousePos;
    return Normalize(Vector3{delta.x, delta.y, 0.0f}) * dt * speedMult * Magnitude(delta);
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

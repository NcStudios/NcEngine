#include "EdgePanCamera.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "Input.h"
#include "Window.h"

namespace
{
    const float ZoomSpeed = -0.9f;
    const float PanSpeed = 10.0f;
    const float CamRotateSpeed = 0.05f;
    const unsigned EdgePanWidth = 50u;
    const unsigned HudHeight = 200u;

    float GetPan(float val, float min, float max)
    {
        if(val < min) return -1.0f;
        if(val > max) return 1.0f;
        return 0.0f;
    };
}

namespace nc::sample
{
    EdgePanCamera::EdgePanCamera(Entity entity)
        : Camera(entity),
          m_lastFrameZoom{0.0f}
    {}

    void EdgePanCamera::FrameUpdate(float dt)
    {
        auto [screenWidth, screenHeight] = window::GetDimensions();
        auto [x, y] = input::MousePos();
        auto xPan = GetPan(x, EdgePanWidth, screenWidth - EdgePanWidth);
        auto yPan = -1.0f * GetPan(y, EdgePanWidth, screenHeight - EdgePanWidth - HudHeight);
        auto pan = Normalize(Vector2{xPan, yPan}) * PanSpeed;
        auto zoom = (float)input::MouseWheel() * ZoomSpeed;
        auto translation = math::Lerp(m_lastFrameTranslation, Vector3{pan.x, zoom, pan.y}, 0.1f);
        if(translation == Vector3::Zero()) // b/c float equality
            translation = Vector3::Zero();
        m_lastFrameTranslation = translation;
        translation = translation * dt;
        ActiveRegistry()->Get<Transform>(GetParentEntity())->Translate(translation);
    }
}
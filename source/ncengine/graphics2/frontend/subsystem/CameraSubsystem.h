#pragma once

#include "ncengine/ecs/EcsFwd.h"

namespace nc
{
class Transform;

namespace config
{
struct GraphicsSettings;
}

namespace graphics
{
class Camera;
struct CameraRenderState;

class CameraSubsystem final
{
    public:
        explicit CameraSubsystem(const config::GraphicsSettings& graphicsSettings);

        void Set(Camera* camera) noexcept
        {
            m_mainCamera = camera;
        }

        auto Get() noexcept -> Camera*
        {
            return m_mainCamera;
        }

        void Clear() noexcept
        {
            m_mainCamera = nullptr;
        }

        auto BuildState(ecs::ExplicitEcs<Transform> ecs) -> CameraRenderState;

    private:
        Camera* m_mainCamera = nullptr;
        bool m_isRightHanded;
};
} // namespace graphics
} // namespace nc

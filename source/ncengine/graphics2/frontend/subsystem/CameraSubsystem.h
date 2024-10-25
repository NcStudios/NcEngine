#pragma once

#include "ncengine/ecs/EcsFwd.h"

namespace nc
{
class Transform;

namespace graphics
{
class Camera;
struct CameraRenderState;

class CameraSubsystem final
{
    public:
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
};
} // namespace graphics
} // namespace nc

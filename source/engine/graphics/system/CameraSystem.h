#pragma once

#include "DirectXMath.h"
#include "ncmath/Geometry.h"
#include "ncmath/Vector.h"

namespace nc
{
class Registry;

namespace graphics
{
class Camera;

struct CameraFrontendState
{
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
    Vector3 position;
    Frustum frustum;
    bool hasCamera = false;
};

class CameraSystem final
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

        auto Execute(Registry* registry) -> CameraFrontendState;

    private:
        Camera* m_mainCamera = nullptr;
};
} // namespace graphics
} // namespace nc

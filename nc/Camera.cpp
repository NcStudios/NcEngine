#include "Camera.h"

namespace nc
{
    DirectX::XMMATRIX Camera::GetMatrix() const noexcept
    {
        const auto zero = DirectX::XMVectorZero();
        const auto up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        const auto pos = DirectX::XMVectorSet(0.0f, 0.0f, -zPos, 0.0f);
        const auto rot = DirectX::XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f);
        const auto transformedPos = DirectX::XMVector3Transform(pos, rot);

        return DirectX::XMMatrixLookAtLH(transformedPos, zero, up) *
               DirectX::XMMatrixRotationRollPitchYaw(xRot, -yRot, zRot);
    }
}
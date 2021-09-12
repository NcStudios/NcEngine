#pragma once

#include "directx/math/DirectXMath.h"

namespace nc
{
    inline float GetMaxScaleExtent(DirectX::FXMMATRIX matrix)
    {
        auto xExtent_v = DirectX::XMVector3Dot(matrix.r[0], matrix.r[0]);
        auto yExtent_v = DirectX::XMVector3Dot(matrix.r[1], matrix.r[1]);
        auto zExtent_v = DirectX::XMVector3Dot(matrix.r[2], matrix.r[2]);
        auto maxExtent_v = DirectX::XMVectorMax(xExtent_v, DirectX::XMVectorMax(yExtent_v, zExtent_v));
        return sqrt(DirectX::XMVectorGetX(maxExtent_v));
    }
}
#pragma once

#include "DirectXMath.h"

#include <vector>

namespace nc::graphics
{
struct ObjectRenderState
{
    std::vector<DirectX::XMMATRIX> modelMatrices;
};
} // namespace nc::graphics

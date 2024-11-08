#pragma once

#include "ncmath/Vector.h"

#include "DirectXMath.h"

namespace nc::graphics
{
auto TransposeIfRequired(const DirectX::XMMATRIX& modelMatrix, bool requiresTranspose) -> DirectX::XMMATRIX;
} // namespace nc::graphics

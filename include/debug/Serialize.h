#pragma once

#include "math/Vector.h"
#include "DirectXMath.h"

#include <iostream>

namespace nc
{
    std::ostream& operator << (std::ostream& os, const Vector2& v);
    std::ostream& operator << (std::ostream& os, const Vector3& v);
    std::ostream& operator << (std::ostream& os, const Vector4& v);
    std::ostream& operator << (std::ostream& os, DirectX::FXMMATRIX matrix);
    std::ostream& operator << (std::ostream& os, DirectX::XMVECTOR vector);
} // namespace nc
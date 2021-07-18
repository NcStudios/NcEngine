#include "debug/Serialize.h"

namespace nc
{
    std::ostream& operator << (std::ostream& os, const Vector2& v)
    {
        os << v.x << ", " << v.y;
        return os;
    }

    std::ostream& operator << (std::ostream& os, const Vector3& v)
    {
        os << v.x << ", " << v.y << ", " << v.z;
        return os;
    }

    std::ostream& operator << (std::ostream& os, const Vector4& v)
    {
        os << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
        return os;
    }

    std::ostream& operator << (std::ostream& os, DirectX::FXMMATRIX matrix)
    {
        DirectX::XMFLOAT4X4 m;
        DirectX::XMStoreFloat4x4(&m, matrix);

        os << m._11 << ", " << m._12 << ", " << m._13 << ", " << m._14 << '\n'
           << m._21 << ", " << m._22 << ", " << m._23 << ", " << m._24 << '\n'
           << m._31 << ", " << m._32 << ", " << m._33 << ", " << m._34 << '\n'
           << m._41 << ", " << m._42 << ", " << m._43 << ", " << m._44 << '\n';
        return os;
    }
} // namespace nc
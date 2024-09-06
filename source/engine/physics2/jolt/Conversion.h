#pragma once

#include "Layers.h"
#include "ncengine/physics/RigidBody.h"

#include "DirectXMath.h"


#include <type_traits>

namespace nc::physics
{
inline auto ToJoltVec3(const nc::Vector3& in) -> JPH::Vec3
{
    return JPH::Vec3{reinterpret_cast<const JPH::Float3&>(in)};
}

inline auto ToJoltVec3(DirectX::FXMVECTOR in) -> JPH::Vec3
{
#ifdef JPH_FLOATING_POINT_EXCEPTIONS_ENABLED
    return JPH::Vec3{JPH::Vec3::sFixW(in)};
#else
    return JPH::Vec3{in};
#endif
}

inline auto ToJoltQuaternion(const nc::Quaternion& in) -> JPH::Quat
{
    return JPH::Quat{in.x, in.y, in.z, in.w};
}

inline auto ToJoltQuaternion(DirectX::FXMVECTOR in) -> JPH::Quat
{
    return JPH::Quat{JPH::Vec4{in}};
}

inline auto ToXMVector(const JPH::Vec3& in) -> DirectX::XMVECTOR
{
    return DirectX::XMVectorSetW(in.mValue, 0.0f);
}

inline auto ToXMVectorHomogeneous(const JPH::Vec3& in) -> DirectX::XMVECTOR
{
    return DirectX::XMVectorSetW(in.mValue, 1.0f);
}

inline auto ToXMQuaternion(const JPH::Quat& in) -> DirectX::XMVECTOR
{
    return in.mValue.mValue;
}

inline auto ToVector3(const JPH::Vec3& in) -> Vector3
{
    return Vector3{in.GetX(), in.GetY(), in.GetZ()};
}

inline auto ToQuaternion(const JPH::Quat& in) -> Quaternion
{
    return Quaternion{in.GetX(), in.GetY(), in.GetZ(), in.GetW()};
}

inline auto ToMotionType(BodyType bodyType) -> JPH::EMotionType
{
    switch (bodyType)
    {
        case BodyType::Dynamic:   return JPH::EMotionType::Dynamic;
        case BodyType::Static:    return JPH::EMotionType::Static;
        case BodyType::Kinematic: return JPH::EMotionType::Kinematic;
        default: std::unreachable();
    }
}

inline auto ToObjectLayer(BodyType bodyType) -> JPH::ObjectLayer
{
    switch (bodyType)
    {
        case BodyType::Dynamic:   [[fallthrough]];
        case BodyType::Kinematic: return ObjectLayer::Dynamic;
        case BodyType::Static:    return ObjectLayer::Static;
        default: std::unreachable();
    }
}
} // namespace nc::physics

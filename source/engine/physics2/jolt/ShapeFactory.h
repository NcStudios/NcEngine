#pragma once

#include "ncengine/physics/Shape.h"

#include "Jolt/Jolt.h"
#include "Jolt/Core/Reference.h"
#include "Jolt/Math/Vec3.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"

#include <string_view>
#include <unordered_map>

namespace nc::physics
{
class ShapeFactory
{
    static constexpr auto boxConvexRadius = 0.05f;

    public:
        auto MakeShape(const Shape& shape, const JPH::Vec3& additionalScaling) -> JPH::Ref<JPH::Shape>;

        auto MakeBox(const JPH::Vec3& halfExtents, const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>;
        auto MakeSphere(float radius, const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>;
        auto MakeCapsule(float halfHeight, float radius, const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>;
        auto MakeConvexHull(std::string_view asset, const JPH::Vec3& worldScale) -> JPH::Ref<JPH::Shape>;

        /** @todo 712 remove this function + map */
        void AddConvexHull(std::string_view asset, JPH::Ref<JPH::Shape> shape);

    private:
        std::unordered_map<std::string, JPH::Ref<JPH::Shape>> m_convexHulls;

        template<class T, class... Args>
        auto MakeRef(Args&&... args) -> JPH::Ref<JPH::Shape>
        {
            return JPH::Ref<JPH::Shape>{new T(std::forward<Args>(args)...)};
        }

        auto ApplyLocalOffsets(const JPH::Ref<JPH::Shape>& shape, const JPH::Vec3& localPosition) -> JPH::Ref<JPH::Shape>;
};
} // namespace nc::physics

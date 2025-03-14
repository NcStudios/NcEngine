#include "ncengine/physics/CompoundShape.h"
#include "Conversion.h"
#include "CookedShapeUtility.h"
#include "ShapeFactory.h"

#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/ConvexHullShape.h"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"
#include "Jolt/Physics/Collision/Shape/MutableCompoundShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/StaticCompoundShape.h"

namespace
{
void AddShape(JPH::CompoundShapeSettings& settings,
              const nc::SubShapeInfo& info)
{
    using namespace nc::physics;
    using enum nc::ShapeType;
    const auto position = ToJoltVec3(info.position);
    const auto rotation = ToJoltQuaternion(info.rotation);
    const auto scale = ToJoltVec3(info.shape.GetLocalScale());
    switch (const auto type = info.shape.GetType())
    {
        case Box:
        {
            auto innerShape = new JPH::BoxShapeSettings(scale * 0.5f);
            settings.AddShape(position, rotation, innerShape, info.userData);
            break;
        }
        case Sphere:
        {
            auto innerShape = new JPH::SphereShapeSettings(scale.GetX() * 0.5f);
            settings.AddShape(position, rotation, innerShape, info.userData);
            break;
        }
        case Capsule:
        {
            auto innerShape = new JPH::CapsuleShapeSettings(scale.GetY(), scale.GetX() * 0.5f);
            settings.AddShape(position, rotation, innerShape, info.userData);
            break;
        }
        case ConvexHull:
        {
            auto innerShape = ShapeFactory::Instance()->MakeConvexHull(info.shape.GetAssetId(), scale);
            settings.AddShape(position, rotation, innerShape.GetPtr(), info.userData);
            break;
        }
        case Mesh:
        {
            const auto innerShape = ShapeFactory::Instance()->MakeMesh(info.shape.GetAssetId(), scale);
            settings.AddShape(position, rotation, innerShape.GetPtr(), info.userData);
            break;
        }
        default:
            throw nc::NcError{fmt::format(
                "CompoundShape does not support ShapeType '{}'",
                std::to_underlying(type)
            )};
    }
}

void CopyHull(JPH::CompoundShapeSettings& settings,
              nc::asset::AssetId assetId,
              const JPH::Vec3& position,
              const JPH::Quat& rotation,
              const JPH::Vec3& scale,
              uint32_t userData)
{
    const auto targetBase = nc::physics::ShapeFactory::Instance()->GetConvexHull(assetId);
    const auto targetHull = static_cast<const JPH::ConvexHullShape*>(targetBase);
    const auto numPoints = targetHull->GetNumPoints();
    auto points = JPH::Array<JPH::Vec3>{};
    points.reserve(numPoints);
    for (auto i = 0u; i < numPoints; ++i)
    {
        points.push_back(scale * targetHull->GetPoint(i));
    }

    auto innerShape = new JPH::ConvexHullShapeSettings(std::move(points));
    settings.AddShape(position, rotation, innerShape, userData);
}

void CopyShape(JPH::CompoundShapeSettings& settings,
               const nc::SubShapeInfo& info)
{
    using namespace nc::physics;
    using enum nc::ShapeType;
    const auto type = info.shape.GetType();
    if (type <= Capsule)
    {
        AddShape(settings, info);
        return;
    }

    const auto position = ToJoltVec3(info.position);
    const auto rotation = ToJoltQuaternion(info.rotation);
    const auto scale = ToJoltVec3(info.shape.GetLocalScale());
    switch (type)
    {
        case ConvexHull:
        {
            CopyHull(settings, info.shape.GetAssetId(), position, rotation, scale, info.userData);
            break;
        }
        default:
            throw nc::NcError{fmt::format(
                "Static CompoundShape does not support ShapeType '{}'",
                std::to_underlying(type)
            )};
    }
}

auto Cook(const JPH::CompoundShapeSettings& settings) -> nc::CookedShape
{
    auto result = settings.Create();
    if (result.HasError())
    {
        throw nc::NcError{fmt::format("Failed to create CompoundShape: '{}'", result.GetError())};
    }

    auto storage = nc::CookedShape::ShapeStorage{};
    nc::ShapeStorageRTTI::Construct(storage, result.Get());
    return nc::CookedShape{storage};
}
} // anonymous namespace

namespace nc
{
auto CreateMutableCompoundShape(std::span<const SubShapeInfo> shapes) -> CookedShape
{
    NC_ASSERT(shapes.size() > 1, "At least two subshapes are required for a CompoundShape");
    auto settings = JPH::MutableCompoundShapeSettings{};
    settings.mSubShapes.reserve(shapes.size());
    for (const auto& info : shapes)
    {
        AddShape(settings, info);
    }

    return Cook(settings);
}

auto CreateStaticCompoundShape(std::span<const SubShapeInfo> shapes) -> CookedShape
{
    NC_ASSERT(shapes.size() > 1, "At least two subshapes are required for a CompoundShape");
    auto settings = JPH::StaticCompoundShapeSettings{};
    settings.mSubShapes.reserve(shapes.size());
    for (const auto& info : shapes)
    {
        CopyShape(settings, info);
    }

    return Cook(settings);
}

class CompoundShapeBuilder::Impl
{
    public:
        explicit Impl(CookedShape& shape)
        {
            auto& internalShape = ShapeStorageRTTI::ToShape(shape.GetShapeData());
            NC_ASSERT(
                internalShape->GetSubType() == JPH::EShapeSubType::MutableCompound,
                "CompoundShapeBuilder requires an untransformed mutable CompoundShape"
            );

            m_shape = static_cast<JPH::MutableCompoundShape*>(internalShape.GetPtr());
        }

        auto GetCompoundShape()                -> decltype(auto) { return (*m_shape); }
        auto GetCompoundShape()          const -> decltype(auto) { return (*m_shape); }
        auto GetSubShape(uint32_t index) const -> decltype(auto) { return (m_shape->GetSubShape(index)); }

    private:
        JPH::Ref<JPH::MutableCompoundShape> m_shape;
};

CompoundShapeBuilder::CompoundShapeBuilder(CookedShape& shape)
    : m_impl{std::make_unique<Impl>(shape)}
{
}

CompoundShapeBuilder::~CompoundShapeBuilder() noexcept = default;

auto CompoundShapeBuilder::GetSubShapeCount() const -> uint32_t
{
    return m_impl->GetCompoundShape().GetNumSubShapes();
}

auto CompoundShapeBuilder::GetSubShapeIndex(uint32_t userData) const -> SubShapeIndex
{
    const auto& shape = m_impl->GetCompoundShape();
    const auto numSubShapes = shape.GetNumSubShapes();
    for (auto i = 0u; i < numSubShapes; ++i)
    {
        const auto& subShape = shape.GetSubShape(i);
        if (subShape.mUserData == userData)
        {
            return i;
        }
    }

    throw NcError("SubShape not found");
}

auto CompoundShapeBuilder::GetSubShapePosition(SubShapeIndex index) const -> Vector3
{
    const auto baseCOM = m_impl->GetCompoundShape().GetCenterOfMass();
    const auto subShapeCOM = m_impl->GetSubShape(index).GetPositionCOM();
    return physics::ToVector3(baseCOM + subShapeCOM);
}

auto CompoundShapeBuilder::GetSubShapeRotation(SubShapeIndex index) const -> Quaternion
{
    return physics::ToQuaternion(m_impl->GetSubShape(index).GetRotation());
}

auto CompoundShapeBuilder::AddSubShape(const SubShapeInfo& info) -> SubShapeIndex
{
    return m_impl->GetCompoundShape().AddShape(
        physics::ToJoltVec3(info.position),
        physics::ToJoltQuaternion(info.rotation),
        physics::ShapeFactory::Instance()->MakeShape(info.shape, JPH::Vec3::sOne()),
        info.userData
    );
}

void CompoundShapeBuilder::RemoveSubShape(SubShapeIndex index)
{
    m_impl->GetCompoundShape().RemoveShape(index);
}

void CompoundShapeBuilder::ModifySubShape(SubShapeIndex index,
                                          const Vector3& position,
                                          const Quaternion& rotation)
{
    m_impl->GetCompoundShape().ModifyShape(
        index,
        physics::ToJoltVec3(position),
        physics::ToJoltQuaternion(rotation)
    );
}

void CompoundShapeBuilder::ReplaceSubShape(SubShapeIndex index,
                                           const SubShapeInfo& info)
{
    const auto innerShape = physics::ShapeFactory::Instance()->MakeShape(
        info.shape,
        JPH::Vec3::sOne()
    );

    m_impl->GetCompoundShape().ModifyShape(
        index,
        physics::ToJoltVec3(info.position),
        physics::ToJoltQuaternion(info.rotation),
        innerShape.GetPtr()
    );
}

void CompoundShapeBuilder::RecalculateCenterOfMass()
{
    m_impl->GetCompoundShape().AdjustCenterOfMass();
}
} // namespace nc

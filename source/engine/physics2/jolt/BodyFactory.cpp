#include "BodyFactory.h"
#include "Conversion.h"
#include "ShapeFactory.h"
#include "ncengine/physics/RigidBody.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyInterface.h"

namespace nc::physics
{
BodyFactory::BodyFactory(JPH::BodyInterface& bodyInterface,
                         ShapeFactory& shapeFactory)
    : m_interface{&bodyInterface},
      m_shapeFactory{&shapeFactory}
{
}

auto BodyFactory::MakeBody(const RigidBody& rigidBody, DirectX::FXMMATRIX transform) -> BodyResult
{
    const auto [initScale, initRotation, initPosition] = DecomposeMatrix(transform);
    const auto& shape = rigidBody.GetShape();
    const auto bodyType = rigidBody.GetBodyType();
    auto allowedScaling = Vector3::One();
    auto wasScaleAdjusted = false;
    if (rigidBody.ScalesWithTransform())
    {
        const auto currentScale = nc::ToVector3(initScale);
        allowedScaling = NormalizeScaleForShape(shape.GetType(), currentScale, currentScale);
        if (allowedScaling != currentScale)
        {
            wasScaleAdjusted = true;
        }
    }

    auto bodySettings = JPH::BodyCreationSettings{
        m_shapeFactory->MakeShape(shape, ToJoltVec3(allowedScaling)),
        ToJoltVec3(initPosition),
        ToJoltQuaternion(initRotation),
        ToMotionType(bodyType),
        ToObjectLayer(bodyType, rigidBody.IsTrigger())
    };

    const auto entity = rigidBody.GetEntity();
    bodySettings.mUserData = Entity::Hash{}(entity);
    bodySettings.mAllowedDOFs = ToAllowedDOFs(rigidBody.GetDegreesOfFreedom());
    bodySettings.mAllowDynamicOrKinematic = !entity.IsStatic(); // skip creating MotionProperties for static entities
    bodySettings.mIsSensor = rigidBody.IsTrigger();
    bodySettings.mMotionQuality = ToMotionQuality(rigidBody.UseContinuousDetection());
    bodySettings.mFriction = rigidBody.GetFriction();
    bodySettings.mRestitution = rigidBody.GetRestitution();
    bodySettings.mLinearDamping = rigidBody.GetLinearDamping();
    bodySettings.mAngularDamping = rigidBody.GetAngularDamping();
    bodySettings.mGravityFactor = rigidBody.GetGravityMultiplier();

    return BodyResult{
        m_interface->CreateBody(bodySettings),
        allowedScaling,
        wasScaleAdjusted
    };
}
} // namespace nc::physics

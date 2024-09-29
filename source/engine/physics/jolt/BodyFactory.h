#pragma once

#include "ncengine/utility/MatrixUtilities.h"

namespace JPH
{
class Body;
class BodyInterface;
} // namespace JPH

namespace nc::physics
{
class ShapeFactory;
class RigidBody;

struct BodyResult
{
    JPH::Body* body = nullptr;
    Vector3 adjustedScale = Vector3::One();
    bool wasScaleAdjusted = false;
};

class BodyFactory
{
    public:
        explicit BodyFactory(JPH::BodyInterface& bodyInterface,
                             ShapeFactory& shapeFactory);

        auto MakeBody(const RigidBody& rigidBody,
                      DirectX::FXMMATRIX transform) -> BodyResult;

    private:
        JPH::BodyInterface* m_interface;
        ShapeFactory* m_shapeFactory;
};
} // namespace nc::physics

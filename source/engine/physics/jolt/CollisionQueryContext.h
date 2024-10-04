#pragma once

namespace JPH
{
class BodyInterface;
class BodyLockInterfaceNoLock;
class NarrowPhaseQuery;
} // namespapce JPH

namespace nc::physics
{
class ShapeFactory;

// State needed by CollisionQuery
struct CollisionQueryContext
{
    const JPH::NarrowPhaseQuery& query;
    const JPH::BodyLockInterfaceNoLock& lock;
    ShapeFactory& shapeFactory;
};
} // namespace nc::physics

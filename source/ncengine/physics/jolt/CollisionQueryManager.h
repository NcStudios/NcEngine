#pragma once

#include "CollisionQueryContext.h"
#include "physics/CollisionQueryImpl.h"

#include "ncengine/type/StableAddress.h"

namespace nc::physics
{
// Simple manager for CollisionQuery - just owns and sets the context.
class CollisionQueryManager : public StableAddress
{
    public:
        explicit CollisionQueryManager(const JPH::NarrowPhaseQuery& query,
                                       const JPH::BodyLockInterfaceNoLock& lock,
                                       ShapeFactory& shapeFactory)
            : m_ctx{query, lock, shapeFactory}
        {
            CollisionQueryImpl::SetContext(&m_ctx);
        }

    private:
        CollisionQueryContext m_ctx;
};
} // namespace nc::physics

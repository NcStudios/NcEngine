#include "gtest/gtest.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/RigidBody.h"
#include "physics/jolt/ConstraintManager.h"
#include "physics/jolt/Profiler.inl"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/Body.h"

auto s_lastUpdateConstraintTarget = std::pair<nc::Entity, JPH::Body*>{};

auto ClearParams()
{
    s_lastUpdateConstraintTarget = std::pair{nc::Entity::Null(), nullptr};
}

namespace nc::physics
{
void ConstraintManager::EnableConstraint(Constraint&, bool) {}
void ConstraintManager::UpdateConstraint(Constraint&) {}
void ConstraintManager::UpdateConstraintTarget(Constraint&,
                                               Entity target,
                                               JPH::Body* targetBody)
{
    s_lastUpdateConstraintTarget = std::pair{target, targetBody};
}
} // namespace nc::physics

TEST(ConstraintsTest, GetType_succeeds)
{
    auto uut = nc::Constraint{
        nc::ConstraintInfo{}, // default constructs first type (FixedConstraint)
        nc::Entity{},
        0u
    };

    EXPECT_EQ(nc::ConstraintType::FixedConstraint, uut.GetType());

    uut.GetInfo() = nc::FixedConstraintInfo{};
    EXPECT_EQ(nc::ConstraintType::FixedConstraint, uut.GetType());

    uut.GetInfo() = nc::PointConstraintInfo{};
    EXPECT_EQ(nc::ConstraintType::PointConstraint, uut.GetType());
}

TEST(ConstraintsTest, SetConstraintTarget_validTarget_passesTargetArgs)
{
    auto uut = nc::Constraint{
        nc::FixedConstraintInfo{},
        nc::Entity{},
        0u
    };

    auto expectedEntity = nc::Entity{1, 0, 0};
    auto expectedJoltBody = static_cast<void*>(&expectedEntity); // anything works, stub won't dereference
    auto newTarget = nc::RigidBody{expectedEntity};
    newTarget.SetHandle(expectedJoltBody);

    uut.SetConstraintTarget(&newTarget);
    const auto& [actualEntity, actualJoltBody] = s_lastUpdateConstraintTarget;
    EXPECT_EQ(expectedEntity, actualEntity);
    EXPECT_EQ(expectedJoltBody, actualJoltBody);

    ClearParams();
}

TEST(ConstraintsTest, SetConstraintTarget_nullTarget_passesDummyArgs)
{
    auto uut = nc::Constraint{
        nc::FixedConstraintInfo{},
        nc::Entity{},
        0u
    };

    uut.SetConstraintTarget(nullptr);
    const auto& [actualEntity, actualJoltBody] = s_lastUpdateConstraintTarget;
    EXPECT_EQ(nc::Entity::Null(), actualEntity);
    EXPECT_EQ(&JPH::Body::sFixedToWorld, actualJoltBody);

    ClearParams();
}

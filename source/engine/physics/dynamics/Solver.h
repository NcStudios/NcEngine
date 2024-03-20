#pragma once

#include "Constraint.h"
#include "FreedomSolver.h"
#include "Joint.h"
#include "physics/PhysicsPipelineTypes.h"

namespace nc::physics
{
class Solver
{
    public:
        Solver(Registry* registry);

        /** Create constraints for VelocityRestriction, PositionClamp, and OrientationClamp components. */
        void GenerateFreedomConstraints(float dt);

        /** Create contact constraints for every contact point in each manifold. Also creates
        *  position constraints for each manifold if EnableDirectPositionCorrection == true. */
        void GenerateContactConstraints(std::span<const Manifold> manifolds);

        /** Resolve contact, position, and joint constraints. For contacts and joints, linear
         *  and angular velocities will be updated, but positions must be integrated separately.
         *  Position constraints will update transforms directly. */
        void ResolveConstraints(std::span<Joint> joints, float dt);

        auto ContactConstraints() const -> std::span<const ContactConstraint> { return m_contactConstraints; }
        auto PositionConstraints() const -> std::span<const PositionConstraint> { return m_positionConstraints; }

    private:
        Registry* m_registry;
        std::vector<ContactConstraint> m_contactConstraints;
        std::vector<PositionConstraint> m_positionConstraints;
        std::vector<PositionClampConstraint> m_positionClampConstraints;
        std::vector<VelocityRestrictionConstraint> m_velocityRestrictionConstraints;
};
} // namespace nc::physics

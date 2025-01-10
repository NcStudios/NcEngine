#pragma once

#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/Vehicle.h"

namespace JPH
{
template<class T>
class Ref;

class Body;
class Constraint;
class PhysicsSystem;
class VehicleConstraint;
class VehicleConstraintSettings;
class VehicleEngineSettings;
class VehicleTransmissionSettings;
class WheeledVehicleController;
} // namespace JPH

namespace nc::physics
{
class ConstraintFactory
{
    public:
        explicit ConstraintFactory(JPH::PhysicsSystem& physicsSystem)
            : m_physicsSystem{&physicsSystem}
        {
        }

        auto MakeConstraint(const ConstraintInfo& createInfo,
                            JPH::Body& first,
                            JPH::Body& second) -> JPH::Constraint*;

        auto MakeVehicleConstraint(VehicleInfo& createInfo,
                                   JPH::Body& owner) -> JPH::Ref<JPH::VehicleConstraint>;

    private:
        JPH::PhysicsSystem* m_physicsSystem;
};

void SetVehicleOrientation(const VehicleOrientation& in,
                           JPH::VehicleConstraintSettings& out);
void SetVehicleEngineSettings(const VehicleEngine& in,
                              JPH::VehicleEngineSettings& out);
void SetVehicleTransmissionSettings(const VehicleTransmission& in,
                                    JPH::VehicleTransmissionSettings& out);
void AddWheelAssembly(WheelAssembly& assembly,
                      JPH::VehicleConstraint& constraint,
                      JPH::WheeledVehicleController& controller);
void RemoveWheelAssembly(size_t assemblyIndex,
                         std::vector<WheelAssembly>& assemblies,
                         JPH::VehicleConstraint& constraint,
                         JPH::WheeledVehicleController& controller);
void ModifyWheelAssembly(const WheelAssembly& assembly,
                         JPH::VehicleConstraint& constraint,
                         JPH::WheeledVehicleController& controller);
} // namespace nc::physics

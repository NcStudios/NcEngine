#pragma once

#include "ncengine/physics/Vehicle.h"
#include "ncengine/utility/SparseMap.h"

#include <vector>

namespace JPH
{
template<class T>
class Ref;

class Body;
class Constraint;
class PhysicsSystem;
class VehicleConstraint;
} // namespace JPH

namespace nc::physics
{
class ConstraintFactory;

class VehicleManager
{
    public:
        static constexpr auto ConstraintMapSizeHint = 1000u;

        explicit VehicleManager(JPH::PhysicsSystem& physicsSystem,
                                ConstraintFactory& constraintFactory,
                                uint32_t maxEntities);
        ~VehicleManager() noexcept;

        auto AddVehicle(VehicleInfo&& createInfo,
                        Entity owner,
                        JPH::Body& ownerBody) -> Vehicle&;
        auto RemoveVehicle(Entity owner) -> bool;
        auto GetVehicles() const -> std::span<const std::unique_ptr<Vehicle>>;
        auto GetVehicle(Entity owner) -> Vehicle*;
        auto GetVehicleConstraint(VehicleId vehicleId) -> JPH::VehicleConstraint*;

        void Clear();
        auto BeginBatch() -> size_t;
        void EndBatch(size_t batchBegin);

    private:
        JPH::PhysicsSystem* m_physicsSystem;
        ConstraintFactory* m_factory;
        std::vector<JPH::Ref<JPH::VehicleConstraint>> m_handles;
        std::vector<uint32_t> m_freeIndices;
        sparse_map<std::unique_ptr<Vehicle>> m_entityToVehicle;
        bool m_isBatchInProgress = false;
};
} // namespace nc::physics

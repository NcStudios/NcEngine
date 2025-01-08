#include "VehicleManager.h"
#include "ConstraintFactory.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Vehicle/WheeledVehicleController.h"
#include "Jolt/Physics/Vehicle/VehicleConstraint.h"

#include <ranges>

namespace
{
void AddToSimulation(JPH::PhysicsSystem* physicsSystem,
                     JPH::VehicleConstraint* constraint)
{
    physicsSystem->AddConstraint(constraint);
    physicsSystem->AddStepListener(constraint);
}

void RemoveFromSimulation(JPH::PhysicsSystem* physicsSystem,
                          JPH::VehicleConstraint* constraint)
{
    physicsSystem->RemoveStepListener(constraint);
    physicsSystem->RemoveConstraint(constraint);
}
} // anonymous namesapce

namespace nc::physics
{
VehicleManager::VehicleManager(JPH::PhysicsSystem& physicsSystem,
                               ConstraintFactory& constraintFactory,
                               uint32_t maxEntities)
    : m_physicsSystem{&physicsSystem},
      m_factory{&constraintFactory},
      m_entityToVehicle{std::min(ConstraintMapSizeHint, maxEntities), maxEntities}
{
}

VehicleManager::~VehicleManager() noexcept = default;

auto VehicleManager::AddVehicle(VehicleInfo&& createInfo,
                                Entity owner,
                                JPH::Body& ownerBody) -> Vehicle&
{
    NC_ASSERT(!GetVehicle(owner), "RigidBody already has a Vehicle");
    auto vehicle = m_factory->MakeVehicleConstraint(createInfo, ownerBody); // careful, this gets moved
    auto vehiclePtr = vehicle.GetPtr();
    if (!m_isBatchInProgress)
    {
        AddToSimulation(m_physicsSystem, vehiclePtr);
    }

    const auto index = [&]()
    {
        if (m_freeIndices.empty() || m_isBatchInProgress)
        {
            const auto i = static_cast<uint32_t>(m_handles.size());
            m_handles.push_back(std::move(vehicle));
            return i;
        }

        const auto i = m_freeIndices.back();
        m_freeIndices.pop_back();
        m_handles[i] = std::move(vehicle);
        return i;
    }();

    const auto ownerId = owner.Index();
    auto& state = m_entityToVehicle.contains(ownerId)
        ? m_entityToVehicle.at(ownerId)
        : m_entityToVehicle.emplace(ownerId, {});

    state = std::make_unique<Vehicle>(std::move(createInfo), vehiclePtr, index);
    return *state;
}

auto VehicleManager::RemoveVehicle(Entity owner) -> bool
{
    const auto ownerId = owner.Index();
    if (!m_entityToVehicle.contains(ownerId))
    {
        return false;
    }

    auto& vehicle = m_entityToVehicle.at(ownerId);
    const auto index = vehicle->GetId();
    const auto handle = std::exchange(m_handles[index], nullptr);
    RemoveFromSimulation(m_physicsSystem, handle.GetPtr());
    m_entityToVehicle.erase(ownerId);
    return true;
}

auto VehicleManager::GetVehicles() const -> std::span<const std::unique_ptr<Vehicle>>
{
    return m_entityToVehicle.values();
}

auto VehicleManager::GetVehicle(Entity owner) -> Vehicle*
{
    const auto id = owner.Index();
    if (m_entityToVehicle.contains(id))
    {
        return m_entityToVehicle.at(id).get();
    }

    return nullptr;
}

auto VehicleManager::GetVehicleConstraint(VehicleId vehicleId) -> JPH::VehicleConstraint*
{
    NC_ASSERT(vehicleId < m_handles.size() && m_handles[vehicleId], "Bad VehicleId");
    return m_handles[vehicleId];
}

void VehicleManager::Clear()
{
    const auto vehiclesBeg = m_handles.begin();
    const auto vehiclesEnd = std::remove(vehiclesBeg, m_handles.end(), JPH::Ref<JPH::VehicleConstraint>{});
    for (auto cur = vehiclesBeg; cur != vehiclesEnd; ++cur)
    {
        RemoveFromSimulation(m_physicsSystem, cur->GetPtr());
    }

    m_handles.clear();
    m_handles.shrink_to_fit();
    m_freeIndices.clear();
    m_freeIndices.shrink_to_fit();
    m_entityToVehicle.clear();
}

auto VehicleManager::BeginBatch() -> size_t
{
    NC_ASSERT(!m_isBatchInProgress, "Vehicle batch already in progress");
    m_isBatchInProgress = true;
    return m_handles.size();
}

void VehicleManager::EndBatch(size_t batchBegin)
{
    NC_ASSERT(m_isBatchInProgress, "Vehicle batch is not in progress");
    m_isBatchInProgress = false;

    const auto numVehicles = m_handles.size();
    if (batchBegin == numVehicles)
    {
        return;
    }

    NC_ASSERT(batchBegin < numVehicles, "Vehicle batching out-of-sync");
    for (auto& constraint : std::views::drop(m_handles, batchBegin))
    {
        AddToSimulation(m_physicsSystem, constraint.GetPtr());
    }
}
} // namespace nc::physics

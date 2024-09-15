#include "ConstraintManager.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Constraints/TwoBodyConstraint.h"

namespace nc::physics
{
auto ConstraintManager::AddConstraint(const ConstraintInfo& createInfo,
                                      Entity owner,
                                      JPH::Body* ownerBody,
                                      Entity target,
                                      JPH::Body* targetBody) -> Constraint&
{
    auto handle = m_factory.MakeConstraint(createInfo, *ownerBody, *targetBody);
    m_physicsSystem->AddConstraint(handle);

    const auto ownerId = owner.Index();
    const auto targetId = target.Index();
    const auto index = [&]()
    {
        if (m_freeIndices.empty())
        {
            const auto i = static_cast<uint32_t>(m_handles.size());
            m_handles.push_back(handle);
            m_pairs.emplace_back(ownerId, targetId);
            return i;
        }

        const auto i = m_freeIndices.back();
        m_freeIndices.pop_back();
        m_handles[i] = handle;
        m_pairs[i] = ConstraintPair{ownerId, targetId};
        return i;
    }();

    auto& ownerState = TrackConstraint(ownerId, index);
    auto& constraint = ownerState.constraints.emplace_back(createInfo, target, index);
    if (target.Valid())
    {
        TrackConstraint(targetId, index);
    }

    return constraint;
}

void ConstraintManager::EnableConstraint(Constraint& constraint, bool enabled)
{
    constraint.m_enabled = enabled;
    GetConstraintHandle(constraint.GetId())->SetEnabled(enabled);
}

void ConstraintManager::UpdateConstraint(Constraint& constraint)
{
    auto oldHandle = GetConstraintHandle(constraint.GetId());
    auto [ownerBody, targetBody] = GetConstraintBodies(oldHandle);
    ReplaceInternalConstraint(constraint, oldHandle, ownerBody, targetBody);
}

void ConstraintManager::UpdateConstraintTarget(Constraint& constraint,
                                               Entity referenced,
                                               JPH::Body* referencedBody)
{
    constraint.m_otherBody = referenced;

    const auto id = constraint.GetId();
    auto oldHandle = GetConstraintHandle(id);
    auto [ownerBody, _] = GetConstraintBodies(oldHandle);
    ReplaceInternalConstraint(constraint, oldHandle, ownerBody, referencedBody);

    auto& [trackedOwner, trackedReference] = m_pairs.at(id);
    NC_ASSERT(trackedOwner != referenced.Index(), "Attempt to self assign constraint attachment");
    if (trackedReference != Entity::NullIndex)
    {
        UntrackConstraint(trackedReference, id, false);
    }

    trackedReference = referenced.Index();
    if (referenced.Valid())
    {
        TrackConstraint(trackedReference, id);
    }
}

void ConstraintManager::RemoveConstraint(ConstraintId constraintId)
{
    const auto handle = std::exchange(m_handles.at(constraintId), nullptr);
    NC_ASSERT(handle, "Bad ConstraintId");
    m_physicsSystem->RemoveConstraint(handle);
    const auto [owner, referenced] = std::exchange(m_pairs.at(constraintId), ConstraintPair{});
    m_freeIndices.push_back(constraintId);
    UntrackConstraint(owner, constraintId, true);
    if (referenced != Entity::NullIndex)
    {
        UntrackConstraint(referenced, constraintId, false);
    }
}

void ConstraintManager::RemoveConstraints(Entity toRemove)
{
    const auto toRemoveId = toRemove.Index();
    if (!m_entityState.contains(toRemoveId))
    {
        return;
    }

    auto& toRemoveInfo = m_entityState.at(toRemoveId);
    for (auto constraintId : toRemoveInfo.ids)
    {
        const auto handle = std::exchange(m_handles[constraintId], nullptr);
        m_physicsSystem->RemoveConstraint(handle);
        m_freeIndices.push_back(constraintId);
        const auto [ownerId, referencedId] = std::exchange(m_pairs[constraintId], ConstraintPair{});
        const auto [otherId, otherIsOwner] = [toRemoveId, ownerId, referencedId]()
        {
            return toRemoveId == ownerId
                ? std::pair{referencedId, false}
                : std::pair{ownerId, true};
        }();

        if (otherId != Entity::NullIndex)
        {
            UntrackConstraint(otherId, constraintId, otherIsOwner);
        }
    }

    m_entityState.erase(toRemoveId);
}

auto ConstraintManager::GetConstraintHandle(ConstraintId constraintId) -> JPH::Constraint*
{
    auto handle = m_handles.at(constraintId);
    NC_ASSERT(handle, "Bad ConstraintId");
    return handle;
}

auto ConstraintManager::GetConstraint(Entity owner, ConstraintId constraintId) -> Constraint&
{
    auto& owned = m_entityState.at(owner.Index()).constraints;
    auto pos = std::ranges::find(owned, constraintId, &Constraint::GetId);
    NC_ASSERT(pos != owned.cend(), "Invalid ConstraintId");
    return *pos;
}

auto ConstraintManager::GetConstraints(Entity owner) -> std::span<Constraint>
{
    const auto id = owner.Index();
    if (m_entityState.contains(id))
    {
        return m_entityState.at(id).constraints;
    }

    return std::span<Constraint>{};
}

void ConstraintManager::Clear()
{
    const auto constraintsBeg = m_handles.begin();
    const auto constraintsEnd = std::remove(constraintsBeg, m_handles.end(), nullptr);
    const auto constraintCount = std::distance(constraintsBeg, constraintsEnd);
    m_physicsSystem->RemoveConstraints(m_handles.data(), static_cast<int>(constraintCount));

    m_handles.clear();
    m_handles.shrink_to_fit();
    m_pairs.clear();
    m_pairs.shrink_to_fit();
    m_freeIndices.clear();
    m_freeIndices.shrink_to_fit();
    m_entityState.clear();
}

auto ConstraintManager::TrackConstraint(uint32_t entityId,
                                        ConstraintId constraintId) -> EntityConstraints&
{
    auto& state = m_entityState.contains(entityId)
        ? m_entityState.at(entityId)
        : m_entityState.emplace(entityId, EntityConstraints{});

    state.ids.push_back(constraintId);
    return state;
}

void ConstraintManager::UntrackConstraint(uint32_t entityId,
                                          ConstraintId constraintId,
                                          bool isOwner)
{
    auto& state = m_entityState.at(entityId);
    auto idPos = std::ranges::find(state.ids, constraintId);
    NC_ASSERT(idPos != state.ids.cend(), "ConstraintId not found");
    *idPos = state.ids.back();
    state.ids.pop_back();
    if (!isOwner)
    {
        return;
    }

    auto constraintPos = std::ranges::find(state.constraints, constraintId, &nc::physics::Constraint::GetId);
    NC_ASSERT(constraintPos != state.constraints.cend(), "Constraint not found");
    state.constraints.erase(constraintPos);
}

auto ConstraintManager::GetConstraintBodies(JPH::Constraint* handle) -> ConstraintBodies
{
    const auto upcast = static_cast<JPH::TwoBodyConstraint*>(handle);
    return ConstraintBodies{upcast->GetBody1(), upcast->GetBody2()};
}

void ConstraintManager::ReplaceInternalConstraint(const Constraint& replaceWith,
                                                  JPH::Constraint* toReplace,
                                                  JPH::Body* owner,
                                                  JPH::Body* referenced)
{
    m_physicsSystem->RemoveConstraint(toReplace);
    auto newHandle = m_factory.MakeConstraint(replaceWith.GetInfo(), *owner, *referenced);
    newHandle->SetEnabled(replaceWith.IsEnabled());
    m_physicsSystem->AddConstraint(newHandle);
    m_handles.at(replaceWith.GetId()) = newHandle;
}
} // namespace nc::physics

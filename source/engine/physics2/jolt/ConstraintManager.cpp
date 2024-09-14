#include "ConstraintManager.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/PhysicsSystem.h"

namespace
{
void UntrackConstraint(nc::physics::EntityConstraints& state, nc::physics::ConstraintId constraintId, bool isOwner)
{
    auto idPos = std::ranges::find(state.ids, constraintId);
    NC_ASSERT(idPos != state.ids.cend(), "ConstraintId not found");
    *idPos = state.ids.back();
    state.ids.pop_back();
    if (!isOwner)
    {
        return;
    }

    auto infoPos = std::ranges::find(state.views, constraintId, &nc::physics::ConstraintView::id);
    NC_ASSERT(infoPos != state.views.cend(), "ConstraintView not found");
    state.views.erase(infoPos);
}
} // anonymous namespace

namespace nc::physics
{
auto ConstraintManager::AddConstraint(const ConstraintInfo& createInfo,
                                      Entity owner,
                                      JPH::Body* ownerBody,
                                      Entity referenced,
                                      JPH::Body* referencedBody) -> ConstraintId
{
    auto handle = m_factory.MakeConstraint(createInfo, *ownerBody, *referencedBody);
    m_physicsSystem->AddConstraint(handle);

    const auto ownerId = owner.Index();
    const auto referencedId = referenced.Index();
    const auto index = [&]()
    {
        if (m_freeIndices.empty())
        {
            const auto index = static_cast<uint32_t>(m_handles.size());
            m_handles.push_back(handle);
            m_pairs.emplace_back(ownerId, referencedId);
            return index;
        }

        const auto index = m_freeIndices.back();
        m_freeIndices.pop_back();
        m_handles[index] = handle;
        m_pairs[index] = ConstraintPair{ownerId, referencedId};
        return index;
    }();

    auto& ownerState = m_entityState.contains(ownerId)
        ? m_entityState.at(ownerId)
        : m_entityState.emplace(ownerId, EntityConstraints{});

    ownerState.ids.emplace_back(index);
    ownerState.views.emplace_back(createInfo, referenced, index);

    if (referenced.Valid())
    {
        auto& referencedState = m_entityState.contains(referencedId)
            ? m_entityState.at(referencedId)
            : m_entityState.emplace(referencedId, EntityConstraints{});

        referencedState.ids.emplace_back(index);
    }

    return index;
}

void ConstraintManager::RemoveConstraint(ConstraintId constraintId)
{
    const auto handle = std::exchange(m_handles.at(constraintId), nullptr);
    NC_ASSERT(handle, "Bad ConstraintId");
    m_physicsSystem->RemoveConstraint(handle);
    const auto [owner, referenced] = std::exchange(m_pairs.at(constraintId), ConstraintPair{});
    m_freeIndices.push_back(constraintId);
    UntrackConstraint(m_entityState.at(owner), constraintId, true);
    if (referenced != Entity::NullIndex)
    {
        UntrackConstraint(m_entityState.at(referenced), constraintId, false);
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
            auto& otherInfo = m_entityState.at(otherId);
            UntrackConstraint(otherInfo, constraintId, otherIsOwner);
        }
    }

    m_entityState.erase(toRemoveId);
}

auto ConstraintManager::GetConstraints(Entity owner) const -> std::span<const ConstraintView>
{
    auto id = owner.Index();
    if (m_entityState.contains(id))
    {
        return m_entityState.at(id).views;
    }

    return std::span<const ConstraintView>{};
}

void ConstraintManager::Clear()
{

    // todo: do better?
    std::erase(m_handles, nullptr);

    m_physicsSystem->RemoveConstraints(m_handles.data(), static_cast<int>(m_handles.size()));
    m_handles.clear();
    m_handles.shrink_to_fit();
    m_pairs.clear();
    m_pairs.shrink_to_fit();
    m_freeIndices.clear();
    m_freeIndices.shrink_to_fit();
    m_entityState.clear();
}
} // namespace nc::physics

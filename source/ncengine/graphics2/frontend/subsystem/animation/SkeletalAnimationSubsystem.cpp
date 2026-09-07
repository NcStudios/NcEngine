#include "SkeletalAnimationSubsystem.h"
#include "SkeletalAnimationCalculator.h"

#include "ncengine/debug/Profile.h"
#include "ncengine/time/Time.h"
#include "ncutility/NcError.h"

namespace
{
struct StepResult
{
    float ticks;
    bool completed;
};

auto GetAnimationDuration(const nc::asset::SkeletalAnimation& animation) -> float
{
    return static_cast<float>(animation.durationInTicks) / animation.ticksPerSecond;
}

auto StepAnimationTime(float& timeInOut,
                       const nc::asset::SkeletalAnimation& animation,
                       float dt) -> StepResult
{
    const auto duration = GetAnimationDuration(animation);
    const auto newTime = timeInOut + dt;
    timeInOut = std::fmod(newTime, duration);
    return StepResult{
        timeInOut * animation.ticksPerSecond,
        newTime > duration
    };
}

auto StepTransition(nc::graphics::InFlightAnimation& state,
                    const nc::asset::SkeletalAnimation& blendFromAnimation,
                    float dt) -> StepResult
{
    state.currentTransitionTime += dt;
    const auto normalized = state.currentTransitionTime / state.transitionDuration;
    state.blendFactor = std::clamp(normalized, 0.0f, 1.0f);
    if (state.currentTransitionTime >= state.transitionDuration)
    {
        state.blendFromAnimId = nc::asset::NullAssetId;
    }

    return StepAnimationTime(state.blendFromTime, blendFromAnimation, dt);
}
} // anonymous namespace

namespace nc::graphics
{
using EntityAnimation = std::pair<nc::Entity&, nc::graphics::InFlightAnimation&>;

auto ISkeletalAnimationSubsystem::AllocateBones(uint64_t meshId) -> BoneCacheHandle
{
    const auto boneCount = GetRigBoneCount(meshId);
    return boneCount > 0
        ? m_boneCache.GetStagingArea().Allocate(boneCount)
        : NullBoneCacheHandle;
}

void ISkeletalAnimationSubsystem::NotifyRemove(Entity entity, BoneCacheHandle boneIndex)
{
    m_removed.push_back(entity);
    if (boneIndex != NullBoneCacheHandle)
    {
        m_boneCache.GetStagingArea().Free(boneIndex);
    }
}

auto ISkeletalAnimationSubsystem::GetBoneSnapperOffset(Entity targetEntity) -> DirectX::XMMATRIX
{
    return m_boneSnapperOffsets.at(targetEntity.Index());
}

auto ISkeletalAnimationSubsystem::ContainsBone(uint64_t meshId, const std::string& boneName) -> bool
{
    const auto _ = m_storage.AcquireReadLock();

    const auto& rig = m_storage.GetRig(meshId);
    auto pos = std::ranges::find(rig.boneNames, boneName);
    return pos != rig.boneNames.end();
}

auto ISkeletalAnimationSubsystem::GetRigBoneCount(uint64_t meshId) -> uint32_t
{
    const auto _ = m_storage.AcquireReadLock();

    return m_storage.HasRig(meshId)
        ? static_cast<uint32_t>(m_storage.GetRig(meshId).vertexToBone.size())
        : 0u;
}

void SkeletalAnimationSubsystem::Update(ecs::ExplicitEcs<SkinnedMesh, BoneSnapper> ecs)
{
    NC_PROFILE_TASK("SkeletalAnimationSubsystem::Update()", ProfileCategory::Animation);

    const auto& boneSnappers = ecs.GetPool<BoneSnapper>();
    for (auto& boneSnapper : boneSnappers)
    {
        m_boneSnapperTargets.emplace(boneSnapper.target, boneSnapper.boneName);
    }

    auto& pool = ecs.GetPool<SkinnedMesh>();
    CommitPendingChanges();
    m_stateOrchestrator.Transition(pool, m_storage);
    CalculateBoneMatrices();
    m_stateOrchestrator.NotifyCompleted(pool, m_completedAnimations);
    m_completedAnimations.clear();
    m_boneSnapperTargets.clear();
}

void SkeletalAnimationSubsystem::CalculateBoneMatrices()
{
    NC_PROFILE_SCOPE("SkeletalAnimationSubsystem::CalculateBoneMatrices()", ProfileCategory::Animation);

    m_boneSnapperOffsets.clear();

    auto calculator = SkeletalAnimationCalculator{};
    const auto dt = time::DeltaTime();
    const auto _ = m_storage.AcquireReadLock();
    auto inFlightAnimations = std::views::zip(m_stateOrchestrator.GetEntities(), m_stateOrchestrator.GetAnimations());

    auto animationsToSnap = std::vector<EntityAnimation>{};
    auto animationsToNotSnap = std::vector<EntityAnimation>{};

    // Split the collection of animations into two categories:
    // Those that need to produce an animated bone offset for
    // a BoneSnapper component and those that don't
    for (auto&& [entity, animation] : inFlightAnimations)
    {
        if (m_boneSnapperTargets.contains(entity.Index()))
        {
            animationsToSnap.emplace_back(entity, animation);
        }
        else
        {
            animationsToNotSnap.emplace_back(entity, animation);
        }
    }

    // Has BoneSnapper loop
    for (auto [entity, state] : animationsToSnap)
    {
        auto snapBone = m_boneSnapperTargets.at(entity.Index());

        const auto& animation = m_storage.GetAnimation(state.animId);
        const auto [ticks, completed] = StepAnimationTime(state.time, animation, dt);
        if (completed)
        {
            m_completedAnimations.push_back(entity);
        }

        const auto bones = [&]()
        {
            const auto& rig = m_storage.GetRig(state.meshId);

            if (!m_storage.HasAnimation(state.blendFromAnimId))
            {
                auto boneData = calculator.Animate(rig, animation, ticks);
                m_boneSnapperOffsets.emplace(entity, calculator.GetBoneOffset(snapBone));
                return boneData;
            }

            const auto& blendFromAnimation = m_storage.GetAnimation(state.blendFromAnimId);
            const auto [blendFromTicks, unused] = StepTransition(state, blendFromAnimation, dt);
            return calculator.Animate(
                rig,
                blendFromAnimation,
                blendFromTicks,
                animation,
                ticks,
                state.blendFactor
            );
        }();

        m_boneCache.UpdateRegion(state.boneIndex, bones);
        m_boneSnapperOffsets.emplace(entity, calculator.GetBoneOffset(snapBone));
    }

    // Does not have BoneSnapper loop
    for (auto [entity, state] : animationsToNotSnap)
    {
        const auto& animation = m_storage.GetAnimation(state.animId);
        const auto [ticks, completed] = StepAnimationTime(state.time, animation, dt);
        if (completed)
        {
            m_completedAnimations.push_back(entity);
        }

        const auto bones = [&]()
        {
            const auto& rig = m_storage.GetRig(state.meshId);

            if (!m_storage.HasAnimation(state.blendFromAnimId))
            {
                return calculator.Animate(rig, animation, ticks);
            }

            const auto& blendFromAnimation = m_storage.GetAnimation(state.blendFromAnimId);
            const auto [blendFromTicks, unused] = StepTransition(state, blendFromAnimation, dt);
            return calculator.Animate(
                rig,
                blendFromAnimation,
                blendFromTicks,
                animation,
                ticks,
                state.blendFactor
            );
        }();

        m_boneCache.UpdateRegion(state.boneIndex, bones);
    }
}

void SkeletalAnimationSubsystem::CommitPendingChanges()
{
    NC_PROFILE_SCOPE("SkeletalAnimationSubsystem::CommitPendingChanges", ProfileCategory::Animation);
    m_stateOrchestrator.Remove(m_removed);
    m_removed.clear();
    m_boneCache.CommitPendingChanges();
}

auto SkeletalAnimationSubsystem::BuildState() -> SkeletalAnimationRenderState
{
    NC_PROFILE_SCOPE("SkeletalAnimationSubsystem::BuildState", ProfileCategory::Animation);
    return SkeletalAnimationRenderState{m_boneCache.BuildUpdateInfo()};
}

void SkeletalAnimationSubsystem::OnBeforeSceneLoad()
{
    m_boneCache.Purge();
    m_stateOrchestrator.Purge();

    // Previous frame items will be in here, but we're already in a good state. Just toss it.
    m_removed.clear();
    m_removed.shrink_to_fit();
    m_completedAnimations.clear();
    m_completedAnimations.shrink_to_fit();
}
} // namespace nc::graphics

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
        state.blendFromAnimId = nc::NullAnimationId;
    }

    return StepAnimationTime(state.blendFromTime, blendFromAnimation, dt);
}
} // anonymous namespace

namespace nc::graphics
{
auto ISkeletalAnimationSubsystem::GetRigBoneCount(uint64_t meshId) -> uint32_t
{
    const auto _ = m_storage.AcquireReadLock();
    return m_storage.HasRig(meshId)
        ? static_cast<uint32_t>(m_storage.GetRig(meshId).vertexToBone.size())
        : 0u;
}

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

void SkeletalAnimationSubsystem::Update(ecs::ExplicitEcs<SkinnedMesh> ecs)
{
    NC_PROFILE_TASK("SkeletalAnimationSubsystem::Update()", ProfileCategory::Animation);

    auto& pool = ecs.GetPool<SkinnedMesh>();
    CommitPendingChanges();
    Transition(pool);
    CalculateBoneMatrices();
    NotifyCompletedAnimations(pool);
}

void SkeletalAnimationSubsystem::CalculateBoneMatrices()
{
    NC_PROFILE_SCOPE("SkeletalAnimationSubsystem::CalculateBoneMatrices()", ProfileCategory::Animation);

    auto calculator = SkeletalAnimationCalculator{};
    const auto dt = time::DeltaTime();
    const auto _ = m_storage.AcquireReadLock();
    for (auto [entity, state] : std::views::zip(m_animatedEntities, m_animationState))
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
    // todo: we have another pop-swap thing below - make some algorithm finally?

    for (const auto toRemove : m_removed)
    {
        auto pos = std::ranges::find(m_animatedEntities, toRemove);
        if (pos != m_animatedEntities.end())
        {
            const auto index = (size_t)std::distance(m_animatedEntities.begin(), pos);
            *pos = m_animatedEntities.back();
            m_animatedEntities.pop_back();
            m_animationState.at(index) = std::move(m_animationState.back());
            m_animationState.pop_back();
        }
    }

    m_removed.clear();
    m_boneCache.CommitPendingChanges();
}

void SkeletalAnimationSubsystem::NotifyCompletedAnimations(ecs::ComponentPool<SkinnedMesh>& pool)
{
    for (const auto entity : m_completedAnimations)
    {
        pool.Get(entity).GetAnimationController().NotifyCompleteState();
    }

    m_completedAnimations.clear();
}

auto SkeletalAnimationSubsystem::BuildState() -> SkeletalAnimationRenderState
{
    return SkeletalAnimationRenderState{m_boneCache.BuildUpdateInfo()};
}

void SkeletalAnimationSubsystem::OnBeforeSceneLoad()
{
    m_boneCache.Purge();
    m_animatedEntities.shrink_to_fit();
    m_animationState.shrink_to_fit();
    m_removed.shrink_to_fit();
    m_completedAnimations.clear();
    m_completedAnimations.shrink_to_fit();
}

void SkeletalAnimationSubsystem::Clear() noexcept
{
    // if mesh subsystem notifies on all removes, we can just move to OnBeforeSceneLoad() I think
}

void SkeletalAnimationSubsystem::Transition(ecs::ComponentPool<SkinnedMesh>& pool)
{
    for (auto& mesh : pool)
    {
        const auto transition = mesh.GetAnimationController().CheckForTransition();
        switch (transition.type)
        {
            case AnimationTransitionType::Continue:
                break;
            case AnimationTransitionType::Loop:
            case AnimationTransitionType::PlayOnce:
                Start(mesh.GetContext(), transition);
                break;
            case AnimationTransitionType::Stop:
                Stop(mesh.GetContext());
                break;
        }
    }
}

void SkeletalAnimationSubsystem::Start(const MeshInstanceContext& ctx, const nc::AnimationTransition& transition)
{
    // Null animation and boneless mesh are valid, just ignore
    if (transition.toAnimId == NullAnimationId || !m_storage.HasRig(ctx.meshId))
    {
        return;
    }

    const auto pos = std::ranges::find(m_animatedEntities, ctx.entity);
    if (pos == m_animatedEntities.end())
    {
        m_animatedEntities.push_back(ctx.entity);
        m_animationState.emplace_back(
            ctx.meshId,
            transition.toAnimId,
            transition.fromAnimId,
            ctx.boneDataHandle,
            0.0f,
            0.0f,
            0.0f,
            transition.transitionDuration,
            0.0f
        );

        return;
    }

    auto& state = m_animationState.at(std::distance(m_animatedEntities.begin(), pos));
    const auto blendFromTime = state.time;
    state = InFlightAnimation{
        ctx.meshId,
        transition.toAnimId,
        transition.fromAnimId,
        ctx.boneDataHandle,
        0.0f,
        blendFromTime,
        0.0f,
        transition.transitionDuration,
        0.0f
    };
}

void SkeletalAnimationSubsystem::Stop(const MeshInstanceContext& ctx)
{
    auto pos = std::ranges::find(m_animatedEntities, ctx.entity);
    if (pos == m_animatedEntities.end())
    {
        return;
    }

    auto posIndex = std::distance(m_animatedEntities.begin(), pos);
    m_animatedEntities.at(posIndex) = m_animatedEntities.back();
    m_animatedEntities.pop_back();
    m_animationState.at(posIndex) = m_animationState.back();
    m_animationState.pop_back();
}
} // namespace nc::graphics

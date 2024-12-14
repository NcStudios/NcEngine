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

auto StepTransition(nc::graphics::gfx2::InFlightAnimation& state,
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
void SkeletalAnimationSubsystem::CalculateBoneMatrices()
{
    NC_PROFILE_TASK("SkeletalAnimationSubsystem::CalculateBoneMatrices()", ProfileCategory::Animation);
    m_boneCache.CommitPendingChanges();
    auto boneBuffer = std::vector<BoneData>{};
    auto calculator = gfx3::SkeletalAnimationCalculator{};

    const auto dt = time::DeltaTime();
    const auto _ = m_storage.AcquireReadLock();
    for (auto [entity, state] : std::views::zip(m_animatedEntities, m_animationState))
    {
        const auto& rig = m_storage.GetRig(state.meshId);
        const auto& animation = m_storage.GetAnimation(state.animId);
        const auto [ticks, completed] = StepAnimationTime(state.time, animation, dt);
        if (completed)
        {
            m_completedAnimations.push_back(entity);
        }

        if (m_storage.HasAnimation(state.blendFromAnimId))
        {
            const auto& blendFromAnimation = m_storage.GetAnimation(state.blendFromAnimId);
            const auto [blendFromTicks, unused] = StepTransition(state, blendFromAnimation, dt);
            const auto bones = calculator.Animate(
                rig,
                blendFromAnimation,
                blendFromTicks,
                animation,
                ticks,
                state.blendFactor
            );

            m_boneCache.UpdateRegion(state.boneIndex, bones);
        }
        else
        {
            const auto bones = calculator.Animate(
                rig,
                animation,
                ticks
            );

            m_boneCache.UpdateRegion(state.boneIndex, bones);
        }
    }
}

void SkeletalAnimationSubsystem::UpdateAnimationControllers(ecs::ExplicitEcs<SkinnedMesh> ecs)
{
    NC_PROFILE_TASK("SkeletalAnimationSubsystem::UpdateAnimationControllers()", ProfileCategory::Animation);

    auto& pool = ecs.GetPool<SkinnedMesh>();
    for (const auto entity : m_completedAnimations)
    {
        pool.Get(entity).GetAnimationController().NotifyCompleteState();
    }

    m_completedAnimations.clear();

    for (auto& mesh : pool)
    {
        Transition(mesh);
    }
}

auto SkeletalAnimationSubsystem::BuildState() -> SkeletalAnimationRenderState
{
    return SkeletalAnimationRenderState{m_boneCache.BuildUpdateInfo()};
}

void SkeletalAnimationSubsystem::OnBeforeSceneLoad()
{
    m_boneCache.Purge();
}

void SkeletalAnimationSubsystem::Clear() noexcept
{
    // todo: insufficient - does not handle persistent entities:
    m_animatedEntities.clear();
    m_animatedEntities.shrink_to_fit();
    m_animationState.clear();
    m_animationState.shrink_to_fit();
}


void SkeletalAnimationSubsystem::Transition(SkinnedMesh& mesh)
{
    const auto transition = mesh.GetAnimationController().CheckForTransition();
    switch (transition.type)
    {
        case AnimationTransitionType::Continue: return;
        case AnimationTransitionType::Loop:     [[fallthrough]];
        case AnimationTransitionType::PlayOnce: return Start(mesh.GetContext(), transition);
        case AnimationTransitionType::Stop:     return Stop(mesh.GetContext());
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
    state = gfx2::InFlightAnimation{
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

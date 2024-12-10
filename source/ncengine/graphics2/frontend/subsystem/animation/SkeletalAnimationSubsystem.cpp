#include "SkeletalAnimationSubsystem.h"
#include "SkeletalAnimationCalculations2.h"

#include "ncengine/time/Time.h"

namespace
{
auto GetAnimationDuration(const nc::asset::SkeletalAnimation& animation) -> float
{
    return static_cast<float>(animation.durationInTicks) / animation.ticksPerSecond;
}

auto StepAnimationTime(const nc::asset::SkeletalAnimation& animation,
                       float currentTime,
                       float dt) -> float
{
    return std::fmod(currentTime + dt, GetAnimationDuration(animation));
}

auto PrepareAnimation(nc::graphics::gfx2::InFlightAnimation& animation,
                      const nc::asset::SkeletalAnimation* fromAnim,
                      const nc::asset::SkeletalAnimation* toAnim,
                      const nc::graphics::gfx2::PackedRig& rig,
                      float dt) -> nc::graphics::gfx2::PackedAnimation
{
    if (fromAnim && toAnim)
    {
        animation.blendToTime = StepAnimationTime(*toAnim, animation.blendToTime, dt);
        animation.blendFromTime = StepAnimationTime(*fromAnim, animation.blendFromTime, dt);
        const auto blendFromTimeTicks = animation.blendFromTime * fromAnim->ticksPerSecond;
        const auto blendToTimeTicks = animation.blendToTime * toAnim->ticksPerSecond;
        return nc::graphics::gfx2::ComposeBlendedMatrices(
            blendFromTimeTicks,
            blendToTimeTicks,
            animation.blendFactor,
            rig.boneNames,
            *fromAnim,
            *toAnim
        );
    }

    auto [timeInSeconds, animData] = [&animation, fromAnim, toAnim]()
    {
        return fromAnim
            ? std::pair{&animation.blendFromTime, fromAnim}
            : std::pair{&animation.blendToTime, toAnim};
    }();

    *timeInSeconds = StepAnimationTime(*animData, *timeInSeconds, dt);
    const auto timeInTicks = *timeInSeconds * animData->ticksPerSecond;
    return nc::graphics::gfx2::ComposeMatrices(timeInTicks, rig.boneNames, *animData);
}

auto HasCompletedAnimationCycle(const nc::graphics::gfx2::InFlightAnimation animation,
                                const nc::asset::SkeletalAnimation* toAnim,
                                float dt) -> bool
{
    // return toAnim && animation.blendToTime + dt > GetAnimationDuration(*toAnim);

    if (!toAnim)
        return false;

    const auto dur = GetAnimationDuration(*toAnim);
    const auto proj = animation.blendToTime + dt;

    if (proj > dur)
        return true;
    else
        return false;
}
} // anonymous namespace

namespace nc::graphics
{
void SkeletalAnimationSubsystem::CalculateBoneMatrices()
{
    // todo: would like to preallocate buffer/BoneCache based on capacity from anim system


    // icky, 'unused' values are set w/ homogenous extension, should find way to make them 0
    // std::memset(m_buffer.data(), 0, m_buffer.size() * sizeof(BoneData));
    const auto m = DirectX::XMMatrixSet(
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // for (auto& b : m_buffer)
    // {
    //     b.modelMatrix = m;
    // }



    auto boneBuffer = std::vector<BoneData>{};
    const auto dt = time::DeltaTime();
    const auto _ = m_storage.AcquireLock();
    for (auto [entity, animation] : std::views::zip(m_animatedEntities, m_animationState))
    {
        // todo: animations might not exist, may need to be nullable?
        const auto from = m_storage.HasAnimation(animation.blendFromAnim) ? &m_storage.GetAnimation(animation.blendFromAnim) : nullptr;
        const auto to = m_storage.HasAnimation(animation.blendToAnim) ? &m_storage.GetAnimation(animation.blendToAnim) : nullptr;
        const auto& rig = m_storage.GetRig(animation.meshId);

        // todo: can/should just clamp here?
        animation.blendFactor = std::clamp(animation.blendFactor + dt * 8.0f, 0.0f, 1.0f);
        // if (animation.blendFactor < 1.0f)
        // {
        //     animation.blendFactor += dt * 2.0f;
        // }

        if (::HasCompletedAnimationCycle(animation, to, dt))
        {
            m_completedAnimations.push_back(entity);
            // todo: return here? what the heck else?
        }

        // trying stuff
        auto packedAnimation =
            animation.blendFactor >= 1.0f
                ? ::PrepareAnimation(animation, nullptr, to, rig, dt)
                : ::PrepareAnimation(animation, from, to, rig, dt);


        // auto packedAnimation = ::PrepareAnimation(animation, from, to, rig, dt);

        // todo: could reuse animatedBones vector to minimize allocs
        gfx2::AnimateBones(rig, packedAnimation, boneBuffer);

        // todo: BoneCache not implemented here
        // m_boneCache.UpdateRegion(animation.boneIndex, animatedBones);
        if (animation.boneIndex + boneBuffer.size() > m_buffer.size())
        {
            m_buffer.resize(animation.boneIndex + boneBuffer.size(), BoneData{m});
        }

        std::memcpy(m_buffer.data() + animation.boneIndex, boneBuffer.data(), boneBuffer.size() * sizeof(BoneData));
        boneBuffer.clear();
        // std::ranges::copy(animatedBones, m_buffer.begin() + animation.boneIndex);
    }
}

void SkeletalAnimationSubsystem::UpdateAnimationControllers(ecs::ExplicitEcs<SkinnedMesh> ecs)
{
    for (const auto entity : m_completedAnimations)
    {
        ecs.Get<SkinnedMesh>(entity).GetAnimationController().NotifyCompleteState();
    }

    m_completedAnimations.clear();

    for (auto& mesh : ecs.GetAll<SkinnedMesh>())
    {
        Transition(mesh);
    }
}

auto SkeletalAnimationSubsystem::BuildState() -> SkeletalAnimationRenderState
{
    // todo: use BoneCache
    if (m_buffer.empty())
    {
        return SkeletalAnimationRenderState{};
    }

    return SkeletalAnimationRenderState{
        m_buffer,
        { {0, static_cast<uint32_t>(m_buffer.size())} }
    };
}

void SkeletalAnimationSubsystem::Clear() noexcept
{
    // todo: insufficient - does not handle persistent entities:
    m_animatedEntities.clear();
    m_animatedEntities.shrink_to_fit();
    m_animationState.clear();
    m_animationState.shrink_to_fit();
    m_buffer.clear();
    m_buffer.shrink_to_fit();
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
    const auto hasAssets =
        m_storage.HasRig(ctx.meshId) &&
        (m_storage.HasAnimation(transition.prevAnimId) ||
         m_storage.HasAnimation(transition.curAnimId));

    if (!hasAssets)
    {
        return;
    }

    const auto pos = std::ranges::find(m_animatedEntities, ctx.entity);
    if (pos == m_animatedEntities.end())
    {
        m_animatedEntities.push_back(ctx.entity);
        m_animationState.emplace_back(
            ctx.meshId,
            transition.prevAnimId,
            transition.curAnimId,
            ctx.boneDataHandle,
            0.0f,
            0.0f,
            0.0f
        );

        return;
    }

    auto& state = m_animationState.at(std::distance(m_animatedEntities.begin(), pos));
    const auto blendFromTime = state.blendToTime;
    state = gfx2::InFlightAnimation{
        ctx.meshId,
        transition.prevAnimId,
        transition.curAnimId,
        ctx.boneDataHandle,
        blendFromTime,
        0.0f,
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

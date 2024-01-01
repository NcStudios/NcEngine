#include "SkeletalAnimationSystem.h"
#include "ecs/View.h"
#include "graphics/system/SkeletalAnimationCalculations.h"
#include "time/Time.h"

namespace
{
auto PrepareAnimation(nc::graphics::anim::UnitOfWork& unit, const nc::graphics::anim::PackedRig& rig, float dt) -> nc::graphics::anim::PackedAnimation
{
    auto packedAnimation = nc::graphics::anim::PackedAnimation{};

    if (unit.blendFromAnim && unit.blendToAnim)
    {
        unit.blendToTime =   fmod((unit.blendToTime + dt),   (static_cast<float>(unit.blendToAnim->durationInTicks)   / unit.blendToAnim->ticksPerSecond));
        unit.blendFromTime = fmod((unit.blendFromTime + dt), (static_cast<float>(unit.blendFromAnim->durationInTicks) / unit.blendFromAnim->ticksPerSecond));

        auto blendFromTimeTicks = unit.blendFromTime * unit.blendFromAnim->ticksPerSecond;
        auto blendToTimeTicks =   unit.blendToTime   * unit.blendToAnim->ticksPerSecond;

        return nc::graphics::ComposeBlendedMatrices(blendFromTimeTicks, blendToTimeTicks, unit.blendFactor, rig.boneNames, unit.blendFromAnim, unit.blendToAnim);
    }

    float* timeInSeconds;
    nc::asset::SkeletalAnimation* animData;

    if (unit.blendFromAnim)
    {
        timeInSeconds = &unit.blendFromTime;
        animData = unit.blendFromAnim;
    }
    else
    {
        timeInSeconds = &unit.blendToTime;
        animData = unit.blendToAnim;
    }

    *timeInSeconds = fmod((*timeInSeconds + dt), (static_cast<float>(animData->durationInTicks)/animData->ticksPerSecond));
    auto timeInTicks = *timeInSeconds * animData->ticksPerSecond;
    return nc::graphics::ComposeMatrices(timeInTicks, rig.boneNames, animData);
}

auto HasCompletedAnimationCycle(const nc::graphics::anim::UnitOfWork unit, float dt) -> bool
{
    return (unit.blendToAnim && (unit.blendToTime + dt > static_cast<float>(unit.blendToAnim->durationInTicks)/unit.blendToAnim->ticksPerSecond));
}
}

namespace nc::graphics
{
SkeletalAnimationSystem::SkeletalAnimationSystem(Registry* registry,
                                                 Signal<const asset::SkeletalAnimationUpdateEventData&>& onSkeletalAnimationUpdate,
                                                 Signal<const asset::BoneUpdateEventData&>& onBonesUpdate,
                                                 Signal<const std::vector<SkeletalAnimationData>&>&& gpuBackendChannel)
    : m_registry{registry},
      m_rigs{},
      m_onBonesUpdate{onBonesUpdate.Connect(this, &SkeletalAnimationSystem::UpdateBonesStorage)},
      m_animationAssets{},
      m_onSkeletalAnimationUpdate{onSkeletalAnimationUpdate.Connect(this, &SkeletalAnimationSystem::UpdateSkeletalAnimationStorage)},
      m_onAddConnection{registry->OnAdd<graphics::SkeletalAnimator>().Connect(this, &SkeletalAnimationSystem::Add)},
      m_onRemoveConnection{registry->OnRemove<graphics::SkeletalAnimator>().Connect(this, &SkeletalAnimationSystem::Remove)},
      m_onPlayStateChangedHandlers{},
      m_handlerIndices{},
      m_unitEntities{},
      m_units{},
      m_gpuBackendChannel{std::move(gpuBackendChannel)}
{}

auto SkeletalAnimationSystem::Execute() -> SkeletalAnimationSystemState
{
    auto animators = View<SkeletalAnimator>{m_registry};
    for (auto& animator : animators)
    {
        animator.Execute();
    }

    if (m_units.empty()) return SkeletalAnimationSystemState{};

    auto stateIndex = 0u;
    auto buffer = std::vector<SkeletalAnimationData>{};
    auto state = SkeletalAnimationSystemState{};
    auto dt = time::DeltaTime();

    for (auto i = 0u; m_units.size(); i++)
    {
        auto& unit = m_units.at(i);
        auto& rig = *unit.rig;
        auto unitIndex = m_unitEntities[i].Index();

        if (unit.blendFactor < 1.0f) unit.blendFactor += dt * 2.0f;
        if (HasCompletedAnimationCycle(unit, dt)) m_registry->Get<SkeletalAnimator>(m_unitEntities[i])->CompleteFirstRun();

        auto packedAnimation = PrepareAnimation(unit, rig, dt);
        auto animatedBones = AnimateBones(rig, packedAnimation);

        buffer.insert(buffer.begin(), animatedBones.begin(), animatedBones.end());
        state.animationIndices.emplace(unitIndex, stateIndex);
        stateIndex = static_cast<uint32_t>(buffer.size());
    }

    m_gpuBackendChannel.Emit(buffer);
    return state;
}

void SkeletalAnimationSystem::UpdateSkeletalAnimationStorage(const asset::SkeletalAnimationUpdateEventData& eventData)
{
    switch (eventData.updateAction)
    {
        case asset::UpdateAction::Load:
        {
            for (auto i = 0u; i < eventData.ids.size(); i++)
            {
                m_animationAssets.emplace(eventData.ids[i], eventData.data[i]);
            }
            break;
        }
        case asset::UpdateAction::Unload:
        {
            for (auto i = 0u; i < eventData.ids.size(); i++)
            {
                m_animationAssets.erase(eventData.ids[i]);
            }
            break;
        }
        case asset::UpdateAction::UnloadAll:
        {
            m_units.clear();
            m_unitEntities.clear();
            m_animationAssets.clear();
            break;
        }
    }
}

void SkeletalAnimationSystem::UpdateBonesStorage(const asset::BoneUpdateEventData& eventData)
{
    switch (eventData.updateAction)
    {
        case asset::UpdateAction::Load:
        {
            for (auto i = 0u; i < eventData.ids.size(); i++)
            {
                m_rigs.emplace(eventData.ids[i], eventData.data[i]); /* @todo Need to move here and not have span in event data? To avoid copying? */
            }
            break;
        }
        case asset::UpdateAction::Unload:
        {
            for (auto i = 0u; i < eventData.ids.size(); i++)
            {
                m_rigs.erase(eventData.ids[i]);
            }
            break;
        }
        case asset::UpdateAction::UnloadAll:
        {
            m_units.clear();
            m_unitEntities.clear();
            m_rigs.clear();
            break;
        }
    }
}

void SkeletalAnimationSystem::Start(const anim::PlayState& playState)
{
    if (m_rigs.find(playState.meshUid) == m_rigs.end() ||
        m_animationAssets.find(playState.curAnimUid) == m_animationAssets.end() ||
        m_animationAssets.find(playState.prevAnimUid) == m_animationAssets.end())
    {
        return;
    }

    auto pos = std::find_if(m_unitEntities.begin(), m_unitEntities.end(), [playState](Entity entity){ return playState.entity.Index() == entity.Index(); });

    if (pos == m_unitEntities.end())
    {
        m_unitEntities.push_back(playState.entity);
        m_units.push_back(anim::UnitOfWork
        {
            .index         = playState.entity.Index(),
            .rig           = &m_rigs.at(playState.meshUid),
            .blendFromAnim = playState.prevAnimUid == std::string{} ? nullptr : &m_animationAssets.at(playState.prevAnimUid),
            .blendToAnim   = playState.curAnimUid == std::string{} ? nullptr : &m_animationAssets.at(playState.curAnimUid),
            .blendFromTime = 0.0f,
            .blendToTime   = 0.0f,
            .blendFactor   = 0.0f
        });
        return;
    }

    auto posIndex = std::distance(m_unitEntities.begin(), pos);
    m_unitEntities.at(posIndex) = playState.entity;
    m_units.at(posIndex) = anim::UnitOfWork
    {
        .index         = playState.entity.Index(),
        .rig           = &m_rigs.at(playState.meshUid),
        .blendFromAnim = playState.prevAnimUid == std::string{} ? nullptr : &m_animationAssets.at(playState.prevAnimUid),
        .blendToAnim   = playState.curAnimUid == std::string{} ? nullptr : &m_animationAssets.at(playState.curAnimUid),
        .blendFromTime = m_units.at(posIndex).blendToTime,
        .blendToTime   = 0.0f,
        .blendFactor   = 0.0f
    };
}

void SkeletalAnimationSystem::Clear() noexcept
{
    m_units.clear();
    m_unitEntities.clear();
}

void SkeletalAnimationSystem::OnPlayStateChanged(const anim::PlayState& playState)
{
    Start(playState);
}

void SkeletalAnimationSystem::Add(SkeletalAnimator& animator)
{
    auto& signal = animator.Connect();
    auto connectionState = std::make_unique<Connection<const anim::PlayState&>>(signal.Connect(this, &SkeletalAnimationSystem::OnPlayStateChanged));
    m_onPlayStateChangedHandlers.emplace_back(std::move(connectionState));
    m_handlerIndices.emplace_back(animator.ParentEntity().Index());
}

void SkeletalAnimationSystem::Remove(Entity entity)
{
    auto pos = std::find(m_handlerIndices.begin(), m_handlerIndices.end(), entity.Index());
    if(pos == m_handlerIndices.end())
    {
        return;
    }

    auto index = std::distance(m_handlerIndices.begin(), pos);
    m_handlerIndices.at(index) = m_handlerIndices.back();
    m_handlerIndices.pop_back();
    m_onPlayStateChangedHandlers.at(index) = std::move(m_onPlayStateChangedHandlers.back());
    m_onPlayStateChangedHandlers.pop_back();
}
}
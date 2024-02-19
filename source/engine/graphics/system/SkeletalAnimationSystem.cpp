#include "SkeletalAnimationSystem.h"
#include "ecs/View.h"
#include "graphics/system/SkeletalAnimationCalculations.h"
#include "time/Time.h"

#include "optick.h"

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

    auto [timeInSeconds, animData] = [&unit]()
    {
        return unit.blendFromAnim ?
            std::pair{&unit.blendFromTime, unit.blendFromAnim} :
            std::pair{&unit.blendToTime, unit.blendToAnim};
    }();

    *timeInSeconds = fmod((*timeInSeconds + dt), (static_cast<float>(animData->durationInTicks)/animData->ticksPerSecond));
    auto timeInTicks = *timeInSeconds * animData->ticksPerSecond;
    return nc::graphics::ComposeMatrices(timeInTicks, rig.boneNames, animData);
}

auto HasCompletedAnimationCycle(const nc::graphics::anim::UnitOfWork unit, float dt) -> bool
{
    return unit.blendToAnim && (unit.blendToTime + dt > static_cast<float>(unit.blendToAnim->durationInTicks)/unit.blendToAnim->ticksPerSecond);
}
} // anonymous namespace

namespace nc::graphics
{
SkeletalAnimationSystem::SkeletalAnimationSystem(Registry* registry,
                                                 ShaderResourceBus* shaderResourceBus,
                                                 uint32_t maxSkeletalAnimations,
                                                 Signal<const asset::SkeletalAnimationUpdateEventData&>& onSkeletalAnimationUpdate,
                                                 Signal<const asset::BoneUpdateEventData&>& onBonesUpdate)
    : m_registry{registry},
      m_rigs{},
      m_onBonesUpdate{onBonesUpdate.Connect(this, &SkeletalAnimationSystem::UpdateBonesStorage)},
      m_animationAssets{},
      m_onSkeletalAnimationUpdate{onSkeletalAnimationUpdate.Connect(this, &SkeletalAnimationSystem::UpdateSkeletalAnimationStorage)},
      m_onAddConnection{registry->OnAdd<graphics::SkeletalAnimator>().Connect(this, &SkeletalAnimationSystem::Add)},
      m_onRemoveConnection{registry->OnRemove<graphics::SkeletalAnimator>().Connect(this, &SkeletalAnimationSystem::Remove)},
      m_onStateChangedHandlers{},
      m_handlerIndices{},
      m_unitEntities{},
      m_units{},
      m_skeletalAnimationDataBuffer{shaderResourceBus->CreateStorageBuffer(sizeof(SkeletalAnimationData) * AvgBonesPerAnim * maxSkeletalAnimations, ShaderStage::Vertex, 6, 0, false)}
{}

auto SkeletalAnimationSystem::Execute(uint32_t frameIndex) -> SkeletalAnimationSystemState
{
    OPTICK_CATEGORY("Execute", Optick::Category::Animation);

    auto animators = View<SkeletalAnimator>{m_registry};
    for (auto& animator : animators)
    {
        animator.Execute();
    }

    if (m_units.empty()) return SkeletalAnimationSystemState{};

    auto stateIndex = 0u;
    auto buffer = std::vector<SkeletalAnimationData>{};
    auto state = SkeletalAnimationSystemState{};
    const auto dt = time::DeltaTime();

    for (auto&& [unit, unitEntity] : std::views::zip(m_units, m_unitEntities))
    {
        const auto& rig = *unit.rig;
        const auto unitIndex = unitEntity.Index();

        if (unit.blendFactor < 1.0f) unit.blendFactor += dt * 2.0f;
        if (HasCompletedAnimationCycle(unit, dt)) m_registry->Get<SkeletalAnimator>(unitEntity)->CompleteFirstRun();

        auto packedAnimation = PrepareAnimation(unit, rig, dt);
        auto animatedBones = AnimateBones(rig, packedAnimation);

        buffer.insert(buffer.end(), animatedBones.begin(), animatedBones.end());
        state.animationIndices.emplace(unitIndex, stateIndex);
        stateIndex = static_cast<uint32_t>(buffer.size());
    }

    m_skeletalAnimationDataBuffer.Bind(static_cast<void*>(buffer.data()), sizeof(SkeletalAnimationData) * buffer.size(), frameIndex);
    return state;
}

void SkeletalAnimationSystem::UpdateSkeletalAnimationStorage(const asset::SkeletalAnimationUpdateEventData& eventData)
{
    switch (eventData.updateAction)
    {
        case asset::UpdateAction::Load:
        {
            for (auto&& [assetUid, asset] : std::views::zip(eventData.ids, eventData.data))
            {
                m_animationAssets.emplace(assetUid, asset);
            }
            break;
        }
        case asset::UpdateAction::Unload:
        {
            std::ranges::for_each(eventData.ids, [this](auto&& id) { m_animationAssets.erase(id); });
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
            for (auto&& [assetUid, asset] : std::views::zip(eventData.ids, eventData.data))
            {
                m_rigs.emplace(assetUid, asset);
            }
            break;
        }
        case asset::UpdateAction::Unload:
        {
            std::ranges::for_each(eventData.ids, [this](auto&& id) { m_rigs.erase(id); });
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

void SkeletalAnimationSystem::Start(const anim::StateChange& stateChange)
{
    if (!m_rigs.contains(stateChange.meshUid.data()) ||
       (!m_animationAssets.contains(stateChange.curAnimUid.data()) && !m_animationAssets.contains(stateChange.prevAnimUid.data())))
    {
        return;
    }

    auto pos = std::ranges::find(m_unitEntities, stateChange.entity);
    if (pos == m_unitEntities.end())
    {
        m_unitEntities.emplace_back(stateChange.entity);
        m_units.emplace_back
        (
            stateChange.entity.Index(),
            &m_rigs.at(stateChange.meshUid),
            stateChange.prevAnimUid.empty() ? nullptr : &m_animationAssets.at(stateChange.prevAnimUid),
            stateChange.curAnimUid.empty() ? nullptr : &m_animationAssets.at(stateChange.curAnimUid),
            0.0f,
            0.0f,
            0.0f
        );
        return;
    }

    auto posIndex = std::distance(m_unitEntities.begin(), pos);
    m_unitEntities.at(posIndex) = stateChange.entity;
    m_units.at(posIndex) = anim::UnitOfWork
    {
        .index         = stateChange.entity.Index(),
        .rig           = &m_rigs.at(stateChange.meshUid),
        .blendFromAnim = stateChange.prevAnimUid.empty() ? nullptr : &m_animationAssets.at(stateChange.prevAnimUid),
        .blendToAnim   = stateChange.curAnimUid.empty() ? nullptr : &m_animationAssets.at(stateChange.curAnimUid),
        .blendFromTime = m_units.at(posIndex).blendToTime,
        .blendToTime   = 0.0f,
        .blendFactor   = 0.0f
    };
}

void SkeletalAnimationSystem::Stop(const anim::StateChange& stateChange)
{
     auto pos = std::ranges::find(m_unitEntities, stateChange.entity);
    if (pos == m_unitEntities.end())
    {
        return;
    }
    
    auto posIndex = std::distance(m_unitEntities.begin(), pos);
    m_unitEntities.at(posIndex) = m_unitEntities.back();
    m_unitEntities.pop_back();
    m_units.at(posIndex) = m_units.back();
    m_units.pop_back();
}

void SkeletalAnimationSystem::Clear() noexcept
{
    m_units.clear();
    m_unitEntities.clear();
}

void SkeletalAnimationSystem::OnStateChanged(const anim::StateChange& stateChange)
{
    if (stateChange.action == anim::Action::Stop)
    {
        Stop(stateChange);
        return;
    }
    Start(stateChange);
}

void SkeletalAnimationSystem::Add(SkeletalAnimator& animator)
{
    auto& signal = animator.Connect();
    auto connectionState = std::make_unique<Connection<const anim::StateChange&>>(signal.Connect(this, &SkeletalAnimationSystem::OnStateChanged));
    m_onStateChangedHandlers.emplace_back(std::move(connectionState));
    m_handlerIndices.emplace_back(animator.ParentEntity().Index());
}

void SkeletalAnimationSystem::Remove(Entity entity)
{
    auto pos = std::ranges::find(m_handlerIndices, entity.Index());
    if(pos == m_handlerIndices.end())
    {
        return;
    }

    const auto index = std::distance(m_handlerIndices.begin(), pos);
    m_handlerIndices.at(index) = m_handlerIndices.back();
    m_handlerIndices.pop_back();
    m_onStateChangedHandlers.at(index) = std::move(m_onStateChangedHandlers.back());
    m_onStateChangedHandlers.pop_back();
}
}
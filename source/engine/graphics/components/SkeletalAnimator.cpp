#include "graphics/SkeletalAnimator.h"

namespace nc::graphics
{
SkeletalAnimator::SkeletalAnimator(Entity entity, std::string meshUid, std::string animationUid)
    : ComponentBase{entity},
      m_states{MaxStates},
      m_onStateChanged{},
      m_meshUid{std::move(meshUid)},
      m_immediateState{},
      m_activeState{m_states.Insert(anim::State{anim::Root{std::move(animationUid)}})},
      m_immediateStateSet{},
      m_rootStateSet{}
{
}

void SkeletalAnimator::Execute()
{
    if (ExecuteRootState()) { return; }
    if (ExecuteImmediateState()) { return; }
    if (ExecuteExitState()) { return; }

    for (auto childStateHandle : m_states.Get(m_activeState)->successors)
    {
        if (ExecuteChildEnterState(childStateHandle)) { return; }
    }
}

auto SkeletalAnimator::AddState(anim::Loop loopProperties) -> uint32_t
{
    const auto addedStateHandle = m_states.Insert(anim::State{std::move(loopProperties)});
    auto* addedState = m_states.GetLast();
    m_states.Get(addedState->enterFrom)->AddSuccessor(addedStateHandle);
    return addedStateHandle;
}

auto SkeletalAnimator::AddState(anim::PlayOnce playOnceProperties) -> uint32_t
{
    const auto addedStateHandle = m_states.Insert(anim::State{std::move(playOnceProperties)});
    auto* addedState = m_states.GetLast();
    m_states.Get(addedState->enterFrom)->AddSuccessor(addedStateHandle);
    return addedStateHandle;
}

auto SkeletalAnimator::AddState(anim::Stop stopProperties) -> uint32_t
{
    auto state = anim::State{std::move(stopProperties)};
    state.animUid = m_states.Get(m_activeState)->animUid;
    const auto addedStateHandle = m_states.Insert(std::move(state));
    auto* addedState = m_states.GetLast();
    m_states.Get(addedState->enterFrom)->AddSuccessor(addedStateHandle);
    return addedStateHandle;
}

void SkeletalAnimator::LoopImmediate(std::string animUid, std::function<bool ()> exitWhen, uint32_t exitTo)
{
    m_immediateState = std::make_unique<anim::State>(anim::Loop
    {
        .enterFrom = anim::NullState,
        .enterWhen = anim::Never,
        .animUid = std::move(animUid),
        .exitWhen = std::move(exitWhen),
        .exitTo = exitTo
    });
}

void SkeletalAnimator::PlayOnceImmediate(std::string animUid, uint32_t exitTo)
{
    m_immediateState = std::make_unique<anim::State>(anim::PlayOnce
    {
        .enterFrom = anim::NullState,
        .enterWhen = anim::Never,
        .animUid = std::move(animUid),
        .exitTo = exitTo
    });
}

void SkeletalAnimator::StopImmediate(std::function<bool ()> exitWhen, uint32_t exitTo)
{
    m_immediateState = std::make_unique<anim::State>(anim::StopImmediate
    {
        .exitWhen = std::move(exitWhen),
        .exitTo = exitTo
    });
}

void SkeletalAnimator::CompleteFirstRun()
{
    if (m_immediateState && m_immediateState->action == anim::Action::PlayOnce)
    {
        *m_immediateState->firstRunComplete.get() = true;
    }

    auto* activeState = m_states.Get(m_activeState);
    if (activeState && activeState->action == anim::Action::PlayOnce)
    {
        *activeState->firstRunComplete.get() = true;
    }
}

auto SkeletalAnimator::ExecuteRootState() -> bool
{
    if (!m_rootStateSet)
    {
        m_onStateChanged.Emit(anim::StateChange
        {
            .meshUid = m_meshUid,
            .prevAnimUid = std::string{},
            .curAnimUid = m_states.Get(m_activeState)->animUid,
            .action = anim::Action::Loop,
            .entity = ParentEntity()
        });
        m_rootStateSet = true;
        return true;
    }
    return false;
}

auto SkeletalAnimator::ExecuteImmediateState() -> bool
{
    if (m_immediateState)
    {
        if (!m_immediateStateSet)
        {
            m_immediateStateSet = true;
            m_onStateChanged.Emit(anim::StateChange
            {
                .meshUid = m_meshUid,
                .prevAnimUid =  m_states.Get(m_activeState)->animUid,
                .curAnimUid = m_immediateState->animUid,
                .action = m_immediateState->action,
                .entity = ParentEntity()
            });
            return true;
        }
        if (m_immediateState->exitWhen())
        {
            m_activeState = m_immediateState->exitTo;

            m_onStateChanged.Emit(anim::StateChange
            {
                .meshUid = m_meshUid,
                .prevAnimUid = m_immediateState->animUid,
                .curAnimUid = m_states.Get(m_activeState)->animUid,
                .action = m_states.Get(m_activeState)->action,
                .entity = ParentEntity()
            });

            m_immediateStateSet = false;
            m_immediateState = nullptr;
            return true;
        }
        return true; // Trapped in the immediate state until its exit condition is met
    }
    return false;
}

auto SkeletalAnimator::ExecuteExitState() -> bool
{
    if (m_states.Get(m_activeState)->exitWhen())
    {
        auto exitToState = m_states.Get(m_states.Get(m_activeState)->exitTo);
        m_onStateChanged.Emit(anim::StateChange
        {
            .meshUid = m_meshUid,
            .prevAnimUid = m_states.Get(m_activeState)->animUid,
            .curAnimUid = exitToState->animUid,
            .action = exitToState->action,
            .entity = ParentEntity()
        });
        m_activeState = exitToState->id;
        return true;
    }
    return false;
}

auto SkeletalAnimator::ExecuteChildEnterState(uint32_t childStateHandle) -> bool
{
    auto childState = m_states.Get(childStateHandle);
    if (childState->enterWhen())
    {
        m_onStateChanged.Emit(anim::StateChange
        {
            .meshUid = m_meshUid,
            .prevAnimUid = m_states.Get(m_activeState)->animUid,
            .curAnimUid = childState->animUid,
            .action = childState->action,
            .entity = ParentEntity()
        });
        m_activeState = childState->id;
        return true;
    }
    return false;
}

auto SkeletalAnimator::AddState(anim::StopImmediate stopImmediateProperties) -> uint32_t
{
    auto state = anim::State{std::move(stopImmediateProperties)};
    state.animUid = m_states.Get(m_activeState)->animUid;
    const auto addedStateHandle = m_states.Insert(std::move(state));
    auto* addedState = m_states.GetLast();
    m_states.Get(addedState->enterFrom)->AddSuccessor(addedStateHandle);
    return addedStateHandle;
}
} // namespace nc::graphics

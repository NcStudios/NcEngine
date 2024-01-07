#include "graphics/SkeletalAnimator.h"
#include "graphics/MeshRenderer.h"
#include "graphics/ToonRenderer.h"
#include "ecs/Registry.h"

namespace nc::graphics
{
SkeletalAnimator::SkeletalAnimator(Entity entity, std::string meshUid, std::string animationUid)
    : ComponentBase{entity},
      m_states{MaxStates},
      m_activeState{m_states.Insert(anim::State{anim::Initial{animationUid}})},
      m_meshUid{std::move(meshUid)},
      m_immediateState{},
      m_onPlayStateChanged{},
      m_immediateStateInitialized{},
      m_initialStateInitialized{},
      m_initialState{anim::State{anim::Initial{animationUid}}}
{
}

void SkeletalAnimator::Execute()
{
    if (ExecuteInitialState()) { return; }
    if (ExecuteImmediateState()) { return; }
    if (ExecuteExitState()) { return; }

    for (auto childStateHandle : m_states.Get(m_activeState)->successors)
    {
        if (ExecuteChildEnterState(childStateHandle)) { return; }
    }
}

auto SkeletalAnimator::AddState(anim::Loop loopProperties) -> uint32_t
{
    auto addedStateHandle = m_states.Insert(anim::State{std::move(loopProperties)});
    auto* addedState = m_states.GetLast();
    m_states.Get(addedState->enterFrom)->AddSuccessor(addedStateHandle);
    return addedStateHandle;
}

auto SkeletalAnimator::AddState(anim::PlayOnce playOnceProperties) -> uint32_t
{
    auto firstRunComplete = std::make_unique<bool>(false);
    auto addedStateHandle = m_states.Insert(anim::State{std::move(playOnceProperties)});
    auto* addedState = m_states.GetLast();
    m_states.Get(addedState->enterFrom)->AddSuccessor(addedStateHandle);
    return addedStateHandle;
}

auto SkeletalAnimator::AddState(anim::Stop stopProperties) -> uint32_t
{
    auto state = anim::State{std::move(stopProperties)};
    state.animUid = m_states.Get(m_activeState)->animUid;
    auto addedStateHandle = m_states.Insert(std::move(state));
    auto* addedState = m_states.GetLast();
    m_states.Get(addedState->enterFrom)->AddSuccessor(addedStateHandle);
    return addedStateHandle;
}

void SkeletalAnimator::LoopImmediate(std::string animUid, std::function<bool ()> exitWhen, uint32_t exitTo)
{
    m_immediateState = anim::State(anim::Loop
    {
        .enterFrom = anim::State::NullId,
        .enterWhen = [](){ return false; },
        .animUid = std::move(animUid),
        .exitWhen = std::move(exitWhen),
        .exitTo = exitTo
    });
}

void SkeletalAnimator::PlayOnceImmediate(std::string animUid, uint32_t exitTo)
{
    m_immediateState = anim::State(anim::PlayOnce
    {
        .enterFrom = anim::State::NullId,
        .enterWhen = [](){ return false; },
        .animUid = std::move(animUid),
        .exitTo = exitTo
    });
}

void SkeletalAnimator::StopImmediate(std::function<bool ()> exitWhen, uint32_t exitTo)
{
    m_immediateState = anim::State(anim::StopImmediate
    {
        .exitWhen = std::move(exitWhen),
        .exitTo = exitTo
    });
}

void SkeletalAnimator::CompleteFirstRun()
{
    if (m_immediateState.has_value() && m_immediateState.value().action == anim::Action::PlayOnce)
    {
        *m_immediateState.value().firstRunComplete.get() = true;
    }

    if (m_states.Get(m_activeState) && m_states.Get(m_activeState)->action == anim::Action::PlayOnce)
    {
        *m_states.Get(m_activeState)->firstRunComplete.get() = true;
    }
}

auto SkeletalAnimator::ExecuteInitialState() -> bool
{
    if (!m_initialStateInitialized)
    {
        m_onPlayStateChanged.Emit(anim::PlayState
        {
            .meshUid = m_meshUid,
            .prevAnimUid = std::string{},
            .curAnimUid = m_states.Get(m_activeState)->animUid,
            .action = anim::Action::Loop,
            .entity = ParentEntity()
        });
        m_initialStateInitialized = true;
        return true;
    }
    return false;
}

// auto SkeletalAnimator::ExecuteInitialState() -> bool
// {
//     if (!m_initialStateInitialized)
//     {
//         m_onPlayStateChanged.Emit(anim::PlayState
//         {
//             .meshUid = m_meshUid,
//             .prevAnimUid = std::string{},
//             .curAnimUid = m_initialState.animUid,
//             .action = m_initialState.action,
//             .entity = ParentEntity()
//         });
//         m_initialStateInitialized = true;
//         return true;
//     }
//     return false;
// }

auto SkeletalAnimator::ExecuteImmediateState() -> bool
{
    if (m_immediateState.has_value())
    {
        if (!m_immediateStateInitialized)
        {
            m_immediateStateInitialized = true;
            m_onPlayStateChanged.Emit(anim::PlayState
            {
                .meshUid = m_meshUid,
                .prevAnimUid =  m_states.Get(m_activeState)->animUid,
                .curAnimUid = m_immediateState.value().animUid,
                .action = m_immediateState.value().action,
                .entity = ParentEntity()
            });
            return true;
        }
        if (m_immediateState.value().exitWhen())
        {
            m_activeState = m_immediateState.value().exitTo;

            m_onPlayStateChanged.Emit(anim::PlayState
            {
                .meshUid = m_meshUid,
                .prevAnimUid = m_immediateState.value().animUid,
                .curAnimUid = m_states.Get(m_activeState)->animUid,
                .action = m_states.Get(m_activeState)->action,
                .entity = ParentEntity()
            });

            m_immediateStateInitialized = false;
            m_immediateState = std::nullopt;
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
        m_onPlayStateChanged.Emit(anim::PlayState
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
        m_onPlayStateChanged.Emit(anim::PlayState
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
    auto addedStateHandle = m_states.Insert(std::move(state));
    auto* addedState = m_states.GetLast();
    m_states.Get(addedState->enterFrom)->AddSuccessor(addedStateHandle);
    return addedStateHandle;
}
} // namespace nc::graphics

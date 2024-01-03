#include "graphics/SkeletalAnimator.h"
#include "graphics/MeshRenderer.h"
#include "graphics/ToonRenderer.h"
#include "ecs/Registry.h"

namespace
{
namespace nc::graphics
{
SkeletalAnimator::SkeletalAnimator(Entity entity, std::string meshUid, std::string animationUid)
    : ComponentBase{entity},
      m_meshUid{meshUid},
      m_onPlayStateChanged{},
      m_initialState{anim::State{anim::Initial{std::move(animationUid)}}},
      m_initialStateInitialized{}{}

void SkeletalAnimator::Execute()
{
    if (ExecuteInitialState())
    {
        return;
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
            .curAnimUid = m_initialState.animUid,
            .action = m_initialState.action,
            .entity = ParentEntity()
        });
        m_initialStateInitialized = true;
        return true;
    }
    return false;
}

void SkeletalAnimator::CompleteFirstRun()
{
    /* @todo : Truncation point for this PR. */
}
} // namespace nc::graphics

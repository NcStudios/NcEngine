#include "graphics/SkeletalAnimator.h"
#include "graphics/MeshRenderer.h"
#include "graphics/ToonRenderer.h"
#include "ecs/Registry.h"

namespace
{
auto GetMeshPath(nc::Entity entity) -> std::string
{
    auto* registry = nc::ActiveRegistry();
    auto* meshRenderer = registry->Get<nc::graphics::MeshRenderer>(entity);
    auto* toonRenderer = registry->Get<nc::graphics::ToonRenderer>(entity);
    if (meshRenderer)
    {
        return meshRenderer->GetMeshPath();
    }
    else if (toonRenderer)
    {
        return toonRenderer->GetMeshPath();
    }
    else
    {
        throw nc::NcError("SkeletalAnimator must be added to an Entity with either a ToonRenderer or a MeshRenderer.");
    }
}
}

namespace nc::graphics
{
SkeletalAnimator::SkeletalAnimator(Entity entity, std::string animUid)
    : ComponentBase{entity},
      m_meshUid{GetMeshPath(entity)},
      m_onPlayStateChanged{},
      m_initialState{anim::State{anim::Initial{std::move(animUid)}}},
      m_initialStateInitialized{}{}

SkeletalAnimator::SkeletalAnimator(SkeletalAnimator&& other) noexcept = default;
auto SkeletalAnimator::operator=(SkeletalAnimator&& other) noexcept -> SkeletalAnimator& = default;
SkeletalAnimator::~SkeletalAnimator() noexcept = default;

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

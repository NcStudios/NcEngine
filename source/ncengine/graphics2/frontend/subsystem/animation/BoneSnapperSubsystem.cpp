#include "BoneSnapperSubsystem.h"
#include "ncengine/debug/Profile.h"

namespace nc::graphics
{
void BoneSnapperSubsystem::Update(ecs::ExplicitEcs<BoneSnapper, SkinnedMesh, Transform> ecs)
{
    NC_PROFILE_TASK("BoneSnapperSubsystem::Update()", ProfileCategory::Animation);
    // auto& pool = ecs.GetPool<BoneSnapper>();
    ecs.GetPool<BoneSnapper>();
}
}
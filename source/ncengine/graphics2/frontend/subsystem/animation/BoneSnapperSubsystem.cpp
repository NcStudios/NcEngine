#include "BoneSnapperSubsystem.h"
#include "ncengine/debug/Profile.h"
#include "ncengine/utility/Log.h"

#include <ranges>

namespace nc::graphics
{
void BoneSnapperSubsystem::Update(ecs::ExplicitEcs<BoneSnapper, SkinnedMesh, Transform> ecs)
{
    NC_PROFILE_TASK("BoneSnapperSubsystem::Update()", ProfileCategory::Animation);

    auto& boneSnappers = ecs.GetPool<BoneSnapper>();
    for (auto& boneSnapper : boneSnappers)
    {
        auto sourceEntity =  ecs.GetParent(&boneSnapper);
        auto targetEntity = boneSnapper.target;

        if (sourceEntity.IsStatic())
        {
            continue;
        }

        if(!ecs.Contains<SkinnedMesh>(targetEntity))
        {
            continue;
        }

        const auto& skinnedMesh = ecs.Get<SkinnedMesh>(targetEntity);
        if (!m_skeletalAnimationSubsystem.ContainsBone(skinnedMesh.GetMeshId(), boneSnapper.boneName))
        {
            continue;
        }

        const auto& targetBoneTransform = m_skeletalAnimationSubsystem.GetBoneSnapperOffset(boneSnapper.target);
        auto& sourceTransform = ecs.Get<Transform>(sourceEntity);
        const auto sourceLocalScale = sourceTransform.LocalScale();

        sourceTransform.SetTransformationMatrix(targetBoneTransform);
        sourceTransform.SetScale(sourceLocalScale);
    }
}
}
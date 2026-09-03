#include "BoneSnapperSubsystem.h"
#include "ncengine/debug/Profile.h"
#include "ncengine/utility/Log.h"

#include <ranges>

namespace nc::graphics
{
void BoneSnapperSubsystem::Update(ecs::ExplicitEcs<BoneSnapper, SkinnedMesh, Transform> ecs)
{
    NC_PROFILE_TASK("BoneSnapperSubsystem::Update()", ProfileCategory::Animation);

#ifdef NC_BUILD_TESTS
        m_numUpdatedBoneSnappers = 0u;
#endif

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

        const auto& targetBoneTransform = m_skeletalAnimationSubsystem.GetAnimatedBone(skinnedMesh.GetMeshId(), boneSnapper.boneName);
        auto& sourceTransform = ecs.Get<Transform>(sourceEntity);
        const auto sourceLocalScale = sourceTransform.LocalScale();

        sourceTransform.SetTransformationMatrix(DirectX::XMMatrixIdentity());
        sourceTransform.SetTransformationMatrix(sourceTransform.LocalTransformationMatrix() * targetBoneTransform);
        sourceTransform.SetScale(sourceLocalScale);

#ifdef NC_BUILD_TESTS
        m_numUpdatedBoneSnappers++;
#endif
    }
}
}
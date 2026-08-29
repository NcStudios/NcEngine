#include "BoneSnapperSubsystem.h"
#include "ncengine/debug/Profile.h"
#include "ncengine/utility/Log.h"

#include <ranges>

namespace nc::graphics
{
void BoneSnapperSubsystem::Update(ecs::ExplicitEcs<BoneSnapper, SkinnedMesh, Transform> ecs)
{
    NC_PROFILE_TASK("BoneSnapperSubsystem::Update()", ProfileCategory::Animation);

    // For every bone snapper:
    // Get the SkinnedMesh component. Skip if there isn't one (todo: blow in constructor?)
    // Get the bone storage, for the computed bone by name. Skip if the name doesn't exist (Expose method)
    // Walk the tree, applying the bone matrix from bone storage against root and then children transforms a la EcsImpl

    // Entity
    //     BoneSnapper
    //         "Socket01",
    //         MyOtherEntity
    
    auto& boneSnappers = ecs.GetPool<BoneSnapper>();
    for (auto& boneSnapper : boneSnappers)
    {
        auto parentEntity =  ecs.GetParent(&boneSnapper);
        if (parentEntity.IsStatic())
        {
            throw nc::NcError("A BoneSnapper component cannot transform a static entity.");
        }

        if(!ecs.Contains<SkinnedMesh>(boneSnapper.target))
        {
            throw nc::NcError("Target entity on BoneSnapper must contain a valid SkinnedMesh component");
        }

        const auto& skinnedMesh = ecs.Get<SkinnedMesh>(boneSnapper.target);
        if (!m_skeletalAnimationSubsystem.ContainsBone(skinnedMesh.GetMeshId(), boneSnapper.boneName))
        {
            throw nc::NcError("The bone specified was not present in the chosen SkinnedMesh component. This BoneSnapper will be skipped.");
        }

        const auto& animatedBoneTransform = m_skeletalAnimationSubsystem.GetAnimatedBone(skinnedMesh.GetMeshId(), boneSnapper.boneName).animatedBoneMatrix;
        auto& transform = ecs.Get<Transform>(parentEntity);

        transform.SetTransformationMatrix(DirectX::XMMatrixIdentity());
        transform.SetTransformationMatrix(transform.LocalTransformationMatrix() * animatedBoneTransform);
    }
}
}
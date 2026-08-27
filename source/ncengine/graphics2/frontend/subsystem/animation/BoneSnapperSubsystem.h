#pragma once

#include "SkeletalAnimationSubsystem.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/BoneSnapper.h"

namespace nc::graphics
{
class BoneSnapperSubsystem
{
    public:
        BoneSnapperSubsystem(const ISkeletalAnimationSubsystem& ) {}
    
        /** Must run after the animations have been calculated and transforms have been moved. */
        void Update(ecs::ExplicitEcs<BoneSnapper, SkinnedMesh, Transform> ecs);
};
} // namespace nc::graphics
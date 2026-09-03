#pragma once

#include "SkeletalAnimationSubsystem.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/BoneSnapper.h"

namespace nc::graphics
{
class BoneSnapperSubsystem
{
    public:
        BoneSnapperSubsystem(ISkeletalAnimationSubsystem& skeletalAnimationSubsystem)
            : m_skeletalAnimationSubsystem{skeletalAnimationSubsystem}
#ifdef NC_BUILD_TESTS
            ,m_numUpdatedBoneSnappers{0u}
#endif
            {}
    
        /** Must run after the animations have been calculated and before transforms have been applied. */
        void Update(ecs::ExplicitEcs<BoneSnapper, SkinnedMesh, Transform> ecs);
#ifdef NC_BUILD_TESTS
        auto GetNumUpdatedBoneSnappers() -> uint32_t { return m_numUpdatedBoneSnappers; }
#endif

    private:
        ISkeletalAnimationSubsystem& m_skeletalAnimationSubsystem;
#ifdef NC_BUILD_TESTS
        uint32_t m_numUpdatedBoneSnappers;
#endif
};
} // namespace nc::graphics
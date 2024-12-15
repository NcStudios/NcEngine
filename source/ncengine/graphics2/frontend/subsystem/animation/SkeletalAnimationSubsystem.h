#pragma once

#include "BoneCache.h"
#include "SkeletalAnimationStorage.h"

namespace nc::graphics
{
class SkeletalAnimationSubsystem
{
    public:
        explicit SkeletalAnimationSubsystem(uint32_t maxBones)
            : m_boneCache{maxBones}
        {
        }

        auto GetStorage() -> SkeletalAnimationStorage& { return m_storage; }
        auto GetBoneCacheStaging() -> BoneCacheStaging& { return m_boneCache.GetStagingArea(); }

        void OnBeforeSceneLoad();

    private:
        SkeletalAnimationStorage m_storage;
        BoneCache m_boneCache;
};
} // namespace nc::graphics

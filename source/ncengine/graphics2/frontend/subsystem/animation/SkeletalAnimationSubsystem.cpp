#include "SkeletalAnimationSubsystem.h"

namespace nc::graphics
{
void SkeletalAnimationSubsystem::OnBeforeSceneLoad()
{
    m_boneCache.Purge();
}
} // namespace nc::graphics

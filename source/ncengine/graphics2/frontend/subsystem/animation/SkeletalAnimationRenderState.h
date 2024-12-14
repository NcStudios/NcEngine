#pragma once

#include "graphics2/ShaderTypes.h"

namespace nc::graphics
{
struct SkeletalAnimationRenderState
{
    BufferUpdateInfo<BoneData> boneData;
};
} // namespace nc::graphics

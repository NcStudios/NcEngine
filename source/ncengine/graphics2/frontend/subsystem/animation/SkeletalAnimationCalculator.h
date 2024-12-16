#pragma once

#include "Rig.h"
#include "graphics2/ShaderTypes.h"

#include "ncengine/ecs/Component.h"
#include "ncasset/Assets.h"
#include "ncmath/MatrixUtilities.h"

#include <limits>

namespace nc::graphics
{
// Animate context object. animatedBones serves as the primary output, but all fields are valid upon completion. Context
// state will be overwritten on each call to Animate(), allowing allocations be reused, if sufficiently sized.
struct SkeletalAnimationContext
{
    std::vector<BoneData> animatedBones;
    std::vector<DirectX::XMMATRIX> offsets;
    std::vector<DecomposedMatrixXM> fromOffsetsDecomposed;
    std::vector<DecomposedMatrixXM> toOffsetsDecomposed;
};

// Calculate animated bones for a single animation.
void Animate(SkeletalAnimationContext ctx,
             const Rig& rig,
             const asset::SkeletalAnimation& animation,
             float timeInTicks);

// Calculated animated bones for two blended animations.
void Animate(SkeletalAnimationContext ctx,
             const Rig& rig,
             const asset::SkeletalAnimation& blendFromAnimation,
             float blendFromTicks,
             const asset::SkeletalAnimation& blendToAnimation,
             float blendToTicks,
             float blendFactor);
} // namespace nc::graphics

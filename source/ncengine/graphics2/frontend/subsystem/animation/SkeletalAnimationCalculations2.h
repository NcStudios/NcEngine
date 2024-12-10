#pragma once

#include "SkeletalAnimationTypes.h"
#include "graphics2/ShaderTypes.h"

#include "ncengine/asset/AssetData.h"
#include "DirectXMath.h"

#include <vector>

namespace nc::graphics::gfx2
{
auto GetAnimationOffsets(float timeInTicks,
                         const std::vector<std::string>& boneNames,
                         const asset::SkeletalAnimation& animation) -> PackedAnimationDecomposed;

auto ComposeMatrices(float timeInTicks,
                     const std::vector<std::string>& boneNames,
                     const asset::SkeletalAnimation& animation) -> PackedAnimation;

auto ComposeBlendedMatrices(float blendFromTime,
                            float blendToTime,
                            float blendFactor,
                            const std::vector<std::string>& boneNames,
                            const asset::SkeletalAnimation& blendFromAnim,
                            const asset::SkeletalAnimation& blendToAnim) -> PackedAnimation;

void AnimateBones(const PackedRig& rig,
                  const PackedAnimation& anim,
                  std::vector<BoneData>& bonesOut);

auto GetInterpolatedPosition(float timeInTicks, const std::vector<asset::PositionFrame>& positionFrames) -> Vector3;
auto GetInterpolatedRotation(float timeInTicks, const std::vector<asset::RotationFrame>& rotationFrames) -> Quaternion;
auto GetInterpolatedScale(float timeInTicks, const std::vector<asset::ScaleFrame>& scaleFrames) -> Vector3;
}

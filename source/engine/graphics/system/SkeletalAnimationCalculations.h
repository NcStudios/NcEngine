#pragma once

#include "graphics/shader_resource/SkeletalAnimationData.h"
#include "graphics/system/SkeletalAnimationTypes.h"

#include "asset/AssetData.h"
#include "DirectXMath.h"

#include <vector>

namespace nc::graphics
{
auto GetAnimationOffsets(float timeInTicks,
                         const std::vector<std::string>& boneNames,
                         const nc::asset::SkeletalAnimation* animation) -> anim::PackedAnimationDecomposed;

auto ComposeMatrices(float timeInTicks,
                     const std::vector<std::string>& boneNames,
                     const nc::asset::SkeletalAnimation* animation) -> anim::PackedAnimation;

auto ComposeBlendedMatrices(float blendFromTime,
                            float blendToTime,
                            float blendFactor,
                            const std::vector<std::string>& boneNames,
                            const nc::asset::SkeletalAnimation* blendFromAnim,
                            const nc::asset::SkeletalAnimation* blendToAnim) -> anim::PackedAnimation;

auto AnimateBones(const anim::PackedRig& rig,
                  const anim::PackedAnimation& anim) -> std::vector<nc::graphics::SkeletalAnimationData>;

auto GetInterpolatedPosition(float timeInTicks, const std::vector<nc::asset::PositionFrame> positionFrames) -> nc::Vector3;
auto GetInterpolatedRotation(float timeInTicks, const std::vector<nc::asset::RotationFrame> rotationFrames) -> nc::Quaternion;
auto GetInterpolatedScale(float timeInTicks, const std::vector<nc::asset::ScaleFrame> scaleFrames) -> nc::Vector3;
}
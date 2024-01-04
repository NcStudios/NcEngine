#include "SkeletalAnimationCalculations.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace nc::graphics
{
using namespace DirectX;
using namespace nc;

XMVECTOR ToXMVector(const Quaternion& q)         { return XMVectorSet(q.x, q.y, q.z, q.w); }
XMMATRIX ToTransMatrix(const Vector3& v)         { return XMMatrixTranslation(v.x, v.y, v.z); }
XMMATRIX ToScaleMatrix(const Vector3& v)         { return XMMatrixScaling(v.x, v.y, v.z); }
XMMATRIX ToRotMatrix(const Quaternion& q)        { return XMMatrixRotationQuaternion(ToXMVector(q)); }

auto GetAnimationOffsets(float timeInTicks,
                         const std::vector<std::string>& boneNames,
                         const nc::asset::SkeletalAnimation* animation) -> anim::PackedAnimationDecomposed
{
    auto animationMatrices = anim::PackedAnimationDecomposed{};
    animationMatrices.offsets.reserve(boneNames.size());
    animationMatrices.hasValues.reserve(boneNames.size());

    for (const auto& boneName : boneNames)
    {
        auto iter = animation->framesPerBone.find(boneName);
        if (iter == animation->framesPerBone.end())
        {
           animationMatrices.offsets.push_back(anim::DecomposedMatrix{});
           animationMatrices.hasValues.push_back(0);
           continue;
        }
        
        animationMatrices.offsets.emplace_back(anim::DecomposedMatrix
        {
            .pos =   GetInterpolatedPosition(timeInTicks, iter->second.positionFrames),
            .rot =   GetInterpolatedRotation(timeInTicks, iter->second.rotationFrames),
            .scale = GetInterpolatedScale(timeInTicks, iter->second.scaleFrames)
        });
        animationMatrices.hasValues.push_back(1);
    }
    return animationMatrices;
}

auto ComposeMatrices(float timeInTicks,
                     const std::vector<std::string>& boneNames,
                     const nc::asset::SkeletalAnimation* animation) -> anim::PackedAnimation
{
    auto decomposedAnimation = GetAnimationOffsets(timeInTicks, boneNames, animation);

    auto packedAnimation = anim::PackedAnimation{};
    auto elementsCount = decomposedAnimation.offsets.size();
    packedAnimation.offsets.reserve(elementsCount);
    packedAnimation.hasValues = std::vector<anim::HasValue>{decomposedAnimation.hasValues.begin(), decomposedAnimation.hasValues.end()};

    std::ranges::transform(decomposedAnimation.offsets, std::back_inserter(packedAnimation.offsets), [](auto&& offset)
    {
        return ToScaleMatrix(offset.scale) * ToRotMatrix(offset.rot) * ToTransMatrix(offset.pos);
    });

    return packedAnimation;
}

auto ComposeBlendedMatrices(float blendFromTime,
                            float blendToTime,
                            float blendFactor,
                            const std::vector<std::string>& boneNames,
                            const nc::asset::SkeletalAnimation* blendFromAnim,
                            const nc::asset::SkeletalAnimation* blendToAnim) -> anim::PackedAnimation
{
    auto blendFromDecomposed = GetAnimationOffsets(blendFromTime, boneNames, blendFromAnim);
    auto blendToDecomposed = GetAnimationOffsets(blendToTime, boneNames, blendToAnim);

    auto packedAnimation = anim::PackedAnimation{};
    packedAnimation.hasValues = std::vector<anim::HasValue>{blendFromDecomposed.hasValues.begin(), blendFromDecomposed.hasValues.end()};

    auto interpolate = [blendFactor](auto& blendFromOffset, auto& blendToOffset)
    {
        return ToScaleMatrix(Lerp(blendFromOffset.scale, blendToOffset.scale, blendFactor)) *
               ToRotMatrix(Normalize(Slerp(blendFromOffset.rot, blendToOffset.rot, blendFactor))) *
               ToTransMatrix(Lerp(blendFromOffset.pos, blendToOffset.pos, blendFactor));
    };

    auto offsets = std::views::zip_transform(interpolate, blendFromDecomposed.offsets, blendToDecomposed.offsets);
    packedAnimation.offsets = std::vector<DirectX::XMMATRIX>{offsets.begin(), offsets.end()};
    return packedAnimation;
}

auto AnimateBones(const anim::PackedRig& rig,
                  const anim::PackedAnimation& anim) -> std::vector<nc::graphics::SkeletalAnimationData>
{
    // Copy the boneToParent vector to perform modifications in place.
    auto boneToParentSandbox = std::vector<DirectX::XMMATRIX>{rig.boneToParent.begin(), rig.boneToParent.end()};
    auto boneToParentSandboxSize = boneToParentSandbox.size();

    // Replace each boneToParent offset with its animation offset, if present. Else, leave as the original offset.
    for (auto i = 0u; i < boneToParentSandboxSize; i++)
    {
        boneToParentSandbox[i] = (boneToParentSandbox[i] * !anim.hasValues[i]) + (anim.offsets[i] * anim.hasValues[i]);
    }

    // Multiply each child (siblings are contiguous) with its parent.
    for (auto i = 0u; i < boneToParentSandboxSize; i++)
    {
        auto [parentFirstChild, parentChildren] = rig.boneToParentIndices[i];

        for (auto& j : std::views::counted(boneToParentSandbox.begin() + parentFirstChild, parentChildren)) // Update the child to be itself times its parent offset above. This incorporates recursive parent/animation data into the offsets vector.
        {
            j = j * boneToParentSandbox[i];
        }
    }

    // Create the output vector.
    auto animatedBones = std::vector<nc::graphics::SkeletalAnimationData>{};
    animatedBones.reserve(rig.vertexToBone.size());

    // Create a final transform for each bone by multiplying the (vertex-space-to-bone-space matrix) with the (bone-space-to-animated-parent-bone-space matrix) with the (global inverse transform matrix).
    // This outputs a matrix that can be used to transform a vertex into its final animated position.
    auto globalInverseTransform = rig.globalInverseTransform;
    auto vertexToBoneSize = rig.vertexToBone.size();
    for (auto i = 0u; i < vertexToBoneSize; i++)
    {
        animatedBones.push_back(nc::graphics::SkeletalAnimationData{rig.vertexToBone[i] * boneToParentSandbox[rig.offsetsMap[i]] * globalInverseTransform});
    }
    return animatedBones;
}

auto GetInterpolatedPosition(float timeInTicks, const std::vector<nc::asset::PositionFrame>& positionFrames) -> nc::Vector3
{
    NC_ASSERT(positionFrames.size() > 0, "Animation has no position data for the node.");
    if (positionFrames.size() == 1) return positionFrames[0].position;

    auto nextFramePos = std::find_if(positionFrames.begin(), positionFrames.end(), [timeInTicks](auto&& frame){ return timeInTicks < frame.timeInTicks;});

    NC_ASSERT(nextFramePos != positionFrames.end() && nextFramePos != positionFrames.begin(), fmt::format("Animation has no position data at time {}", timeInTicks));
    auto positionIndex = static_cast<size_t>(std::distance(positionFrames.begin(), std::prev(nextFramePos)));

    auto nextPositionIndex = positionIndex + 1;
    NC_ASSERT(nextPositionIndex <= positionFrames.size(), "Animation interpolation index has exceeded the bounds of the frame data.");

    auto deltaTimeInTicks = positionFrames[nextPositionIndex].timeInTicks - positionFrames[positionIndex].timeInTicks;
    auto interpolationFactor = (timeInTicks - positionFrames[positionIndex].timeInTicks) / deltaTimeInTicks;
    NC_ASSERT(interpolationFactor >= 0.0f && interpolationFactor <= 1.0f, fmt::format("Error calculating the interpolation factor: {}", interpolationFactor));

    return Lerp(positionFrames[positionIndex].position, positionFrames[nextPositionIndex].position, static_cast<float>(interpolationFactor));
}

auto GetInterpolatedRotation(float timeInTicks, const std::vector<nc::asset::RotationFrame>& rotationFrames) -> nc::Quaternion
{
    NC_ASSERT(rotationFrames.size() > 0, "Animation has no rotation data for the node.");
    if (rotationFrames.size() == 1) return Normalize(rotationFrames[0].rotation);

    auto nextFramePos = std::find_if(rotationFrames.begin(), rotationFrames.end(), [timeInTicks](auto&& frame){ return timeInTicks < frame.timeInTicks; });

    NC_ASSERT(nextFramePos != rotationFrames.end() && nextFramePos != rotationFrames.begin(), fmt::format("Animation has no rotation data at time {}", timeInTicks));
    auto rotationIndex = static_cast<size_t>(std::distance(rotationFrames.begin(), std::prev(nextFramePos)));

    auto nextRotationIndex = rotationIndex + 1;
    NC_ASSERT(nextRotationIndex <= rotationFrames.size(), "Animation interpolation index has exceeded the bounds of the frame data.");

    auto deltaTimeInTicks = rotationFrames[nextRotationIndex].timeInTicks - rotationFrames[rotationIndex].timeInTicks;
    auto interpolationFactor = (timeInTicks - rotationFrames[rotationIndex].timeInTicks) / deltaTimeInTicks;
    NC_ASSERT(interpolationFactor >= 0.0f && interpolationFactor <= 1.0f, fmt::format("Error calculating the interpolation factor: {}", interpolationFactor));

    return Normalize(Slerp(rotationFrames[rotationIndex].rotation, rotationFrames[nextRotationIndex].rotation, static_cast<float>(interpolationFactor)));
}

auto GetInterpolatedScale(float timeInTicks, const std::vector<nc::asset::ScaleFrame>& scaleFrames) -> nc::Vector3
{
    NC_ASSERT(scaleFrames.size() > 0, "Animation has no scale data for the node.");
    if (scaleFrames.size() == 1) return scaleFrames[0].scale;

    auto nextFramePos = std::find_if(scaleFrames.begin(), scaleFrames.end(), [timeInTicks](auto&& frame){ return timeInTicks < frame.timeInTicks; });

    NC_ASSERT(nextFramePos != scaleFrames.end() && nextFramePos != scaleFrames.begin(), fmt::format("Animation has no scale data at time {}", timeInTicks));
    auto scaleIndex = static_cast<size_t>(std::distance(scaleFrames.begin(), std::prev(nextFramePos)));

    auto nextScaleIndex = scaleIndex + 1;
    NC_ASSERT(nextScaleIndex <= scaleFrames.size(), "Animation interpolation index has exceeded the bounds of the frame data.");

    auto deltaTimeInTicks = scaleFrames[nextScaleIndex].timeInTicks - scaleFrames[scaleIndex].timeInTicks;
    auto interpolationFactor = (timeInTicks - scaleFrames[scaleIndex].timeInTicks) / deltaTimeInTicks;
    NC_ASSERT(interpolationFactor >= 0.0f && interpolationFactor <= 1.0f, fmt::format("Error calculating the interpolation factor: {}", interpolationFactor));

    return Lerp(scaleFrames[scaleIndex].scale, scaleFrames[nextScaleIndex].scale, static_cast<float>(interpolationFactor));
}
} // namespace nc::graphics

#include "SkeletalAnimationCalculations.h"

#include "ncmath/MatrixUtilities.h"
#include "ncutility/NcError.h"

#include <ranges>

namespace nc::graphics
{
using namespace DirectX;
using namespace nc;

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
        
        animationMatrices.offsets.emplace_back
        (
            GetInterpolatedPosition(timeInTicks, iter->second.positionFrames),
            GetInterpolatedRotation(timeInTicks, iter->second.rotationFrames),
            GetInterpolatedScale(timeInTicks, iter->second.scaleFrames)
        );
        animationMatrices.hasValues.push_back(1);
    }
    return animationMatrices;
}

auto ComposeMatrices(float timeInTicks,
                     const std::vector<std::string>& boneNames,
                     const nc::asset::SkeletalAnimation* animation) -> anim::PackedAnimation
{
    auto [offsets, hasValues] = GetAnimationOffsets(timeInTicks, boneNames, animation);
    auto packedAnimation = anim::PackedAnimation{ .offsets = std::vector<DirectX::XMMATRIX>{}, .hasValues = std::move(hasValues) };
    packedAnimation.offsets.reserve(packedAnimation.hasValues.size());

    std::ranges::transform(offsets, std::back_inserter(packedAnimation.offsets), [](auto&& offset)
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

    // Replace each boneToParent offset with its animation offset, if present. Else, leave as the original offset.
    for (auto&& [boneOffset, animOffset, animHasValue] : std::views::zip(boneToParentSandbox, anim.offsets, anim.hasValues))
    {
        boneOffset = boneOffset * !animHasValue + animOffset * animHasValue;
    }

    // Multiply each child (siblings are contiguous) with its parent.
    for (auto&& [parent, childrenIndex] : std::views::zip(boneToParentSandbox, rig.offsetChildren))
    {
        for (auto&& child : std::views::counted(boneToParentSandbox.begin()+ childrenIndex.indexOfFirstChild, childrenIndex.numChildren))
        {
            child = child * parent;
        }
    }

    // Create the output vector.
    auto animatedBones = std::vector<nc::graphics::SkeletalAnimationData>{};
    animatedBones.reserve(rig.vertexToBone.size());

    // Create a final transform for each bone by multiplying the (vertex-space-to-bone-space matrix) with the (bone-space-to-animated-parent-bone-space matrix) with the (global inverse transform matrix).
    // This outputs a matrix that can be used to transform a vertex into its final animated position.
    std::ranges::transform(
    std::views::zip(rig.vertexToBone, rig.offsetsMap),
    std::back_inserter(animatedBones),
        [globalInverseTransform = rig.globalInverseTransform, &boneToParentSandbox](auto&& in)
        {  
            auto&& [matrix, offset] = in;
            return SkeletalAnimationData{matrix * boneToParentSandbox.at(offset) * globalInverseTransform};
        }
    );
    return animatedBones;
}

auto GetInterpolatedPosition(float timeInTicks, const std::vector<nc::asset::PositionFrame>& positionFrames) -> nc::Vector3
{
    NC_ASSERT(positionFrames.size() > 0, "Animation has no position data for the node.");
    if (positionFrames.size() == 1) return positionFrames[0].position;

    const auto nextFrame = std::ranges::find_if(
        std::views::drop(positionFrames, 1),
        [timeInTicks](auto&& frame){ return timeInTicks < frame.timeInTicks;}
    );

    NC_ASSERT(nextFrame != positionFrames.end(), fmt::format("Animation has no position data at time {}", timeInTicks));
    const auto frame = std::prev(nextFrame);
    const auto deltaTimeInTicks = nextFrame->timeInTicks - frame->timeInTicks;
    const auto interpolationFactor = (timeInTicks - frame->timeInTicks) / deltaTimeInTicks;
    NC_ASSERT(interpolationFactor >= 0.0f && interpolationFactor <= 1.0f, fmt::format("Error calculating the interpolation factor: {}", interpolationFactor));

    return Lerp(frame->position, nextFrame->position, interpolationFactor);
}

auto GetInterpolatedRotation(float timeInTicks, const std::vector<nc::asset::RotationFrame>& rotationFrames) -> nc::Quaternion
{
    NC_ASSERT(rotationFrames.size() > 0, "Animation has no rotation data for the node.");
    if (rotationFrames.size() == 1) return Normalize(rotationFrames[0].rotation);

    const auto nextFrame = std::ranges::find_if(
        std::views::drop(rotationFrames, 1),
        [timeInTicks](auto&& frame){ return timeInTicks < frame.timeInTicks;}
    );

    NC_ASSERT(nextFrame != rotationFrames.end(), fmt::format("Animation has no rotation data at time {}", timeInTicks));
    const auto frame = std::prev(nextFrame);
    const auto deltaTimeInTicks = nextFrame->timeInTicks - frame->timeInTicks;
    const auto interpolationFactor = (timeInTicks - frame->timeInTicks) / deltaTimeInTicks;
    NC_ASSERT(interpolationFactor >= 0.0f && interpolationFactor <= 1.0f, fmt::format("Error calculating the interpolation factor: {}", interpolationFactor));

    return Normalize(Slerp(frame->rotation, nextFrame->rotation, interpolationFactor));
}

auto GetInterpolatedScale(float timeInTicks, const std::vector<nc::asset::ScaleFrame>& scaleFrames) -> nc::Vector3
{
    NC_ASSERT(scaleFrames.size() > 0, "Animation has no scale data for the node.");
    if (scaleFrames.size() == 1) return scaleFrames[0].scale;

    const auto nextFrame = std::ranges::find_if(
        std::views::drop(scaleFrames, 1),
        [timeInTicks](auto&& frame){ return timeInTicks < frame.timeInTicks;}
    );

    NC_ASSERT(nextFrame != scaleFrames.end(), fmt::format("Animation has no scale data at time {}", timeInTicks));
    const auto frame = std::prev(nextFrame);
    const auto deltaTimeInTicks = nextFrame->timeInTicks - frame->timeInTicks;
    const auto interpolationFactor = (timeInTicks - frame->timeInTicks) / deltaTimeInTicks;
    NC_ASSERT(interpolationFactor >= 0.0f && interpolationFactor <= 1.0f, fmt::format("Error calculating the interpolation factor: {}", interpolationFactor));

    return Lerp(frame->scale, nextFrame->scale, interpolationFactor);
}
} // namespace nc::graphics

#include "SkeletalAnimationCalculations2.h"

#include "ncmath/MatrixUtilities.h"
#include "ncutility/NcError.h"

#include <ranges>

namespace nc::graphics::gfx2
{
using namespace DirectX;

auto GetAnimationOffsets(float timeInTicks,
                         const std::vector<std::string>& boneNames,
                         const asset::SkeletalAnimation& animation) -> PackedAnimationDecomposed
{
    auto animationMatrices = PackedAnimationDecomposed{};
    animationMatrices.offsets.reserve(boneNames.size());
    animationMatrices.hasValues.reserve(boneNames.size());

    for (const auto& boneName : boneNames)
    {
        auto iter = animation.framesPerBone.find(boneName);
        if (iter == animation.framesPerBone.end())
        {
           animationMatrices.offsets.push_back(DecomposedMatrix{});
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
                     const asset::SkeletalAnimation& animation) -> PackedAnimation
{
    auto [offsets, hasValues] = GetAnimationOffsets(timeInTicks, boneNames, animation);
    auto packedAnimation = PackedAnimation{ .offsets = std::vector<DirectX::XMMATRIX>{}, .hasValues = std::move(hasValues) };
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
                            const asset::SkeletalAnimation& blendFromAnim,
                            const asset::SkeletalAnimation& blendToAnim) -> PackedAnimation
{
    auto [fromOffsets, fromHasValues] = GetAnimationOffsets(blendFromTime, boneNames, blendFromAnim);
    
    // todo: it doesn't look like hasValues is used on this, can refactor?
    auto [toOffsets, _] = GetAnimationOffsets(blendToTime, boneNames, blendToAnim);
    // auto [toOffsets, toHasValues] = GetAnimationOffsets(blendToTime, boneNames, blendToAnim);


    NC_ASSERT(fromOffsets.size() == toOffsets.size(), "fuuck");

    auto interpolate = [blendFactor](auto& blendFromOffset, auto& blendToOffset)
    {
        return ToScaleMatrix(Lerp(blendFromOffset.scale, blendToOffset.scale, blendFactor)) *
               ToRotMatrix(Normalize(Slerp(blendFromOffset.rot, blendToOffset.rot, blendFactor))) *
               ToTransMatrix(Lerp(blendFromOffset.pos, blendToOffset.pos, blendFactor));
    };

    auto offsets = std::views::zip_transform(interpolate, fromOffsets, toOffsets);


    // todo: is it ok to only care about hasValue from fromAnim?
    return gfx2::PackedAnimation{
        .offsets = std::vector<DirectX::XMMATRIX>{offsets.begin(), offsets.end()},
        //.hasValues = std::move(fromHasValues)
        .hasValues = std::move(_)
    };
}

void AnimateBones(const PackedRig& rig,
                  const PackedAnimation& anim,
                  std::vector<BoneData>& bonesOut)
{
    // Copy the boneToParent vector to perform modifications in place.
    auto boneToParentSandbox = rig.boneToParent;

    NC_ASSERT(boneToParentSandbox.size() == anim.offsets.size(), "fuuuck");

    // Replace each boneToParent offset with its animation offset, if present. Else, leave as the original offset.
    for (auto [boneOffset, animOffset, animHasValue] : std::views::zip(boneToParentSandbox, anim.offsets, anim.hasValues))
    {
        if (animHasValue)
            boneOffset = animOffset;
        // else
            // boneOffset = DirectX::XMMatrixSet(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

        // boneOffset = boneOffset * !animHasValue + animOffset * animHasValue;
    }

    NC_ASSERT(boneToParentSandbox.size() == rig.offsetChildren.size(), "fuuuck");


    // Multiply each child (siblings are contiguous) with its parent.
    for (auto [parent, childrenIndex] : std::views::zip(boneToParentSandbox, rig.offsetChildren))
    {
        for (auto& child : std::views::counted(boneToParentSandbox.begin() + childrenIndex.indexOfFirstChild, childrenIndex.numChildren))
        {
            child = XMMatrixMultiply(child, parent);
        }
    }

    bonesOut.reserve(rig.vertexToBone.size());

    // Create a final transform for each bone by multiplying the (vertex-space-to-bone-space matrix) with the (bone-space-to-animated-parent-bone-space matrix) with the (global inverse transform matrix).
    // This outputs a matrix that can be used to transform a vertex into its final animated position.
    std::ranges::transform(
        std::views::zip(rig.vertexToBone, rig.offsetsMap),
        std::back_inserter(bonesOut),
            [&globalInverseTransform = rig.globalInverseTransform, &boneToParentSandbox](const auto& in)
            {
                const auto& [matrix, offset] = in;
                return BoneData{matrix * boneToParentSandbox.at(offset) * globalInverseTransform};
            }
    );
}

template<class Iterator>
struct InterpolationFrames
{
    const Iterator from;
    const Iterator to;
};

template<class FrameType>
auto GetFrames(float timeInTicks, const std::vector<FrameType>& frames)
{
    // todo: can we bin search here?

    const auto to = std::ranges::find_if(
        std::views::drop(frames, 1),
        [timeInTicks](auto&& frame){
            return timeInTicks < frame.timeInTicks;
        }
    );

    return InterpolationFrames{std::prev(to), to};
}

auto GetInterpolationFactor(float timeInTicks, float frameTicks, float nextFrameTicks) -> float
{
    const auto deltaTimeInTicks = nextFrameTicks - frameTicks;
    return (timeInTicks - frameTicks) / deltaTimeInTicks;
}

auto GetInterpolatedPosition(float timeInTicks, const std::vector<asset::PositionFrame>& positionFrames) -> Vector3
{
    // could these load XMVECTORS? we interpolate here and later on, then construct matrices.

    if (positionFrames.size() > 1)
    {
        const auto [from, to] = GetFrames(timeInTicks, positionFrames);
        const auto factor = GetInterpolationFactor(timeInTicks, from->timeInTicks, to->timeInTicks);
        return Lerp(from->position, to->position, factor);
    }

    return positionFrames[0].position;
}

auto GetInterpolatedRotation(float timeInTicks, const std::vector<asset::RotationFrame>& rotationFrames) -> Quaternion
{
    // todo: need to normalize?

    if (rotationFrames.size() > 1)
    {
        const auto [from, to] = GetFrames(timeInTicks, rotationFrames);
        const auto factor = GetInterpolationFactor(timeInTicks, from->timeInTicks, to->timeInTicks);
        return Normalize(Slerp(from->rotation, to->rotation, factor));
    }

    return Normalize(rotationFrames[0].rotation);
}

auto GetInterpolatedScale(float timeInTicks, const std::vector<asset::ScaleFrame>& scaleFrames) -> Vector3
{
    if (scaleFrames.size() > 1)
    {
        const auto [from, to] = GetFrames(timeInTicks, scaleFrames);
        const auto factor = GetInterpolationFactor(timeInTicks, from->timeInTicks, to->timeInTicks);
        return Lerp(from->scale, to->scale, factor);
    }

    return scaleFrames[0].scale;
}
} // namespace nc::graphics

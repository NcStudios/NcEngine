#include "SkeletalAnimationCalculator.h"

#include "ncengine/debug/Profile.h"
#include "ncutility/NcError.h"

#include <ranges>

namespace
{
using namespace nc;
using namespace nc::graphics;
using namespace DirectX;

template<class Iterator>
struct InterpolationFrames
{
    const Iterator from;
    const Iterator to;
};

template<class FrameType>
auto GetFrames(float timeInTicks, const std::vector<FrameType>& frames)
{
    const auto to = std::ranges::lower_bound(
        std::views::drop(frames, 1),
        timeInTicks,
        std::less{},
        &FrameType::timeInTicks
    );

    return InterpolationFrames{std::prev(to), to};
}

auto GetInterpolationFactor(float timeInTicks, float frameTicks, float nextFrameTicks) -> float
{
    const auto deltaTimeInTicks = nextFrameTicks - frameTicks;
    return (timeInTicks - frameTicks) / deltaTimeInTicks;
}

auto GetInterpolatedPosition(float timeInTicks, const std::vector<asset::PositionFrame>& positionFrames) -> XMVECTOR
{
    if (positionFrames.size() > 1)
    {
        const auto [from, to] = GetFrames(timeInTicks, positionFrames);
        const auto factor = GetInterpolationFactor(timeInTicks, from->timeInTicks, to->timeInTicks);
        return XMVectorLerp(ToXMVectorHomogeneous(from->position), ToXMVectorHomogeneous(to->position), factor);
    }

    return ToXMVectorHomogeneous(positionFrames[0].position);
}

auto GetInterpolatedRotation(float timeInTicks, const std::vector<asset::RotationFrame>& rotationFrames) -> XMVECTOR
{
    if (rotationFrames.size() > 1)
    {
        const auto [from, to] = GetFrames(timeInTicks, rotationFrames);
        const auto factor = GetInterpolationFactor(timeInTicks, from->timeInTicks, to->timeInTicks);
        return XMQuaternionSlerp(ToXMVector(from->rotation), ToXMVector(to->rotation), factor);
    }

    return ToXMVector(rotationFrames[0].rotation);
}

auto GetInterpolatedScale(float timeInTicks, const std::vector<asset::ScaleFrame>& scaleFrames) -> XMVECTOR
{
    if (scaleFrames.size() > 1)
    {
        const auto [from, to] = GetFrames(timeInTicks, scaleFrames);
        const auto factor = GetInterpolationFactor(timeInTicks, from->timeInTicks, to->timeInTicks);
        return XMVectorLerp(ToXMVector(from->scale), ToXMVector(to->scale), factor);
    }

    return ToXMVector(scaleFrames[0].scale);
}

// Calculate offset matrices for a single (unblended) animation
void CalculateOffsets(const std::vector<std::string>& boneNames,
                      const asset::SkeletalAnimation& animation,
                      float timeInTicks,
                      std::vector<XMMATRIX>& offsetsOut,
                      std::vector<uint8_t>& hasValuesOut)
{
    // todo: can we just write boneToParentHere? if boneNames and boneToParent are in sync,
    // I think that gets rid of the whole 'hasValues' concept...

    for (const auto& boneName : boneNames)
    {
        const auto iter = animation.framesPerBone.find(boneName);
        if (iter != animation.framesPerBone.end())
        {
            offsetsOut.push_back(ComposeMatrix(
                GetInterpolatedScale(timeInTicks, iter->second.scaleFrames),
                GetInterpolatedRotation(timeInTicks, iter->second.rotationFrames),
                GetInterpolatedPosition(timeInTicks, iter->second.positionFrames)
            ));

            hasValuesOut.push_back(1);
            continue;
        }

        offsetsOut.push_back(XMMATRIX{});
        hasValuesOut.push_back(0);
    }
}

// Calculate decomposed offsets to be blended with another animation
void CalculateOffsets(const std::vector<std::string>& boneNames,
                      const asset::SkeletalAnimation& animation,
                      float timeInTicks,
                      std::vector<DecomposedMatrixXM>& offsetsOut,
                      std::vector<uint8_t>* hasValuesOut = nullptr)
{
    NC_PROFILE_SCOPE("CalculateOffsets()", ProfileCategory::Animation); // prob don't keep
    for (const auto& boneName : boneNames)
    {
        auto iter = animation.framesPerBone.find(boneName);
        if (iter != animation.framesPerBone.end())
        {
            offsetsOut.emplace_back(
                GetInterpolatedScale(timeInTicks, iter->second.scaleFrames),
                GetInterpolatedRotation(timeInTicks, iter->second.rotationFrames),
                GetInterpolatedPosition(timeInTicks, iter->second.positionFrames)
            );

            if (hasValuesOut)
                hasValuesOut->push_back(1);

            continue;
        }

        offsetsOut.push_back(DecomposedMatrixXM{});
        if (hasValuesOut)
            hasValuesOut->push_back(0);
    }
}

// Combine decomposed offsets of two animations
void BlendOffsets(const std::vector<DecomposedMatrixXM>& fromOffsets,
                  const std::vector<DecomposedMatrixXM>& toOffsets,
                  float blendFactor,
                  std::vector<XMMATRIX>& offsetsOut)
{
    NC_PROFILE_SCOPE("ComposeBlendedOffsets()", ProfileCategory::Animation); // prob don't keep

    NC_ASSERT(fromOffsets.size() == toOffsets.size(), "fuuck");

    std::ranges::transform(
        std::views::zip(fromOffsets, toOffsets),
        std::back_inserter(offsetsOut),
        [blendFactor](const auto& pair){
            const auto& [from, to] = pair;
            return ComposeMatrix(
                XMVectorLerp(from.scale, to.scale, blendFactor),
                XMQuaternionSlerp(from.rotation, to.rotation, blendFactor),
                XMVectorLerp(from.position, to.position, blendFactor)
            );
        }
    );
}

void AnimateBones(const gfx2::PackedRig& rig,
                  const std::vector<uint8_t>& hasValues,
                  std::vector<XMMATRIX>& offsets,
                  std::vector<BoneData>& bonesOut)
{
    NC_PROFILE_SCOPE("AnimateBones()", ProfileCategory::Animation);

    // Replace each boneToParent offset with its animation offset, if present. Else, leave as the original offset.
    // Replace unanimated offsets with the original offset
    for (auto [offset, original, animHasValue] : std::views::zip(offsets, rig.boneToParent, hasValues))
    {
        if (!animHasValue)
            offset = original;
    }

    // Multiply each child (siblings are contiguous) with its parent.
    for (auto [parent, childrenIndex] : std::views::zip(offsets, rig.offsetChildren))
    {
        for (auto& child : std::views::counted(offsets.begin() + childrenIndex.indexOfFirstChild, childrenIndex.numChildren))
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
            [&globalInverseTransform = rig.globalInverseTransform, &offsets](const auto& in)
            {
                const auto& [matrix, offset] = in;
                return BoneData{matrix * offsets.at(offset) * globalInverseTransform};
            }
    );
}
} // anonymous namespace

namespace nc::graphics::gfx3
{
auto SkeletalAnimationCalculator::Animate(const gfx2::PackedRig& rig,
                                          const asset::SkeletalAnimation& animation,
                                          float timeInTicks) -> std::span<const BoneData>
{
    Prepare(rig.boneNames.size());
    CalculateOffsets(rig.boneNames, animation, timeInTicks, m_offsets, m_hasValues);
    AnimateBones(rig, m_hasValues, m_offsets, m_boneBuffer);
    return std::span<const BoneData>{m_boneBuffer};
}

auto SkeletalAnimationCalculator::Animate(const gfx2::PackedRig& rig,
                                          const asset::SkeletalAnimation& blendFromAnimation,
                                          float blendFromTicks,
                                          const asset::SkeletalAnimation& blendToAnimation,
                                          float blendToTicks,
                                          float blendFactor) -> std::span<const BoneData>
{
    Prepare(rig.boneNames.size());
    CalculateOffsets(rig.boneNames, blendToAnimation, blendToTicks, m_toOffsetsDecomposed, &m_hasValues);
    CalculateOffsets(rig.boneNames, blendFromAnimation, blendFromTicks, m_fromOffsetsDecomposed, nullptr);
    BlendOffsets(m_fromOffsetsDecomposed, m_toOffsetsDecomposed, blendFactor, m_offsets);
    AnimateBones(rig, m_hasValues, m_offsets, m_boneBuffer);
    return std::span<const BoneData>{m_boneBuffer};
}

void SkeletalAnimationCalculator::Prepare(size_t boneCapacity)
{
    m_boneBuffer.clear();
    m_offsets.clear();
    m_fromOffsetsDecomposed.clear();
    m_toOffsetsDecomposed.clear();
    m_hasValues.clear();

    if (boneCapacity > m_boneBuffer.capacity())
    {
        m_boneBuffer.reserve(boneCapacity); // ok?
        m_offsets.reserve(boneCapacity);
        m_toOffsetsDecomposed.reserve(boneCapacity);
        m_hasValues.reserve(boneCapacity);
    }
}
} // namespace nc::graphics

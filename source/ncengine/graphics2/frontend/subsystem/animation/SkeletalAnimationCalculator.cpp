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
void CalculateOffsets(const Rig& rig,
                      const asset::SkeletalAnimation& animation,
                      float timeInTicks,
                      std::vector<XMMATRIX>& offsetsOut)
{
    for (const auto [boneName, parent] : std::views::zip(rig.boneNames, rig.boneToParent))
    {
        const auto iter = animation.framesPerBone.find(boneName);
        if (iter != animation.framesPerBone.end())
        {
            offsetsOut.push_back(ComposeMatrix(
                GetInterpolatedScale(timeInTicks, iter->second.scaleFrames),
                GetInterpolatedRotation(timeInTicks, iter->second.rotationFrames),
                GetInterpolatedPosition(timeInTicks, iter->second.positionFrames)
            ));

            continue;
        }

        offsetsOut.push_back(parent);
    }
}

// Calculate decomposed offsets to be blended with another animation
void CalculateOffsetsForBlending(const Rig& rig,
                                 const asset::SkeletalAnimation& animation,
                                 float timeInTicks,
                                 std::vector<DecomposedMatrixXM>& offsetsOut)
{
    for (const auto [boneName, parent] : std::views::zip(rig.boneNames, rig.boneToParent))
    {
        const auto iter = animation.framesPerBone.find(boneName);
        if (iter != animation.framesPerBone.end())
        {
            offsetsOut.emplace_back(
                GetInterpolatedScale(timeInTicks, iter->second.scaleFrames),
                GetInterpolatedRotation(timeInTicks, iter->second.rotationFrames),
                GetInterpolatedPosition(timeInTicks, iter->second.positionFrames)
            );

            continue;
        }

        offsetsOut.push_back(DecomposeMatrix(parent));
    }
}

// Combine decomposed offsets of two animations
void BlendOffsets(const std::vector<DecomposedMatrixXM>& fromOffsets,
                  const std::vector<DecomposedMatrixXM>& toOffsets,
                  float blendFactor,
                  std::vector<XMMATRIX>& offsetsOut)
{
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

void AnimateBones(const Rig& rig,
                  std::vector<XMMATRIX>& offsets,
                  std::vector<BoneData>& bonesOut)
{
    // Multiply each child (siblings are contiguous) with its parent.
    for (auto [parent, childrenIndex] : std::views::zip(offsets, rig.offsetChildren))
    {
        for (auto& child : std::views::counted(offsets.begin() + childrenIndex.indexOfFirstChild, childrenIndex.numChildren))
        {
            child = XMMatrixMultiply(child, parent);
        }
    }

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
auto SkeletalAnimationCalculator::Animate(const Rig& rig,
                                          const asset::SkeletalAnimation& animation,
                                          float timeInTicks) -> std::span<const BoneData>
{
    Prepare(rig, false);
    CalculateOffsets(rig, animation, timeInTicks, m_offsets);
    AnimateBones(rig, m_offsets, m_boneBuffer);
    return std::span<const BoneData>{m_boneBuffer};
}

auto SkeletalAnimationCalculator::Animate(const Rig& rig,
                                          const asset::SkeletalAnimation& blendFromAnimation,
                                          float blendFromTicks,
                                          const asset::SkeletalAnimation& blendToAnimation,
                                          float blendToTicks,
                                          float blendFactor) -> std::span<const BoneData>
{
    Prepare(rig, true);
    CalculateOffsetsForBlending(rig, blendToAnimation, blendToTicks, m_toOffsetsDecomposed);
    CalculateOffsetsForBlending(rig, blendFromAnimation, blendFromTicks, m_fromOffsetsDecomposed);
    BlendOffsets(m_fromOffsetsDecomposed, m_toOffsetsDecomposed, blendFactor, m_offsets);
    AnimateBones(rig, m_offsets, m_boneBuffer);
    return std::span<const BoneData>{m_boneBuffer};
}

void SkeletalAnimationCalculator::Prepare(const Rig& rig, bool blended)
{
    const auto boneCapacity = rig.boneToParent.size();
    const auto vertexToBoneCapacity = rig.vertexToBone.size();
    m_boneBuffer.clear();
    if (vertexToBoneCapacity > m_boneBuffer.capacity())
    {
        m_boneBuffer.reserve(vertexToBoneCapacity);
    }

    m_offsets.clear();
    m_fromOffsetsDecomposed.clear();
    m_toOffsetsDecomposed.clear();

    if (boneCapacity > m_offsets.capacity())
    {
        m_offsets.reserve(boneCapacity);
        if (blended)
        {
            m_fromOffsetsDecomposed.reserve(boneCapacity);
            m_toOffsetsDecomposed.reserve(boneCapacity);
        }
    }
}
} // namespace nc::graphics

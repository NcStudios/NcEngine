#include "SkeletalAnimationCalculator.h"

#include "ncengine/debug/Profile.h"
#include "ncutility/Hash.h"
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

// Blended animations use DecomposedMatrixXM for OutType, otherwise offsets are XMMATRIX
template<class OutType>
void CalculateOffsets(const Rig& rig,
                      const asset::SkeletalAnimation& animation,
                      float timeInTicks,
                      std::vector<OutType>& offsetsOut)
{
    for (const auto [boneName, parent] : std::views::zip(rig.boneNames, rig.boneToParent))
    {
        const auto iter = animation.framesPerBone.find(boneName);
        if (iter != animation.framesPerBone.end())
        {
            if constexpr (std::same_as<OutType, DecomposedMatrixXM>)
            {
                offsetsOut.emplace_back(
                    GetInterpolatedScale(timeInTicks, iter->second.scaleFrames),
                    GetInterpolatedRotation(timeInTicks, iter->second.rotationFrames),
                    GetInterpolatedPosition(timeInTicks, iter->second.positionFrames)
                );
            }
            else
            {
                offsetsOut.push_back(ComposeMatrix(
                    GetInterpolatedScale(timeInTicks, iter->second.scaleFrames),
                    GetInterpolatedRotation(timeInTicks, iter->second.rotationFrames),
                    GetInterpolatedPosition(timeInTicks, iter->second.positionFrames)
                ));
            }

            continue;
        }

        if constexpr (std::same_as<OutType, DecomposedMatrixXM>)
        {
            offsetsOut.push_back(DecomposeMatrix(parent));
        }
        else
        {
            offsetsOut.push_back(parent);
        }
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

void AnimateBones(uint64_t meshId,
                  const Rig& rig,
                  std::vector<XMMATRIX>& offsets,
                  std::vector<BoneData>& bonesOut,
                  std::vector<std::string>& boneNamesOut)
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
    // This outputs a matrix that can be used to transform a vertex into its final animated position, and the name of the bone
    for (const auto& [matrix, offset, name] : std::views::zip(rig.vertexToBone, rig.offsetsMap, rig.boneNames))
    {
        bonesOut.push_back(BoneData{matrix * offsets.at(offset) * rig.globalInverseTransform});
        boneNamesOut.push_back(std::to_string(meshId) + name);
    }
    NC_ASSERT(bonesOut.size() == boneNamesOut.size(), "Bone names must be in lock step with the bone matrices and they are not!");
}
} // anonymous namespace

namespace nc::graphics
{
auto SkeletalAnimationCalculator::Animate(uint64_t meshId,
                                          const Rig& rig,
                                          const asset::SkeletalAnimation& animation,
                                          float timeInTicks,
                                          std::vector<std::string>& boneNamesOut) -> std::span<const BoneData>
{
    Prepare(rig, false);
    CalculateOffsets(rig, animation, timeInTicks, m_offsets);
    AnimateBones(meshId, rig, m_offsets, m_boneBuffer, boneNamesOut);
    return std::span<const BoneData>{m_boneBuffer};
}

auto SkeletalAnimationCalculator::Animate(uint64_t meshId,
                                          const Rig& rig,
                                          const asset::SkeletalAnimation& blendFromAnimation,
                                          float blendFromTicks,
                                          const asset::SkeletalAnimation& blendToAnimation,
                                          float blendToTicks,
                                          float blendFactor,
                                          std::vector<std::string>& boneNamesOut) -> std::span<const BoneData>
{
    Prepare(rig, true);
    CalculateOffsets(rig, blendToAnimation, blendToTicks, m_toOffsetsDecomposed);
    CalculateOffsets(rig, blendFromAnimation, blendFromTicks, m_fromOffsetsDecomposed);
    BlendOffsets(m_fromOffsetsDecomposed, m_toOffsetsDecomposed, blendFactor, m_offsets);
    AnimateBones(meshId, rig, m_offsets, m_boneBuffer, boneNamesOut);
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

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
        
        auto animatedBoneOffset = anim::DecomposedMatrix
        {
            .pos =   GetInterpolatedPosition(timeInTicks, iter->second.positionFrames),
            .rot =   GetInterpolatedRotation(timeInTicks, iter->second.rotationFrames),
            .scale = GetInterpolatedScale(timeInTicks, iter->second.scaleFrames)
        };

        animationMatrices.offsets.push_back(std::move(animatedBoneOffset));
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

    for (auto i = 0u; i < elementsCount; i++)
    {
        auto positionMatrix = ToTransMatrix(decomposedAnimation.offsets[i].pos);
        auto rotationMatrix = ToRotMatrix(decomposedAnimation.offsets[i].rot);
        auto scaleMatrix = ToScaleMatrix(decomposedAnimation.offsets[i].scale);

        packedAnimation.offsets.push_back(scaleMatrix * rotationMatrix * positionMatrix);
    }
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
    auto elementsCount = blendFromDecomposed.offsets.size(); // Both vectors (blendFromDecomposed and blendToDecomposed) guaranteed to have same size.
    packedAnimation.offsets.reserve(elementsCount);
    packedAnimation.hasValues = std::vector<anim::HasValue>{blendFromDecomposed.hasValues.begin(), blendFromDecomposed.hasValues.end()};

    for (auto i = 0u; i < elementsCount; i++) /* @todo: replace with std::ranges::zip_view once we have cpp 23*/
    {
        auto blendedPos = Lerp(blendFromDecomposed.offsets[i].pos, blendToDecomposed.offsets[i].pos, blendFactor);
        auto blendedRot = Normalize(Slerp(blendFromDecomposed.offsets[i].rot, blendToDecomposed.offsets[i].rot, blendFactor));
        auto blendedScale = Lerp(blendFromDecomposed.offsets[i].scale, blendToDecomposed.offsets[i].scale, blendFactor);

        auto positionMatrix = ToTransMatrix(blendedPos);
        auto rotationMatrix = ToRotMatrix(blendedRot);
        auto scaleMatrix = ToScaleMatrix(blendedScale);

        packedAnimation.offsets.push_back(scaleMatrix * rotationMatrix * positionMatrix);
    }
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

auto GetInterpolatedPosition(float timeInTicks, const std::vector<nc::asset::PositionFrame> positionFrames) -> nc::Vector3
{
    auto interpolatedPosition = nc::Vector3{};

    if (positionFrames.size() == 0)
    {
        throw NcError("Animation has no position data for the node.");
    }

    if (positionFrames.size() == 1)
    {
        return positionFrames[0].position;
    }

    auto positionIndex = UINT_LEAST32_MAX;
    for (auto i = 0u; i < positionFrames.size(); i++)
    {
        if (timeInTicks < positionFrames[i+1].timeInTicks)
        {
            positionIndex = i;
            break;
        }
    }
    if (positionIndex == UINT_LEAST32_MAX)
    {
        throw NcError("Animation has no position data at time: ", std::to_string(timeInTicks));
    }

    auto nextPositionIndex = positionIndex + 1;
    if ((positionIndex + 1) > positionFrames.size())
    {
        throw NcError("Animation interpolation index has exceeded the bounds of the frame data.");
    }

    auto deltaTimeInTicks = positionFrames[nextPositionIndex].timeInTicks - positionFrames[positionIndex].timeInTicks;
    auto interpolationFactor = (timeInTicks - positionFrames[positionIndex].timeInTicks) / deltaTimeInTicks;

    if (interpolationFactor < 0.0f || interpolationFactor > 1.0f)
    {
        throw NcError("Error calculating the interpolation factor: ", std::to_string(interpolationFactor));
    }

    interpolatedPosition = Lerp(positionFrames[positionIndex].position, positionFrames[nextPositionIndex].position, static_cast<float>(interpolationFactor));
    return interpolatedPosition;
}

auto GetInterpolatedRotation(float timeInTicks, const std::vector<nc::asset::RotationFrame> rotationFrames) -> nc::Quaternion
{
    auto interpolatedRotation = nc::Quaternion{};

    if (rotationFrames.size() == 0)
    {
        throw NcError("Animation has no rotation data.");
    }

    if (rotationFrames.size() == 1)
    {
        return Normalize(rotationFrames[0].rotation);
    }

    auto rotationIndex = UINT_LEAST32_MAX;
    for (auto i = 0u; i < rotationFrames.size(); i++)
    {
        if (timeInTicks < rotationFrames[i+1].timeInTicks)
        {
            rotationIndex = i;
            break;
        }
    }
    if (rotationIndex == UINT_LEAST32_MAX)
    {
        throw NcError("Animation has no rotation data at time: ", std::to_string(timeInTicks));
    }

    auto nextRotationIndex = rotationIndex + 1;
    if ((rotationIndex + 1) > rotationFrames.size())
    {
        throw NcError("Animation interpolation index has exceeded the bounds of the frame data.");
    }

    auto deltaTimeInTicks = rotationFrames[nextRotationIndex].timeInTicks - rotationFrames[rotationIndex].timeInTicks;
    auto interpolationFactor = (timeInTicks - rotationFrames[rotationIndex].timeInTicks) / deltaTimeInTicks;

    if (interpolationFactor < 0.0f || interpolationFactor > 1.0f)
    {
        throw NcError("Error calculating the interpolation factor: ", std::to_string(interpolationFactor));
    }

    interpolatedRotation = Slerp(rotationFrames[rotationIndex].rotation, rotationFrames[nextRotationIndex].rotation, static_cast<float>(interpolationFactor));
    return Normalize(interpolatedRotation);
}

auto GetInterpolatedScale(float timeInTicks, const std::vector<nc::asset::ScaleFrame> scaleFrames) -> nc::Vector3
{
    auto interpolatedScale = nc::Vector3{};
    if (scaleFrames.size() == 0)
    {
        throw NcError("Animation has no scale data.");
    }

    if (scaleFrames.size() == 1)
    {
        return scaleFrames[0].scale;
    }

    auto scaleIndex = UINT_LEAST32_MAX;
    for (auto i = 0u; i < scaleFrames.size(); i++)
    {
        if (timeInTicks < scaleFrames[i+1].timeInTicks)
        {
            scaleIndex = i;
            break;
        }
    }
    if (scaleIndex == UINT_LEAST32_MAX)
    {
        throw NcError("Animation has no scale data at time: ", std::to_string(timeInTicks));
    }

    auto nextScaleIndex = scaleIndex + 1;
    if ((scaleIndex + 1) > scaleFrames.size())
    {
        throw NcError("Animation interpolation index has exceeded the bounds of the frame data.");
    }

    auto deltaTimeInTicks = scaleFrames[nextScaleIndex].timeInTicks - scaleFrames[scaleIndex].timeInTicks;
    auto interpolationFactor = (timeInTicks - scaleFrames[scaleIndex].timeInTicks) / deltaTimeInTicks;

    if (interpolationFactor < 0.0f || interpolationFactor > 1.0f)
    {
        throw NcError("Error calculating the interpolation factor: ", std::to_string(interpolationFactor));
    }

    interpolatedScale = Lerp(scaleFrames[scaleIndex].scale, scaleFrames[nextScaleIndex].scale, static_cast<float>(interpolationFactor));
    return interpolatedScale;
}
} // namespace nc::graphics

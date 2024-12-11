#pragma once

#include "ncengine/ecs/Component.h"
#include "ncasset/Assets.h"

#include <limits>

namespace nc::graphics::gfx2
{
/**
 * @brief Used to traverse the flattened offsets tree.
 */
struct OffsetChildren
{
    uint32_t indexOfFirstChild;
    uint32_t numChildren;
};

/**
 * @brief An SoA representation of nc::asset::BonesData.
 * 
 * SkeletalAnimationSystem uses this type to efficiently perform animation transformation calculations.
 */
struct PackedRig
{
    PackedRig(const nc::asset::BonesData& bonesData);

    std::vector<DirectX::XMMATRIX> vertexToBone;
    std::vector<DirectX::XMMATRIX> boneToParent;
    DirectX::XMMATRIX globalInverseTransform;
    std::vector<std::string> boneNames;
    std::vector<OffsetChildren> offsetChildren;
    std::vector<uint32_t> offsetsMap;
};

/**
 * @brief The raw data members that get composed into a DirectX::XMMATRIX.
 * 
 * We need this raw data as we need to interpolate between two DecomposedMatrix types in the case of blending between states 
 * rather than attempting to interpolate between two DirectX::XMMATRIX types.
 */
struct DecomposedMatrix
{
    Vector3 pos;
    Quaternion rot;
    Vector3 scale; // todo: we construct a default one of these somewhere; shouldn't scale be One()?
};

/**
 * @brief A quick wrapper representing a bool, avoiding a std::vector<bool>.
 */
using HasValue = uint8_t;

/**
 * @brief The interpolated DirectX::XMMATRIX objects representing the animated transformation matrix per PackedRig::boneToParent node.
 * 
 * The items in both vectors below sync up exactly to the PackedRig::boneToParent vector. Because some PackedRig::boneToParent items will not 
 * have animation data, we need the hasValues vector to act as a 'sparse set'.
 */
struct PackedAnimation
{
    std::vector<DirectX::XMMATRIX> offsets;
    std::vector<HasValue> hasValues;
};

/**
 * @brief The pre-interpolated DecomposedMatrix objects representing the animated transformation matrix per PackedRig::boneToParent node.
 * 
 * The items in both vectors below sync up exactly to the PackedRig::boneToParent vector. Because some PackedRig::boneToParent items will not 
 * have animation data, we need the hasValues vector to act as a 'sparse set'.
 * 
 * We need this raw data as we need to interpolate between two DecomposedMatrix types in the case of blending between states 
 * rather than attempting to interpolate between two DirectX::XMMATRIX types.
 */
struct PackedAnimationDecomposed
{
    std::vector<DecomposedMatrix> offsets;
    std::vector<HasValue> hasValues;
};

struct InFlightAnimation
{
    uint64_t meshId;
    uint64_t animId;
    uint64_t blendFromAnimId;
    uint32_t boneIndex;
    float time;
    float blendFromTime;
    float currentTransitionTime;
    float transitionDuration;
    float blendFactor;
};
} // namespace nc::anim

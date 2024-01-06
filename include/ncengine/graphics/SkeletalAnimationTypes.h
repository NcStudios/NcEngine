/**
 * @file SkeletalAnimationTypes.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"
#include "ncasset/Assets.h"

namespace nc::graphics::anim
{
/**
 * @brief Options for controlling the behavior of the animation.
 */
enum class Action : uint8_t
{
    Loop
};

/**
 * @brief A simplified representation of a SkeletalAnimator state machine state sent to SkeletalAnimationSystem on state transition.
 */
struct PlayState
{
    std::string meshUid;
    std::string prevAnimUid;
    std::string curAnimUid;
    Action action;
    Entity entity;
};

/**
 * @brief A type used to simplify creating SkeletalAnimator state machine states.
 * 
 * The Initial state is implicitly created from the constructor of the SkeletalAnimator and nowhere else.
 */
struct Initial
{
    Initial(std::string animUid_)
        : animUid{std::move(animUid_)}{} 
    std::string animUid;
};

/**
 * @brief An object representing a node in the SkeletalAnimator state machine.
 */
struct State
{
    static constexpr uint32_t NullId = UINT32_MAX;
    State(const Initial& initialProperties)
        : id{},
          action{Action::Loop},
          animUid{std::move(initialProperties.animUid)}{}

    uint32_t id;
    Action action;
    std::string animUid;
};

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
    Vector3 scale;
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

/**
 * @brief The unit of work representing all the data SkeletalAnimationSystem needs to process each frame per animation.
 */
struct UnitOfWork
{
    Entity::index_type index;
    PackedRig* rig;
    asset::SkeletalAnimation* blendFromAnim;
    asset::SkeletalAnimation* blendToAnim;
    float blendFromTime;
    float blendToTime;
    float blendFactor;
};
} // namespace nc::graphics::anim

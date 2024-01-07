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
    Loop,
    PlayOnce,
    Stop
};

/**
 * @brief A simplified representation of a SkeletalAnimator state machine state sent to SkeletalAnimationSystem on state transition.
 */
struct StateChange
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
 * The Stop state is used to stop an animaton and remove it from SkeletalAnimationSystem's active animations container.
 */
struct Stop
{
    uint32_t enterFrom;
    std::function<bool ()> enterWhen;
};

/**
 * @brief A type used to simplify creating SkeletalAnimator state machine states.
 * 
 * The Loop state is used to loop an animaton until the exitWhen condition is met.
 */
struct Loop
{
    uint32_t enterFrom;
    std::function<bool ()> enterWhen;
    std::string animUid;
    std::function<bool ()> exitWhen;
    uint32_t exitTo;
};

/**
 * @brief A type used to simplify creating SkeletalAnimator state machine states.
 * 
 * The PlayOnce state is used to play an animaton through a single cycle. Upon conclusion of the cycle, the animation will transition to the exitTo state.
 */
struct PlayOnce
{
    uint32_t enterFrom;
    std::function<bool ()> enterWhen;
    std::string animUid;
    uint32_t exitTo;
};

/**
 * @brief A type used to simplify creating SkeletalAnimator state machine states.
 * 
 * The Root state is implicitly created from the constructor of the SkeletalAnimator and nowhere else.
 */
struct Root
{
    Root(std::string animUid_)
        : animUid{std::move(animUid_)}{} 
    std::string animUid;
};

/**
 * @brief A type used to simplify creating SkeletalAnimator state machine states.
 * 
 * The StopImmediate state is used as an ad-hoc state meant to be invoked from outside of the state machine (example, OnCollisionEnter).
 * The StopImmediate state then transitions back into the state machine into the active animation.
 */
struct StopImmediate
{
    std::function<bool ()> exitWhen;
    uint32_t exitTo;
};

/**
 * @brief An object representing a node in the SkeletalAnimator state machine.
 */
struct State
{
    static constexpr uint32_t NullId = UINT32_MAX;

    /**
     * @brief Construct a new State in the state machine representing the state the SkeletalAnimator is initialized playing.
     * @param initialProperties The parent entity.
     */
    State(const Root& initialProperties)
        : id{},
          action{Action::Loop},
          animUid{std::move(initialProperties.animUid)},
          firstRunComplete{nullptr},
          successors{},
          enterFrom{State::NullId},
          enterWhen{[](){return false;}},
          exitWhen{[](){return false;}},
          exitTo{State::NullId}
    {}

    /**
     * @brief Construct a new State in the state machine representing stopping the currently playing animation.
     * @param stopProperties Specifies from which state and from which condition you enter the Stop state from.
     */
    State(const Stop& stopProperties)
        : id{},
          action{Action::Stop},
          animUid{},
          firstRunComplete{nullptr},
          successors{},
          enterFrom{stopProperties.enterFrom},
          enterWhen{std::move(stopProperties.enterWhen)},
          exitWhen{[](){return false;}},
          exitTo{State::NullId}
    {}

    /**
     * @brief Construct a new State in the state machine representing adding an animation which loops until the exit condition is met.
     * @param loopProperties Specifies from which state and from which condition you enter the looping state from, 
     * which state and from which condition you enter after the exit condition is met, and which animation you loop.
     */
    State(const Loop& loopProperties)
        : id{},
          action{Action::Loop},
          animUid{std::move(loopProperties.animUid)},
          firstRunComplete{nullptr},
          successors{},
          enterFrom{loopProperties.enterFrom},
          enterWhen{std::move(loopProperties.enterWhen)},
          exitWhen{std::move(loopProperties.exitWhen)},
          exitTo{loopProperties.exitTo}
    {}

    /**
     * @brief Construct a new State in the state machine representing adding an animation which plays once and then exits to the exit state.
     * @param playOnceProperties Specifies from which state and from which condition you enter the play once state from, 
     * which state you enter after the animation has completed one cycle, and which animation you play once.
     */
    State(const PlayOnce& playOnceProperties)
        : id{},
          action{Action::PlayOnce},
          animUid{std::move(playOnceProperties.animUid)},
          firstRunComplete{std::make_unique<bool>()},
          successors{},
          enterFrom{playOnceProperties.enterFrom},
          enterWhen{std::move(playOnceProperties.enterWhen)},
          exitWhen{[firstRunComplete=firstRunComplete.get()](){ if (*firstRunComplete == true) { *firstRunComplete = false; return true; } return false; }},
          exitTo{playOnceProperties.exitTo}
    {}

    /**
     * @brief Construct a new State in the state machine representing adding an animation which stops the current animation and then exits to the exit state.
     * @param stopImmediateProperties Specifies which state you enter after the exitCondition is met, and the exitCondition.
     */
    State(const StopImmediate& stopImmediateProperties)
        : id{},
          action{Action::Stop},
          animUid{},
          firstRunComplete{nullptr},
          successors{},
          enterFrom{State::NullId},
          enterWhen{[](){return false;}},
          exitWhen{std::move(stopImmediateProperties.exitWhen)},
          exitTo{stopImmediateProperties.exitTo}
    {}

    /**
     * @brief Add a state as a successor to the current state.
     */
    void AddSuccessor(uint32_t successor);

    uint32_t id;
    Action action;
    std::string animUid;
    std::unique_ptr<bool> firstRunComplete;
    std::vector<uint32_t> successors;
    uint32_t enterFrom;
    std::function<bool ()> enterWhen;
    std::function<bool ()> exitWhen;
    uint32_t exitTo;
};

class SparseSet
{
    public:
        SparseSet(uint32_t maxStates);
        auto Insert(State toInsert) -> uint32_t;
        auto Remove(uint32_t toRemove) -> bool;
        auto Contains(uint32_t id) -> bool;
        auto Get(uint32_t id) -> State*;
        auto GetLast() -> State*;

    private:
        auto AssignId() -> uint32_t { return static_cast<uint32_t>(m_dense.size()); }

        std::vector<anim::State> m_dense;
        std::vector<uint32_t> m_sparse;
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

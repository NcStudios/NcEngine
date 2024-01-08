/**
 * @file SkeletalAnimationTypes.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"
#include "ncasset/Assets.h"

namespace nc::graphics::anim
{

static constexpr uint32_t NullState = UINT32_MAX;
static constexpr uint32_t RootState = 0u;
static constexpr bool Never(){return false;};

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
 * The Stop state is used to stop an animation and remove it from SkeletalAnimationSystem's active
 * animations container.
 */
struct Stop
{
        uint32_t enterFrom;
        std::function<bool()> enterWhen;
};

/**
 * @brief A type used to simplify creating SkeletalAnimator state machine states.
 *
 * The Loop state is used to loop an animation until the exitWhen condition is met.
 */
struct Loop
{
        uint32_t enterFrom;
        std::function<bool()> enterWhen;
        std::string animUid;
        std::function<bool()> exitWhen;
        uint32_t exitTo = RootState;
};

/**
 * @brief A type used to simplify creating SkeletalAnimator state machine states.
 *
 * The PlayOnce state is used to play an animation through a single cycle. Upon conclusion of the
 * cycle, the animation will transition to the exitTo state.
 */
struct PlayOnce
{
        uint32_t enterFrom;
        std::function<bool()> enterWhen;
        std::string animUid;
        uint32_t exitTo = RootState;
};

/**
 * @brief A type used to simplify creating SkeletalAnimator state machine states.
 *
 * The Root state is implicitly created from the constructor of the SkeletalAnimator and nowhere
 * else.
 */
struct Root
{
        Root(std::string animUid_) : animUid{std::move(animUid_)} {}
        std::string animUid;
};

/**
 * @brief A type used to simplify creating SkeletalAnimator state machine states.
 *
 * The StopImmediate state is used as an ad-hoc state meant to be invoked from outside of the state
 * machine (example, OnCollisionEnter). The StopImmediate state then transitions back into the state
 * machine into the active animation. Note: All three Action types are available to be called in
 * Immediate mode, but only Stop needed this helper.
 */
struct StopImmediate
{
        std::function<bool()> exitWhen;
        uint32_t exitTo = RootState;
};

/**
 * @brief An object representing a node in the SkeletalAnimator state machine.
 */
struct State
{
    /**
     * @brief Construct a new State in the state machine representing the state the SkeletalAnimator is initialized playing.
     * @param initialProperties The properties of the root state.
     */
    State(Root initialProperties)
        : animUid{std::move(initialProperties.animUid)},
          firstRunComplete{nullptr},
          enterWhen{anim::Never},
          exitWhen{anim::Never},
          id{},
          enterFrom{NullState},
          exitTo{NullState},
          action{Action::Loop},
          successors{}
    {}

    /**
     * @brief Construct a new State in the state machine representing stopping the currently playing animation.
     * @param stopProperties Specifies from which state and from which condition you enter the Stop state from.
     */
    State(Stop stopProperties)
        : animUid{},
          firstRunComplete{nullptr},
          enterWhen{std::move(stopProperties.enterWhen)},
          exitWhen{Never},
          id{},
          enterFrom{stopProperties.enterFrom},
          exitTo{NullState},
          action{Action::Stop},
          successors{}
    {}

    /**
     * @brief Construct a new State in the state machine representing adding an animation which loops until the exit condition is met.
     * @param loopProperties Specifies from which state and from which condition you enter the looping state from, 
     * which state and from which condition you enter after the exit condition is met, and which animation you loop.
     */
    State(Loop loopProperties)
        : animUid{std::move(loopProperties.animUid)},
          firstRunComplete{nullptr},
          enterWhen{std::move(loopProperties.enterWhen)},
          exitWhen{std::move(loopProperties.exitWhen)},
          id{},
          enterFrom{loopProperties.enterFrom},
          exitTo{loopProperties.exitTo},
          action{Action::Loop},
          successors{}
    {}

    /**
     * @brief Construct a new State in the state machine representing adding an animation which plays once and then exits to the exit state.
     * @param playOnceProperties Specifies from which state and from which condition you enter the play once state from, 
     * which state you enter after the animation has completed one cycle, and which animation you play once.
     */
    State(PlayOnce playOnceProperties)
        : animUid{std::move(playOnceProperties.animUid)},
          firstRunComplete{std::make_unique<bool>(false)},
          enterWhen{std::move(playOnceProperties.enterWhen)},
          exitWhen{[firstRunComplete=firstRunComplete.get()](){ if (*firstRunComplete == true) { *firstRunComplete = false; return true; } return false; }},
          id{},
          enterFrom{playOnceProperties.enterFrom},
          exitTo{playOnceProperties.exitTo},
          action{Action::PlayOnce},
          successors{}
    {}

    /**
     * @brief Construct a new State in the state machine representing adding an animation which stops the current animation and then exits to the exit state.
     * @param stopImmediateProperties Specifies which state you enter after the exitCondition is met, and the exitCondition.
     */
    State(const StopImmediate stopImmediateProperties)
        : animUid{},
          firstRunComplete{nullptr},
          enterWhen{Never},
          exitWhen{std::move(stopImmediateProperties.exitWhen)},
          id{},
          enterFrom{NullState},
          exitTo{stopImmediateProperties.exitTo},
          action{Action::Stop},
          successors{}
    {}

    /**
     * @brief Add a state as a successor to the current state.
     */
    void AddSuccessor(uint32_t successor);

    std::string animUid;
    std::unique_ptr<bool> firstRunComplete;
    std::function<bool ()> enterWhen;
    std::function<bool ()> exitWhen;
    uint32_t id;
    uint32_t enterFrom;
    uint32_t exitTo;
    Action action;
    std::vector<uint32_t> successors;
};

/**
 * @brief A container needed for tracking State objects with stable handles.
 */
class StableSet
{
    public:
        /**
         * @brief Construct a new Stable Set object
         * 
         * @param maxStates The container capacity.
         */
        StableSet(uint32_t maxStates);

        /**
         * @brief Add a State.
         * 
         * @param toInsert The State to add.
         * @return The handle to the State.
         */
        auto Insert(State toInsert) -> uint32_t;

        /**
         * @brief Remove a State.
         * 
         * @param toRemove the handle of the State to remove.
         * @return true If the State was removed.
         * @return false if the State was not present.
         */
        auto Remove(uint32_t toRemove) noexcept -> bool;

        /**
         * @brief Does the container hold the State with the given handle?
         * 
         * @param id The State handle.
         * @return true If the container holds the State.
         * @return false If the container does not hold the State.
         */
        auto Contains(uint32_t id) const noexcept -> bool;

        /**
         * @brief Get the State with the given handle.
         * 
         * @param id The handle of the State to get.
         * @return State* The State requested or nullptr if not present.
         */
        auto Get(uint32_t id) noexcept -> State*;

        /**
         * @brief Get the last State added.
         * 
         * @return State* The handle of the most recently added State or nullptr if no State exists.
         */
        auto GetLast() noexcept -> State*;

    private:
        auto AssignId() const noexcept -> uint32_t { return static_cast<uint32_t>(m_states.size()); }

        std::vector<anim::State> m_states;
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

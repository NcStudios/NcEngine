#include "gtest/gtest.h"
#include "ncengine/graphics/SkeletalAnimationController.h"

TEST(SkeletalAnimationControllerTest, GetCurrentAnimationId_returnsUntransitionedId)
{
    auto uut = nc::SkeletalAnimationController{1};

    // null state until first transition
    EXPECT_EQ(nc::asset::NullAssetId, uut.GetCurrentAnimationId());
    uut.CheckForTransition();
    EXPECT_EQ(1, uut.GetCurrentAnimationId());

    // immediate transition preserves current id until completed
    uut.PlayOnceImmediate(2);
    EXPECT_EQ(1, uut.GetCurrentAnimationId());
    uut.CheckForTransition();
    EXPECT_EQ(2, uut.GetCurrentAnimationId());

    // immediate -> immediate transition preserves first id until completed
    uut.PlayOnceImmediate(3);
    EXPECT_EQ(2, uut.GetCurrentAnimationId());
    uut.CheckForTransition();
    EXPECT_EQ(3, uut.GetCurrentAnimationId());

    // stop sets to null
    auto exitStop = false;
    uut.StopImmediate([&exitStop](){ return exitStop; });
    EXPECT_EQ(3, uut.GetCurrentAnimationId());
    uut.CheckForTransition();
    EXPECT_EQ(nc::asset::NullAssetId, uut.GetCurrentAnimationId());
    exitStop = true;
    uut.CheckForTransition(); // back to root
    EXPECT_EQ(1, uut.GetCurrentAnimationId());

    // update root id preserves original root id until completed
    uut.SetAnimation(nc::RootAnimationState, 4);
    EXPECT_EQ(1, uut.GetCurrentAnimationId());
    uut.CheckForTransition();
    EXPECT_EQ(4, uut.GetCurrentAnimationId());
}

TEST(SkeletalAnimationControllerTest, SetRootAnimation_modifyActiveState_retransitions)
{
    auto uut = nc::SkeletalAnimationController{1};
    uut.CheckForTransition();

    const auto state = uut.AddState(nc::PlayOnceAnimation{
        .animId = 2,
        .enterWhen = []() { return true; }}
    );

    uut.CheckForTransition();
    uut.SetAnimation(state, 3);
    const auto actual = uut.CheckForTransition();
    EXPECT_EQ(2, actual.fromAnimId);
    EXPECT_EQ(3, actual.toAnimId);
    EXPECT_EQ(nc::AnimationTransitionType::PlayOnce, actual.type);
    EXPECT_EQ(state, uut.GetActiveState());
}

TEST(SkeletalAnimationControllerTest, SetDefaultTransitionDuration_reportsNewTimeInTransition)
{
    auto uut = nc::SkeletalAnimationController{1, 0.1f};
    uut.CheckForTransition();
    const auto state = uut.AddState(nc::LoopAnimation{
        .animId = 2,
        .enterWhen = [](){ return true; },
        .transitionDuration = nc::UseDefaultTransitionDuration
    });

    constexpr auto expectedDuration = 0.5f;
    uut.SetDefaultTransitionDuration(expectedDuration);
    const auto actual = uut.CheckForTransition();
    EXPECT_EQ(1, actual.fromAnimId);
    EXPECT_EQ(2, actual.toAnimId);
    EXPECT_FLOAT_EQ(expectedDuration, actual.transitionDuration);
    EXPECT_EQ(state, uut.GetActiveState());
}

TEST(SkeletalAnimationControllerTest, CheckForTransition_afterConstruction_entersRootState)
{
    auto uut = nc::SkeletalAnimationController{1, 0.1f};
    const auto actual = uut.CheckForTransition();
    EXPECT_EQ(nc::asset::NullAssetId, actual.fromAnimId);
    EXPECT_EQ(1, actual.toAnimId);
    EXPECT_FLOAT_EQ(0.1f, actual.transitionDuration);
    EXPECT_EQ(nc::RootAnimationState, uut.GetActiveState());
}

TEST(SkeletalAnimationControllerTest, CheckForTransition_immediateStateQueued_entersImmediateState)
{
    auto uut = nc::SkeletalAnimationController{1, 0.1f};

    uut.PlayOnceImmediate(2);
    const auto actual = uut.CheckForTransition();
    EXPECT_EQ(nc::asset::NullAssetId, actual.fromAnimId);
    EXPECT_EQ(2, actual.toAnimId);
    EXPECT_FLOAT_EQ(0.1f, actual.transitionDuration);
    EXPECT_EQ(nc::ImmediateAnimationState, uut.GetActiveState());
}

TEST(SkeletalAnimationControllerTest, CheckForTransition_inImmediateState_transitionsOnCondition)
{
    auto uut = nc::SkeletalAnimationController{1, 0.1f};
    uut.CheckForTransition();

    uut.LoopImmediate(2, [](){ return true; });
    uut.CheckForTransition(); // enter immediate state
    const auto actual = uut.CheckForTransition(); // exit immediate state
    EXPECT_EQ(2, actual.fromAnimId);
    EXPECT_EQ(1, actual.toAnimId);
    EXPECT_FLOAT_EQ(0.1f, actual.transitionDuration);
    EXPECT_EQ(nc::RootAnimationState, uut.GetActiveState());
}

TEST(SkeletalAnimationControllerTest, CheckForTransition_inImmediatePlayOnceState_transitionsOnComplete)
{
    auto uut = nc::SkeletalAnimationController{1, 0.1f};
    uut.CheckForTransition();

    // enter immediate state
    uut.PlayOnceImmediate(2);
    uut.CheckForTransition();
    EXPECT_EQ(nc::ImmediateAnimationState, uut.GetActiveState());

    // stays in state
    EXPECT_EQ(nc::AnimationTransitionType::Continue, uut.CheckForTransition().type);
    EXPECT_EQ(nc::ImmediateAnimationState, uut.GetActiveState());

    // exits on notify complete
    uut.NotifyCompleteState();
    const auto actual = uut.CheckForTransition();
    EXPECT_EQ(2, actual.fromAnimId);
    EXPECT_EQ(1, actual.toAnimId);
    EXPECT_EQ(nc::RootAnimationState, uut.GetActiveState());
}

TEST(SkeletalAnimationControllerTest, CheckForTransition_inImmediateStopState_transitionsOnExitCondition)
{
    auto uut = nc::SkeletalAnimationController{1, 0.1f};
    uut.CheckForTransition();

    // transition to stop state
    auto exit = false;
    uut.StopImmediate([&exit](){ return exit; });
    const auto stopTransition = uut.CheckForTransition();
    EXPECT_EQ(1, stopTransition.fromAnimId);
    EXPECT_EQ(nc::asset::NullAssetId, stopTransition.toAnimId);
    EXPECT_EQ(nc::AnimationTransitionType::Stop, stopTransition.type);
    EXPECT_EQ(nc::ImmediateAnimationState, uut.GetActiveState());

    // stays in state
    EXPECT_EQ(nc::AnimationTransitionType::Continue, uut.CheckForTransition().type);
    EXPECT_EQ(nc::ImmediateAnimationState, uut.GetActiveState());

    // exits back to root on condition
    exit = true;
    const auto exitTransition = uut.CheckForTransition();
    EXPECT_EQ(nc::asset::NullAssetId, exitTransition.fromAnimId);
    EXPECT_EQ(1, exitTransition.toAnimId);
    EXPECT_EQ(nc::AnimationTransitionType::Loop, exitTransition.type);
    EXPECT_EQ(nc::RootAnimationState, uut.GetActiveState());
}

TEST(SkeletalAnimationControllerTest, CheckForTransition_inLoopState_transitionsOnExitCondition)
{
    auto uut = nc::SkeletalAnimationController{1, 0.1f};
    uut.CheckForTransition();

    // transition to loop state
    auto exit = false;
    const auto state = uut.AddState(nc::LoopAnimation{
        .animId = 2,
        .enterWhen = []() { return true; },
        .exitWhen = [&exit](){ return exit; }
    });

    const auto loopTransition = uut.CheckForTransition();
    EXPECT_EQ(1, loopTransition.fromAnimId);
    EXPECT_EQ(2, loopTransition.toAnimId);
    EXPECT_EQ(nc::AnimationTransitionType::Loop, loopTransition.type);
    EXPECT_EQ(state, uut.GetActiveState());

    // stays in state
    EXPECT_EQ(nc::AnimationTransitionType::Continue, uut.CheckForTransition().type);
    EXPECT_EQ(state, uut.GetActiveState());

    // exits back to root on condition
    exit = true;
    const auto exitTransition = uut.CheckForTransition();
    EXPECT_EQ(2, exitTransition.fromAnimId);
    EXPECT_EQ(1, exitTransition.toAnimId);
    EXPECT_EQ(nc::AnimationTransitionType::Loop, exitTransition.type);
    EXPECT_EQ(nc::RootAnimationState, uut.GetActiveState());
}

TEST(SkeletalAnimationControllerTest, CheckForTransition_inPlayOnceState_transitionsOnComplete)
{
    auto uut = nc::SkeletalAnimationController{1, 0.1f};
    uut.CheckForTransition();

    // transition to play once state
    const auto state = uut.AddState(nc::PlayOnceAnimation{
        .animId = 2,
        .enterWhen = []() { return true; }
    });

    const auto playOnceTransition = uut.CheckForTransition();
    EXPECT_EQ(1, playOnceTransition.fromAnimId);
    EXPECT_EQ(2, playOnceTransition.toAnimId);
    EXPECT_EQ(nc::AnimationTransitionType::PlayOnce, playOnceTransition.type);
    EXPECT_EQ(state, uut.GetActiveState());

    // stays in state
    EXPECT_EQ(nc::AnimationTransitionType::Continue, uut.CheckForTransition().type);
    EXPECT_EQ(state, uut.GetActiveState());

    // exits back to root on completed
    uut.NotifyCompleteState();
    const auto exitTransition = uut.CheckForTransition();
    EXPECT_EQ(2, exitTransition.fromAnimId);
    EXPECT_EQ(1, exitTransition.toAnimId);
    EXPECT_EQ(nc::AnimationTransitionType::Loop, exitTransition.type);
    EXPECT_EQ(nc::RootAnimationState, uut.GetActiveState());
}

TEST(SkeletalAnimationControllerTest, CheckForTransition_successorCycle_transitionsThroughCycle)
{
    auto uut = nc::SkeletalAnimationController{1, 0.1f};
    uut.CheckForTransition();

    const auto stopState = uut.AddState(nc::StopAnimation{
        .enterWhen = []() { return true; },
        .enterFrom = nc::RootAnimationState
    });

    const auto loopState = uut.AddState(nc::LoopAnimation{
        .animId = 2,
        .enterWhen = []() { return true; },
        .enterFrom = stopState,
        .exitWhen = []() { return true; },
        .exitTo = nc::RootAnimationState
    });

    // expect to repeatedly cycle from root -> stop -> loop
    const auto stopTransition = uut.CheckForTransition();
    EXPECT_EQ(1, stopTransition.fromAnimId);
    EXPECT_EQ(nc::asset::NullAssetId, stopTransition.toAnimId);
    EXPECT_EQ(nc::AnimationTransitionType::Stop, stopTransition.type);
    EXPECT_EQ(stopState, uut.GetActiveState());

    const auto loopTransition = uut.CheckForTransition();
    EXPECT_EQ(nc::asset::NullAssetId, loopTransition.fromAnimId);
    EXPECT_EQ(2, loopTransition.toAnimId);
    EXPECT_EQ(nc::AnimationTransitionType::Loop, loopTransition.type);
    EXPECT_EQ(loopState, uut.GetActiveState());

    const auto exitTransition = uut.CheckForTransition();
    EXPECT_EQ(2, exitTransition.fromAnimId);
    EXPECT_EQ(1, exitTransition.toAnimId);
    EXPECT_EQ(nc::AnimationTransitionType::Loop, exitTransition.type);
    EXPECT_EQ(nc::RootAnimationState, uut.GetActiveState());

    const auto stopTransition2 = uut.CheckForTransition();
    EXPECT_EQ(1, stopTransition2.fromAnimId);
    EXPECT_EQ(nc::asset::NullAssetId, stopTransition2.toAnimId);
    EXPECT_EQ(nc::AnimationTransitionType::Stop, stopTransition2.type);
    EXPECT_EQ(stopState, uut.GetActiveState());
}

TEST(SkeletalAnimationControllerTest, CheckForTransition_afterRefreshState_retransitions)
{
    auto uut = nc::SkeletalAnimationController{1, 0.1f};
    uut.CheckForTransition();

    const auto loopState = uut.AddState(nc::LoopAnimation{
        .animId = 2,
        .enterWhen = []() { return true; },
    });

    const auto initialTransition = uut.CheckForTransition();
    EXPECT_EQ(1, initialTransition.fromAnimId);
    EXPECT_EQ(2, initialTransition.toAnimId);
    EXPECT_EQ(nc::AnimationTransitionType::Loop, initialTransition.type);
    EXPECT_EQ(loopState, uut.GetActiveState());

    // forces a transition
    uut.RefreshAnimation();
    const auto refreshTransition = uut.CheckForTransition();
    EXPECT_EQ(2, refreshTransition.fromAnimId);
    EXPECT_EQ(2, refreshTransition.toAnimId);
    EXPECT_EQ(nc::AnimationTransitionType::Loop, refreshTransition.type);
    EXPECT_EQ(loopState, uut.GetActiveState());
}

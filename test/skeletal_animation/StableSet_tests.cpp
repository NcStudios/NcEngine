#include "gtest/gtest.h"
#include "ncengine/graphics/SkeletalAnimationTypes.h"

using namespace nc::graphics::anim;

TEST(StableSet_test, Insert_exceedsMaxSize_throws)
{
    auto uut = StableSet{10};
    uut.Insert(State{Root{"dummy"}});
    uut.Insert(State{Root{"dummy"}});
    uut.Insert(State{Root{"dummy"}});
    uut.Insert(State{Root{"dummy"}});
    uut.Insert(State{Root{"dummy"}});
    uut.Insert(State{Root{"dummy"}});
    uut.Insert(State{Root{"dummy"}});
    uut.Insert(State{Root{"dummy"}});
    uut.Insert(State{Root{"dummy"}});
    uut.Insert(State{Root{"dummy"}});

    EXPECT_THROW(uut.Insert(State{Root{"dummy"}}), std::runtime_error);
}

TEST(StableSet_test, Remove_insertAfterRemove_indexIsCorrect)
{
    auto uut = StableSet{10};
    auto first = uut.Insert(State{Root{"dummy1"}});
    auto second = uut.Insert(State{Root{"dummy2"}});
    uut.Remove(first);
    auto secondReturned = uut.Get(second);
    ASSERT_EQ(second, secondReturned->id);
}

TEST(StableSet_test, Remove_insertAfterRemove_removedReturnsNullPtr)
{
    auto uut = StableSet{10};
    uut.Insert(State{Root{"dummy1"}});
    auto second = uut.Insert(State{Root{"dummy2"}});
    uut.Remove(second);
    uut.Get(second);
}

TEST(StableSet_test, Get_insertAfterRemove_handlesStillValid)
{
    auto uut = StableSet{10};
    auto first  = uut.Insert(State{Root{"dummy1"}});
    auto second = uut.Insert(State{Root{"dummy2"}});
    auto third  = uut.Insert(State{Root{"dummy3"}});
    uut.Remove(second);
    auto fourth = uut.Insert(State{Root{"dummy4"}});

    auto firstReturned = uut.Get(first);
    auto thirdReturned = uut.Get(third);
    auto fourthReturned = uut.Get(fourth);

    ASSERT_EQ(firstReturned->animUid, "dummy1");
    ASSERT_EQ(thirdReturned->animUid, "dummy3");
    ASSERT_EQ(fourthReturned->animUid, "dummy4");
}

TEST(StableSet_test, GetLast_noStates_ReturnsNullPtr)
{
    auto uut = StableSet{10};
    auto last = uut.GetLast();
    ASSERT_EQ(last, nullptr);
}



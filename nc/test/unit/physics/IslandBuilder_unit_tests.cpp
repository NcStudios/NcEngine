#include "gtest/gtest.h"

#include "physics/IslandBuilder.h"

using namespace nc::physics;

TEST(IslandBuilder_unit_tests, FindIslands_NoColliders_ReturnsNoIslands)
{
    std::vector<BroadEvent> events {};
    IslandBuilder builder;
    auto islands = builder.FindIslands(0u, events);
    EXPECT_EQ(islands.size(), 0u);
}

TEST(IslandBuilder_unit_tests, FindIslands_NoEvents_ReturnsOneIslandPerCollider)
{
    std::vector<BroadEvent> events {};
    IslandBuilder builder;
    auto islands = builder.FindIslands(3u, events);
    ASSERT_EQ(islands.size(), 3u);
    EXPECT_EQ(islands[0][0].colliderIndex, 0u);
    EXPECT_EQ(islands[1][0].colliderIndex, 1u);
    EXPECT_EQ(islands[2][0].colliderIndex, 2u);
}

TEST(IslandBuilder_unit_tests, FindIslands_AllCollidersTouching_ReturnsOneIsland)
{
    std::vector<BroadEvent> singleEvent
    {
        {.first = 0u, .second = 1u, .eventType = CollisionEventType::TwoBodyPhysics}
    };

    IslandBuilder builder;
    auto islands = builder.FindIslands(2u, singleEvent);
    ASSERT_EQ(islands.size(), 1u);
    ASSERT_EQ(islands[0].size(), 2u);
    EXPECT_EQ(islands[0][0].colliderIndex, 0u);
    EXPECT_EQ(islands[0][1].colliderIndex, 1u);

    std::vector<BroadEvent> multipleEvents
    {
        {.first = 1u, .second = 2u, .eventType = CollisionEventType::TwoBodyPhysics},
        {.first = 3u, .second = 0u, .eventType = CollisionEventType::TwoBodyPhysics},
        {.first = 3u, .second = 2u, .eventType = CollisionEventType::TwoBodyPhysics},
    };

    islands = builder.FindIslands(4, multipleEvents);
    ASSERT_EQ(islands.size(), 1u);
    EXPECT_EQ(islands[0][0].colliderIndex, 0u);
    EXPECT_EQ(islands[0][1].colliderIndex, 1u);
    EXPECT_EQ(islands[0][2].colliderIndex, 2u);
    EXPECT_EQ(islands[0][3].colliderIndex, 3u);
}

TEST(IslandBuilder_unit_tests, FindIslands_OneCollision_OtherCollidersRemainInUniqueIslands)
{
    std::vector<BroadEvent> events
    {
        {.first = 4u, .second = 2u, .eventType = CollisionEventType::TwoBodyPhysics}
    };

    IslandBuilder builder;
    auto islands = builder.FindIslands(6u, events);
    ASSERT_EQ(islands.size(), 5u);

    for(const auto& island : islands)
    {
        if(island.size() == 1)
            continue;
        
        ASSERT_EQ(island.size(), 2u);
        EXPECT_EQ(island[0].colliderIndex, 2u);
        EXPECT_EQ(island[1].colliderIndex, 4u);
    }
}

TEST(IslandBuilder_unit_tests, FindIslands_EventsInDisjointGroups_ReturnsDisjointIslands)
{
    std::vector<BroadEvent> events
    {
        {.first = 0u, .second = 1u, .eventType = CollisionEventType::TwoBodyPhysics},
        {.first = 0u, .second = 2u, .eventType = CollisionEventType::TwoBodyPhysics},
        {.first = 4u, .second = 6u, .eventType = CollisionEventType::TwoBodyPhysics},
        {.first = 5u, .second = 0u, .eventType = CollisionEventType::TwoBodyPhysics},
    };

    IslandBuilder builder;
    auto islands = builder.FindIslands(7, events);
    ASSERT_EQ(islands.size(), 3u);

    const auto totalItemCount = islands[0].size() + islands[1].size() + islands[2].size();
    ASSERT_EQ(totalItemCount, 7u);

    for(const auto& island : islands)
    {
        switch(island.size())
        {
            case 1:
            {
                EXPECT_EQ(island[0].colliderIndex, 3u);
                break;
            }
            case 2:
            {
                EXPECT_EQ(island[0].colliderIndex, 4u);
                EXPECT_EQ(island[1].colliderIndex, 6u);
                break;
            }
            case 4:
            {
                EXPECT_EQ(island[0].colliderIndex, 0u);
                EXPECT_EQ(island[1].colliderIndex, 1u);
                EXPECT_EQ(island[2].colliderIndex, 2u);
                EXPECT_EQ(island[3].colliderIndex, 5u);
                break;
            }
            default:
            {
                FAIL();
            }
        }
    }
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
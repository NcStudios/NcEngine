#include "gtest/gtest.h"
#include "ncengine/scene/NcScene.h"

int loadCalls = 0;
int unloadCalls = 0;

struct TestScene : public nc::Scene
{
    void Load(nc::Registry*, nc::ModuleProvider) override
    {
        ++loadCalls;
    }

    void Unload() override
    {
        ++unloadCalls;
    }
};

class NcSceneTests : public testing::Test
{
    public:
        std::unique_ptr<nc::NcScene> uut = nc::BuildSceneModule();
        nc::Registry* registry = nullptr; // nullptr is fine, just passes along
        nc::ModuleRegistry modules; // don't need any modules registered

        NcSceneTests()
        {
            loadCalls = 0;
            unloadCalls = 0;
        }
};

TEST_F(NcSceneTests, Queue_returnsQueuePosition)
{
    EXPECT_EQ(0, uut->Queue(std::make_unique<TestScene>()));
    EXPECT_EQ(1, uut->Queue(std::make_unique<TestScene>()));
}

TEST_F(NcSceneTests, DequeueScene_validPosition_removesScene)
{
    uut->Queue(std::make_unique<TestScene>());
    uut->Queue(std::make_unique<TestScene>());
    EXPECT_EQ(2, uut->GetNumberOfScenesInQueue());
    uut->DequeueScene(0);
    EXPECT_EQ(1, uut->GetNumberOfScenesInQueue());
    uut->DequeueScene(0);
    EXPECT_EQ(0, uut->GetNumberOfScenesInQueue());
}

TEST_F(NcSceneTests, DequeueScene_invalidPosition_throws)
{
    uut->Queue(std::make_unique<TestScene>());
    EXPECT_THROW(uut->DequeueScene(1), nc::NcError);
    EXPECT_NO_THROW(uut->DequeueScene(0));
    EXPECT_THROW(uut->DequeueScene(0), nc::NcError);
}

TEST_F(NcSceneTests, IsTransitionScheduled__returnsCurrentTransitionFlag)
{
    EXPECT_FALSE(uut->IsTransitionScheduled());
    uut->ScheduleTransition();
    EXPECT_TRUE(uut->IsTransitionScheduled());
}

TEST_F(NcSceneTests, LoadQueuedScene_sceneQueued_callsLoadAndReturnsTrue)
{
    uut->Queue(std::make_unique<TestScene>());
    uut->ScheduleTransition();
    EXPECT_TRUE(uut->LoadQueuedScene(registry, modules));
    EXPECT_EQ(1, loadCalls);
    EXPECT_EQ(0, uut->GetNumberOfScenesInQueue());
    EXPECT_FALSE(uut->IsTransitionScheduled());
}

TEST_F(NcSceneTests, LoadQueuedScene_noSceneQueued_returnsFalse)
{
    EXPECT_FALSE(uut->LoadQueuedScene(registry, modules));
}

TEST_F(NcSceneTests, LoadQueuedScene_hasActiveScene_throws)
{
    uut->Queue(std::make_unique<TestScene>());
    uut->Queue(std::make_unique<TestScene>());
    ASSERT_TRUE(uut->LoadQueuedScene(registry, modules));
    EXPECT_THROW(uut->LoadQueuedScene(registry, modules), nc::NcError);
}

TEST_F(NcSceneTests, UnloadActiveScene_hasActiveScene_unloadsAndReturnsTrue)
{
    uut->Queue(std::make_unique<TestScene>());
    ASSERT_TRUE(uut->LoadQueuedScene(registry, modules));
    EXPECT_TRUE(uut->UnloadActiveScene());
    EXPECT_EQ(1, unloadCalls);
}

TEST_F(NcSceneTests, UnloadActiveScene_noActiveScene_returnsFalse)
{
    EXPECT_FALSE(uut->UnloadActiveScene());
}

TEST_F(NcSceneTests, LoadUnload_multipleCalls_succeed)
{
    uut->Queue(std::make_unique<TestScene>());
    uut->Queue(std::make_unique<TestScene>());
    uut->LoadQueuedScene(registry, modules);
    uut->UnloadActiveScene();
    uut->LoadQueuedScene(registry, modules);
    uut->UnloadActiveScene();
    uut->Queue(std::make_unique<TestScene>());
    uut->UnloadActiveScene();
    uut->LoadQueuedScene(registry, modules);
    uut->UnloadActiveScene();
    EXPECT_EQ(0, uut->GetNumberOfScenesInQueue());
    EXPECT_EQ(3, loadCalls);
    EXPECT_EQ(3, unloadCalls);
}

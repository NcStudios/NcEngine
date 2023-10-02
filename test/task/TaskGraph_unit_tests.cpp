#include "gtest/gtest.h"
#include "task/TaskGraph.h"

class TestTaskGraph : public nc::task::TaskGraph
{
    public:
        TestTaskGraph() = default;

        auto GetTaskFlow() -> tf::Taskflow&
        {
            return m_ctx->graph;
        }

        auto GetTaskBucket(nc::task::ExecutionPhase phase) -> const std::vector<tf::Task>&
        {
            return m_taskBuckets.at(static_cast<size_t>(phase));
        }

        auto GetGraphStorage() -> const std::vector<std::unique_ptr<tf::Taskflow>>&
        {
            return m_ctx->storage;
        }
};

TEST(TaskGraphTests, Add_callableOverload_addsToPhase)
{
    constexpr auto phase = nc::task::ExecutionPhase::Render;
    auto uut = TestTaskGraph{};
    ASSERT_TRUE(uut.GetTaskBucket(phase).empty());
    uut.Add(phase, "task1", [](){});
    uut.Add(phase, "task2", [](){});
    uut.Add(phase, "task3", [](){});
    EXPECT_EQ(3, uut.GetTaskBucket(phase).size());
}

TEST(TaskGraphTests, Add_taskflowOverload_addsToPhaseAndStorage)
{
    constexpr auto phase = nc::task::ExecutionPhase::Begin;
    auto uut = TestTaskGraph{};
    ASSERT_TRUE(uut.GetTaskBucket(phase).empty());
    ASSERT_TRUE(uut.GetGraphStorage().empty());
    uut.Add(phase, "testTask", std::make_unique<tf::Taskflow>());
    EXPECT_EQ(1, uut.GetTaskBucket(phase).size());
    EXPECT_EQ(1, uut.GetGraphStorage().size());
}

TEST(TaskGraphTests, Add_invalidPhase_throws)
{
    constexpr auto phase = static_cast<nc::task::ExecutionPhase>(1000);
    auto uut = TestTaskGraph{};
    EXPECT_THROW(uut.Add(phase, "", [](){}), std::exception);
}

TEST(TaskGraphTests, StoreGraph_addsToStorage)
{
    auto uut = TestTaskGraph{};
    ASSERT_TRUE(uut.GetGraphStorage().empty());
    uut.StoreGraph(std::make_unique<tf::Taskflow>());
    EXPECT_EQ(1, uut.GetGraphStorage().size());
}

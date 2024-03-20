#include "gtest/gtest.h"
#include "task/TaskGraph.h"

class TestTaskGraph : public nc::task::TaskGraph<nc::task::UpdatePhase>
{
    public:
        TestTaskGraph() = default;

        auto GetTaskFlow() -> tf::Taskflow&
        {
            return m_ctx->graph;
        }

        auto GetTasks() -> const std::vector<nc::task::Task>&
        {
            return m_tasks;
        }

        auto GetGraphStorage() -> const std::vector<std::unique_ptr<tf::Taskflow>>&
        {
            return m_ctx->storage;
        }
};

TEST(TaskGraphTests, Add_callableOverload_addsToPhase)
{
    auto uut = TestTaskGraph{};
    ASSERT_TRUE(uut.GetTasks().empty());
    uut.Add(0, "task1", [](){});
    uut.Add(1, "task2", [](){});
    uut.Add(2, "task3", [](){});
    EXPECT_EQ(3, uut.GetTasks().size());
}

TEST(TaskGraphTests, Add_taskflowOverload_addsToPhaseAndStorage)
{
    auto uut = TestTaskGraph{};
    ASSERT_TRUE(uut.GetTasks().empty());
    ASSERT_TRUE(uut.GetGraphStorage().empty());
    uut.Add(0, "testTask", std::make_unique<tf::Taskflow>());
    EXPECT_EQ(1, uut.GetTasks().size());
    EXPECT_EQ(1, uut.GetGraphStorage().size());
}

TEST(TaskGraphTests, StoreGraph_addsToStorage)
{
    auto uut = TestTaskGraph{};
    ASSERT_TRUE(uut.GetGraphStorage().empty());
    uut.StoreGraph(std::make_unique<tf::Taskflow>());
    EXPECT_EQ(1, uut.GetGraphStorage().size());
}

#include "gtest/gtest.h"
#include "task/Executor.h"

#include <mutex>
#include <sstream>

auto s_numTasksRun = size_t{};
auto s_invokeOrder = std::vector<nc::task::ExecutionPhase>{};
auto s_taskMutex = std::mutex{}; // for safety in case scheduling is broken

// Test tasks should call this to track that/when they're invoked.
void RegisterTaskInvocation(nc::task::ExecutionPhase taskId)
{
    auto lock = std::lock_guard{s_taskMutex};
    ++s_numTasksRun;
    s_invokeOrder.push_back(taskId);
}

struct SingleTaskModule : nc::Module
{
    void OnBuildTaskGraph(nc::task::TaskGraph& graph) override
    {
        // Note: If setting dependencies is totally stripped from the executor,
        // tasks tend to finish in the order they're added. Tasks should be
        // added in reverse phase order to prevent false positives.

        graph.Add(nc::task::ExecutionPhase::PostFrameSync, "", [] {
            RegisterTaskInvocation(nc::task::ExecutionPhase::PostFrameSync); }
        );

        graph.Add(nc::task::ExecutionPhase::Render, "", [] {
            RegisterTaskInvocation(nc::task::ExecutionPhase::Render);
        });

        graph.Add(nc::task::ExecutionPhase::Logic, "", [] {
            RegisterTaskInvocation(nc::task::ExecutionPhase::Logic);
        });
    }
};

struct GraphModule : nc::Module
{
    void OnBuildTaskGraph(nc::task::TaskGraph& graph) override
    {
        auto tf1 = std::make_unique<tf::Taskflow>();
        tf1->emplace([] { RegisterTaskInvocation(nc::task::ExecutionPhase::PreRenderSync); });
        graph.Add(nc::task::ExecutionPhase::PreRenderSync, "", std::move(tf1));

        auto tf2 = std::make_unique<tf::Taskflow>();
        tf2->emplace([] { RegisterTaskInvocation(nc::task::ExecutionPhase::Physics); });
        graph.Add(nc::task::ExecutionPhase::Physics, "", std::move(tf2));

        auto tf3 = std::make_unique<tf::Taskflow>();
        tf3->emplace([] { RegisterTaskInvocation(nc::task::ExecutionPhase::Begin); });
        graph.Add(nc::task::ExecutionPhase::Begin, "", std::move(tf3));
    }
};

struct ThrowingModule : nc::Module
{
    void OnBuildTaskGraph(nc::task::TaskGraph& graph) override
    {
        graph.Add(nc::task::ExecutionPhase::Begin, "", [] {
            RegisterTaskInvocation(nc::task::ExecutionPhase::Begin);
            throw std::exception("failed task");
        });
    }
};

// Test helper to build a module list
template<std::derived_from<nc::Module>... Ts>
auto BuildModules() -> std::vector<std::unique_ptr<nc::Module>>
{
    auto out = std::vector<std::unique_ptr<nc::Module>>{};
    (out.push_back(std::make_unique<Ts>()), ...);
    return out;
}

// Fixture to wipe counters
class ExecutorTests : public ::testing::Test
{
    public:
        ExecutorTests()
        {
            s_numTasksRun = 0;
            s_invokeOrder.clear();
        }
};

TEST_F(ExecutorTests, BuildContext_succeeds)
{
    auto modules = BuildModules<SingleTaskModule, GraphModule, ThrowingModule>();
    EXPECT_NO_THROW(nc::task::BuildContext(modules));
}

TEST_F(ExecutorTests, PrintGraph_writesToStream)
{
    auto modules = BuildModules<SingleTaskModule>();
    auto uut = nc::task::Executor{nc::task::BuildContext(modules)};
    auto stream = std::ostringstream{};
    uut.WriteGraph(stream);
    // Just checking that it succeeded and wrote something
    EXPECT_NE(std::streampos{0}, stream.tellp());
}

TEST_F(ExecutorTests, Run_allPhases_schedulesCorrectly)
{
    auto modules = BuildModules<SingleTaskModule, GraphModule>();
    auto uut = nc::task::Executor{nc::task::BuildContext(modules)};
    EXPECT_NO_THROW(uut.Run());
    EXPECT_EQ(6, s_numTasksRun);
    ASSERT_EQ(6, s_invokeOrder.size());
    // Not adding Free phase tasks so that the order is predicable
    EXPECT_EQ(nc::task::ExecutionPhase::Begin, s_invokeOrder.at(0));
    EXPECT_EQ(nc::task::ExecutionPhase::Logic, s_invokeOrder.at(1));
    EXPECT_EQ(nc::task::ExecutionPhase::Physics, s_invokeOrder.at(2));
    EXPECT_EQ(nc::task::ExecutionPhase::PreRenderSync, s_invokeOrder.at(3));
    EXPECT_EQ(nc::task::ExecutionPhase::Render, s_invokeOrder.at(4));
    EXPECT_EQ(nc::task::ExecutionPhase::PostFrameSync, s_invokeOrder.at(5));
}

TEST_F(ExecutorTests, Run_missingPhases_schedulesCorrectly)
{
    auto modules = BuildModules<SingleTaskModule>();
    auto uut = nc::task::Executor{nc::task::BuildContext(modules)};
    EXPECT_NO_THROW(uut.Run());
    ASSERT_EQ(3, s_numTasksRun);
    ASSERT_EQ(3, s_invokeOrder.size());
    // Test dependencies propagate across empty phases - i.e. with no physics
    // tasks, rendering tasks are still scheduled after logic
    EXPECT_EQ(nc::task::ExecutionPhase::Logic, s_invokeOrder.at(0));
    EXPECT_EQ(nc::task::ExecutionPhase::Render, s_invokeOrder.at(1));
    EXPECT_EQ(nc::task::ExecutionPhase::PostFrameSync, s_invokeOrder.at(2));
}

TEST_F(ExecutorTests, Run_taskThrows_completesGraph)
{
    auto modules = BuildModules<SingleTaskModule, ThrowingModule>();
    auto uut = nc::task::Executor{nc::task::BuildContext(modules)};
    EXPECT_THROW(uut.Run(), std::exception);
    EXPECT_EQ(4, s_numTasksRun); // should still have run the other tasks
    ASSERT_EQ(4, s_invokeOrder.size());
}

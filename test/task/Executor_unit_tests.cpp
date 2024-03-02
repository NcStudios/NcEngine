#include "gtest/gtest.h"
#include "task/Executor.h"

#include <mutex>
#include <sstream>

auto s_numTasksRun = size_t{};
auto s_updateInvokeOrder = std::vector<nc::task::UpdatePhase>{};
auto s_renderInvokeOrder = std::vector<nc::task::RenderPhase>{};
auto s_taskMutex = std::mutex{}; // for safety in case scheduling is broken

// Test tasks should call this to track that/when they're invoked.
void RegisterTaskInvocation(nc::task::UpdatePhase taskId)
{
    auto lock = std::lock_guard{s_taskMutex};
    ++s_numTasksRun;
    s_updateInvokeOrder.push_back(taskId);
}

void RegisterTaskInvocation(nc::task::RenderPhase taskId)
{
    auto lock = std::lock_guard{s_taskMutex};
    ++s_numTasksRun;
    s_renderInvokeOrder.push_back(taskId);
}

// Helper to build modules from a type list
template<std::derived_from<nc::Module>... Ts>
auto BuildModules() -> std::vector<std::unique_ptr<nc::Module>>
{
    auto out = std::vector<std::unique_ptr<nc::Module>>{};
    (out.push_back(std::make_unique<Ts>()), ...);
    return out;
}

// Module that schedules individual tasks
struct SingleTaskModule : nc::Module
{
    static constexpr auto UpdateTaskCount = 2ull;
    static constexpr auto RenderTaskCount = 1ull;

    void OnBuildTaskGraph(nc::task::UpdateTasks& update, nc::task::RenderTasks& render) override
    {
        // Note: If setting dependencies is totally stripped from the executor,
        // tasks tend to finish in the order they're added. Tasks should be
        // added in reverse phase order to prevent false positives.
        update.Add(nc::task::UpdatePhase::Sync, "", [] {
            RegisterTaskInvocation(nc::task::UpdatePhase::Sync); }
        );

        update.Add(nc::task::UpdatePhase::Logic, "", [] {
            RegisterTaskInvocation(nc::task::UpdatePhase::Logic);
        });

        render.Add(nc::task::RenderPhase::Render, "", [] {
            RegisterTaskInvocation(nc::task::RenderPhase::Render);
        });
    }
};

// Module that schedules tf::Taskflows
struct GraphModule : nc::Module
{
    static constexpr auto UpdateTaskCount = 3ull;
    static constexpr auto RenderTaskCount = 1ull;

    void OnBuildTaskGraph(nc::task::UpdateTasks& update, nc::task::RenderTasks& render) override
    {
        auto tf1 = std::make_unique<tf::Taskflow>();
        tf1->emplace([] { RegisterTaskInvocation(nc::task::UpdatePhase::Sync); });
        update.Add(nc::task::UpdatePhase::Sync, "", std::move(tf1));

        auto tf2 = std::make_unique<tf::Taskflow>();
        tf2->emplace([] { RegisterTaskInvocation(nc::task::UpdatePhase::Physics); });
        update.Add(nc::task::UpdatePhase::Physics, "", std::move(tf2));

        auto tf3 = std::make_unique<tf::Taskflow>();
        tf3->emplace([] { RegisterTaskInvocation(nc::task::UpdatePhase::Begin); });
        update.Add(nc::task::UpdatePhase::Begin, "", std::move(tf3));

        auto tf4 = std::make_unique<tf::Taskflow>();
        tf4->emplace([] { RegisterTaskInvocation(nc::task::RenderPhase::PostRender); });
        render.Add(nc::task::RenderPhase::PostRender, "", std::move(tf4));
    }
};

// Module whose task throws
struct ThrowingModule : nc::Module
{
    static constexpr auto UpdateTaskCount = 1ull;
    static constexpr auto RenderTaskCount = 0ull;

    void OnBuildTaskGraph(nc::task::UpdateTasks& graph, nc::task::RenderTasks&) override
    {
        graph.Add(nc::task::UpdatePhase::Begin, "", [] {
            RegisterTaskInvocation(nc::task::UpdatePhase::Begin);
            throw std::runtime_error("failed task");
        });
    }
};

// Module whose task tries to rerun the executor
struct DoubleRunModule : nc::Module
{
    static constexpr auto UpdateTaskCount = 1ull;
    static constexpr auto RenderTaskCount = 0ull;

    nc::task::Executor* executor = nullptr;
    void OnBuildTaskGraph(nc::task::UpdateTasks& graph, nc::task::RenderTasks&) override
    {
        graph.Add(nc::task::UpdatePhase::Begin, "", [this] {
            if (!executor)
            {
                FAIL() << "Bad test setup";
            }

            executor->RunUpdateTasks();
        });
    }
};

// Module whose task tries to rebuild the graph its running on
struct GraphRebuildModule : nc::Module
{
    static constexpr auto UpdateTaskCount = 1ull;
    static constexpr auto RenderTaskCount = 0ull;

    nc::task::Executor* executor = nullptr;
    void OnBuildTaskGraph(nc::task::UpdateTasks& graph, nc::task::RenderTasks&) override
    {
        graph.Add(nc::task::UpdatePhase::Begin, "", [this] {
            if (!executor)
            {
                FAIL() << "Bad test setup";
            }

            auto modules = BuildModules<SingleTaskModule>();
            executor->SetContext(nc::task::BuildContext(modules));
        });
    }
};

// Fixture to wipe counters
class ExecutorTests : public ::testing::Test
{
    public:
        ExecutorTests()
        {
            s_numTasksRun = 0;
            s_updateInvokeOrder.clear();
            s_renderInvokeOrder.clear();
        }
};

TEST_F(ExecutorTests, BuildContext_succeeds)
{
    auto modules = BuildModules<SingleTaskModule, GraphModule, ThrowingModule>();
    EXPECT_NO_THROW(nc::task::BuildContext(modules));
}

TEST_F(ExecutorTests, Run_allPhases_schedulesCorrectly)
{
    auto modules = BuildModules<SingleTaskModule, GraphModule>();
    auto uut = nc::task::Executor{nc::task::BuildContext(modules)};
    EXPECT_NO_THROW(uut.RunUpdateTasks());
    EXPECT_NO_THROW(uut.RunRenderTasks());
    const auto expectedUpdateTaskCount = SingleTaskModule::UpdateTaskCount + GraphModule::UpdateTaskCount;
    const auto expectedRenderTaskCount = SingleTaskModule::RenderTaskCount + GraphModule::RenderTaskCount;
    EXPECT_EQ(expectedUpdateTaskCount + expectedRenderTaskCount, s_numTasksRun);
    ASSERT_EQ(expectedUpdateTaskCount, s_updateInvokeOrder.size());
    ASSERT_EQ(expectedRenderTaskCount, s_renderInvokeOrder.size());
    // Not adding Free phase tasks so that the order is predicable
    EXPECT_EQ(nc::task::UpdatePhase::Begin, s_updateInvokeOrder.at(0));
    EXPECT_EQ(nc::task::UpdatePhase::Logic, s_updateInvokeOrder.at(1));
    EXPECT_EQ(nc::task::UpdatePhase::Physics, s_updateInvokeOrder.at(2));
    EXPECT_EQ(nc::task::UpdatePhase::Sync, s_updateInvokeOrder.at(3));
    EXPECT_EQ(nc::task::UpdatePhase::Sync, s_updateInvokeOrder.at(4));

    EXPECT_EQ(nc::task::RenderPhase::Render, s_renderInvokeOrder.at(0));
    EXPECT_EQ(nc::task::RenderPhase::PostRender, s_renderInvokeOrder.at(1));
}

TEST_F(ExecutorTests, Run_missingPhases_schedulesCorrectly)
{
    auto modules = BuildModules<SingleTaskModule>();
    auto uut = nc::task::Executor{nc::task::BuildContext(modules)};
    EXPECT_NO_THROW(uut.RunUpdateTasks());
    ASSERT_EQ(SingleTaskModule::UpdateTaskCount, s_numTasksRun);
    ASSERT_EQ(SingleTaskModule::UpdateTaskCount, s_updateInvokeOrder.size());
    // Test dependencies propagate across empty phases - i.e. with no physics
    // tasks, sync tasks are still scheduled after logic
    EXPECT_EQ(nc::task::UpdatePhase::Logic, s_updateInvokeOrder.at(0));
    EXPECT_EQ(nc::task::UpdatePhase::Sync, s_updateInvokeOrder.at(1));
}

TEST_F(ExecutorTests, Run_taskThrows_completesGraph)
{
    auto modules = BuildModules<SingleTaskModule, ThrowingModule>();
    auto uut = nc::task::Executor{nc::task::BuildContext(modules)};
    EXPECT_THROW(uut.RunUpdateTasks(), std::exception);
    const auto expectedUpdateTaskCount = SingleTaskModule::UpdateTaskCount + ThrowingModule::UpdateTaskCount;
    EXPECT_EQ(expectedUpdateTaskCount, s_numTasksRun); // should still have run the other tasks
    ASSERT_EQ(expectedUpdateTaskCount, s_updateInvokeOrder.size());
}

TEST_F(ExecutorTests, Run_alreadyRunning_throws)
{
    auto modules = BuildModules<GraphRebuildModule>();
    auto uut = nc::task::Executor{nc::task::BuildContext(modules)};
    dynamic_cast<GraphRebuildModule*>(modules.at(0).get())->executor = &uut;
    EXPECT_THROW(uut.RunUpdateTasks(), nc::NcError);
}

TEST_F(ExecutorTests, SetContext_subsequentRun_succeeds)
{
    auto modules = BuildModules<SingleTaskModule>();
    auto uut = nc::task::Executor{nc::task::BuildContext(modules)};
    EXPECT_NO_THROW(uut.RunUpdateTasks());
    EXPECT_NO_THROW(uut.RunRenderTasks());
    auto newModules = BuildModules<SingleTaskModule, GraphModule>();
    EXPECT_NO_THROW(uut.SetContext(nc::task::BuildContext(newModules)));
    EXPECT_NO_THROW(uut.RunUpdateTasks());
    EXPECT_NO_THROW(uut.RunRenderTasks());
    const auto expectedUpdateTaskCount = SingleTaskModule::UpdateTaskCount * 2 + GraphModule::UpdateTaskCount;
    const auto expectedRenderTaskCount = SingleTaskModule::RenderTaskCount * 2 + GraphModule::RenderTaskCount;
    EXPECT_EQ(expectedUpdateTaskCount + expectedRenderTaskCount, s_numTasksRun);
    EXPECT_EQ(expectedUpdateTaskCount, s_updateInvokeOrder.size());
    EXPECT_EQ(expectedRenderTaskCount, s_renderInvokeOrder.size());
}

TEST_F(ExecutorTests, SetContext_graphRunning_throws)
{
    auto modules = BuildModules<GraphRebuildModule>();
    auto uut = nc::task::Executor{nc::task::BuildContext(modules)};
    dynamic_cast<GraphRebuildModule*>(modules.at(0).get())->executor = &uut;
    EXPECT_THROW(uut.RunUpdateTasks(), nc::NcError);
}

TEST_F(ExecutorTests, WriteGraph_writesToStream)
{
    auto modules = BuildModules<SingleTaskModule>();
const auto uut = nc::task::Executor{nc::task::BuildContext(modules)};
    auto stream = std::ostringstream{};
    uut.WriteGraph(stream);
    // Just checking that it succeeded and wrote something
    EXPECT_NE(std::streampos{0}, stream.tellp());
}

#include "gtest/gtest.h"
#include "ncengine/task/Executor.h"

#include <mutex>
#include <sstream>

auto s_numTasksRun = size_t{};
auto s_updateInvokeOrder = std::vector<size_t>{};
auto s_renderInvokeOrder = std::vector<size_t>{};
auto s_taskMutex = std::mutex{}; // for safety in case scheduling is broken

constexpr size_t g_updateId1 = 1ull;
constexpr size_t g_updateId2 = 2ull;
constexpr size_t g_updateId3 = 3ull;
constexpr size_t g_updateId4 = 4ull;
constexpr size_t g_updateId5 = 5ull;

constexpr size_t g_renderId1 = 10ull;
constexpr size_t g_renderId2 = 11ull;

// Test tasks should call this to track that/when they're invoked.
void RegisterUpdateTaskInvocation(size_t taskId)
{
    auto lock = std::lock_guard{s_taskMutex};
    ++s_numTasksRun;
    s_updateInvokeOrder.push_back(taskId);
}

void RegisterRenderTaskInvocation(size_t taskId)
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
        update.Add(
            g_updateId2,
            "",
            [] { RegisterUpdateTaskInvocation(g_updateId2); },
            {g_updateId1}
        );

        update.Add(
            g_updateId1,
            "",
            [] { RegisterUpdateTaskInvocation(g_updateId1); }
        );

        render.Add(
            g_renderId1,
            "",
            [] { RegisterRenderTaskInvocation(g_renderId1); }
        );
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
        tf1->emplace([] { RegisterUpdateTaskInvocation(g_updateId5); });
        update.Add(g_updateId5, "", std::move(tf1), {g_updateId4});

        auto tf2 = std::make_unique<tf::Taskflow>();
        tf2->emplace([] { RegisterUpdateTaskInvocation(g_updateId4); });
        update.Add(g_updateId4, "", std::move(tf2), {g_updateId3});

        auto tf3 = std::make_unique<tf::Taskflow>();
        tf3->emplace([] { RegisterUpdateTaskInvocation(g_updateId3); });
        update.Add(g_updateId3, "", std::move(tf3), {g_updateId2});

        auto tf4 = std::make_unique<tf::Taskflow>();
        tf4->emplace([] { RegisterRenderTaskInvocation(g_renderId2); });
        render.Add(g_renderId2, "", std::move(tf4), {g_renderId1});
    }
};

// Module that schedules tasks using predecessors and successors
struct PredecessorAndSuccessorModule : nc::Module
{
    static constexpr auto UpdateTaskCount = 5ull;
    static constexpr auto RenderTaskCount = 0ull;

    void OnBuildTaskGraph(nc::task::UpdateTasks& update, nc::task::RenderTasks&) override
    {
        update.Add(
            g_updateId5,
            "",
            [] { RegisterUpdateTaskInvocation(g_updateId5); },
            {g_updateId4}
        );

        update.Add(
            g_updateId4,
            "",
            [] { RegisterUpdateTaskInvocation(g_updateId4); },
            {},
            {g_updateId5}
        );

        update.Add(
            g_updateId3,
            "",
            [] { RegisterUpdateTaskInvocation(g_updateId3); },
            {g_updateId2},
            {g_updateId4}
        );

        update.Add(
            g_updateId2,
            "",
            [] { RegisterUpdateTaskInvocation(g_updateId2); },
            {g_updateId1}
        );

        update.Add(
            g_updateId1,
            "",
            [] { RegisterUpdateTaskInvocation(g_updateId1); },
            {},
            {g_updateId2}
        );
    }
};

// Module whose task throws
struct ThrowingModule : nc::Module
{
    static constexpr auto UpdateTaskCount = 1ull;
    static constexpr auto RenderTaskCount = 0ull;

    void OnBuildTaskGraph(nc::task::UpdateTasks& graph, nc::task::RenderTasks&) override
    {
        graph.Add(g_updateId1, "", [] {
            RegisterUpdateTaskInvocation(g_updateId1);
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
        graph.Add(g_updateId1, "", [this] {
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
        graph.Add(g_updateId1, "", [this] {
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

TEST_F(ExecutorTests, Run_validGraph_schedulesCorrectly)
{
    auto modules = BuildModules<SingleTaskModule, GraphModule>();
    auto uut = nc::task::Executor{4, nc::task::BuildContext(modules)};
    EXPECT_NO_THROW(uut.RunUpdateTasks());
    EXPECT_NO_THROW(uut.RunRenderTasks());
    const auto expectedUpdateTaskCount = SingleTaskModule::UpdateTaskCount + GraphModule::UpdateTaskCount;
    const auto expectedRenderTaskCount = SingleTaskModule::RenderTaskCount + GraphModule::RenderTaskCount;
    EXPECT_EQ(expectedUpdateTaskCount + expectedRenderTaskCount, s_numTasksRun);
    ASSERT_EQ(expectedUpdateTaskCount, s_updateInvokeOrder.size());
    ASSERT_EQ(expectedRenderTaskCount, s_renderInvokeOrder.size());
    EXPECT_EQ(g_updateId1, s_updateInvokeOrder.at(0));
    EXPECT_EQ(g_updateId2, s_updateInvokeOrder.at(1));
    EXPECT_EQ(g_updateId3, s_updateInvokeOrder.at(2));
    EXPECT_EQ(g_updateId4, s_updateInvokeOrder.at(3));
    EXPECT_EQ(g_updateId5, s_updateInvokeOrder.at(4));

    EXPECT_EQ(g_renderId1, s_renderInvokeOrder.at(0));
    EXPECT_EQ(g_renderId2, s_renderInvokeOrder.at(1));
}

TEST_F(ExecutorTests, Run_graphScheduledWithPredecessorsAndSuccessors_schedulesCorrectly)
{
    auto modules = BuildModules<PredecessorAndSuccessorModule>();
    auto uut = nc::task::Executor{4, nc::task::BuildContext(modules)};
    EXPECT_NO_THROW(uut.RunUpdateTasks());
    EXPECT_NO_THROW(uut.RunRenderTasks());
    const auto expectedUpdateTaskCount = PredecessorAndSuccessorModule::UpdateTaskCount;
    const auto expectedRenderTaskCount = PredecessorAndSuccessorModule::RenderTaskCount;
    EXPECT_EQ(expectedUpdateTaskCount + expectedRenderTaskCount, s_numTasksRun);
    ASSERT_EQ(expectedUpdateTaskCount, s_updateInvokeOrder.size());
    ASSERT_EQ(expectedRenderTaskCount, s_renderInvokeOrder.size());
    EXPECT_EQ(g_updateId1, s_updateInvokeOrder.at(0));
    EXPECT_EQ(g_updateId2, s_updateInvokeOrder.at(1));
    EXPECT_EQ(g_updateId3, s_updateInvokeOrder.at(2));
    EXPECT_EQ(g_updateId4, s_updateInvokeOrder.at(3));
    EXPECT_EQ(g_updateId5, s_updateInvokeOrder.at(4));
}

TEST_F(ExecutorTests, Run_missingDependencies_throws)
{
    auto modules = BuildModules<GraphModule>();
    EXPECT_THROW(nc::task::Executor(4, nc::task::BuildContext(modules)), nc::NcError);
}

TEST_F(ExecutorTests, Run_taskThrows_completesGraph)
{
    auto modules = BuildModules<SingleTaskModule, ThrowingModule>();
    auto uut = nc::task::Executor{4, nc::task::BuildContext(modules)};
    EXPECT_THROW(uut.RunUpdateTasks(), std::exception);
    const auto expectedUpdateTaskCount = SingleTaskModule::UpdateTaskCount + ThrowingModule::UpdateTaskCount;
    EXPECT_EQ(expectedUpdateTaskCount, s_numTasksRun); // should still have run the other tasks
    ASSERT_EQ(expectedUpdateTaskCount, s_updateInvokeOrder.size());
}

TEST_F(ExecutorTests, Run_alreadyRunning_throws)
{
    auto modules = BuildModules<GraphRebuildModule>();
    auto uut = nc::task::Executor{4, nc::task::BuildContext(modules)};
    dynamic_cast<GraphRebuildModule*>(modules.at(0).get())->executor = &uut;
    EXPECT_THROW(uut.RunUpdateTasks(), nc::NcError);
}

TEST_F(ExecutorTests, SetContext_subsequentRun_succeeds)
{
    auto modules = BuildModules<SingleTaskModule>();
    auto uut = nc::task::Executor{4, nc::task::BuildContext(modules)};
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
    auto uut = nc::task::Executor{4, nc::task::BuildContext(modules)};
    dynamic_cast<GraphRebuildModule*>(modules.at(0).get())->executor = &uut;
    EXPECT_THROW(uut.RunUpdateTasks(), nc::NcError);
}

TEST_F(ExecutorTests, WriteGraph_writesToStream)
{
    auto modules = BuildModules<SingleTaskModule>();
const auto uut = nc::task::Executor{4, nc::task::BuildContext(modules)};
    auto stream = std::ostringstream{};
    uut.WriteGraph(stream);
    // Just checking that it succeeded and wrote something
    EXPECT_NE(std::streampos{0}, stream.tellp());
}

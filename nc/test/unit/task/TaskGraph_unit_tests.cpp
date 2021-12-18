#include "gtest/gtest.h"
#include "task/TaskGraph.h"

class TaskGraph_unit_tests : public ::testing::Test
{
    public:
        tf::Executor executor;
        nc::TaskGraph taskGraph;

        TaskGraph_unit_tests() : executor{2u}, taskGraph{} {}
        ~TaskGraph_unit_tests() {}
};

TEST_F(TaskGraph_unit_tests, AddGuardedTask_InitializesTask)
{
    auto task = taskGraph.AddGuardedTask([](){});
    EXPECT_TRUE(task.has_work());
    EXPECT_NE(nullptr, task.data());
}

TEST_F(TaskGraph_unit_tests, Run_TaskThrows_ThrowsUponCompletingAllTasks)
{
    int value = 0;
    auto task1 = taskGraph.AddGuardedTask([](){ throw std::runtime_error{"error"}; });
    auto task2 = taskGraph.AddGuardedTask([&value](){ value = 1; });
    task1.precede(task2);
    EXPECT_THROW(taskGraph.Run(executor), std::runtime_error);
    executor.wait_for_all();
    EXPECT_EQ(value, 1);
}

TEST_F(TaskGraph_unit_tests, RunAsync_TaskThrows_StoresException)
{
    taskGraph.AddGuardedTask([](){ throw std::runtime_error{"error"}; });
    auto result = taskGraph.RunAsync(executor);
    result.wait();
    EXPECT_THROW(taskGraph.ThrowIfExceptionStored(), std::runtime_error);
}

TEST_F(TaskGraph_unit_tests, RunAsync_TaskThrows_OtherTasksFinish)
{
    int value = 0;
    auto task1 = taskGraph.AddGuardedTask([](){ throw std::runtime_error{"error"}; });
    auto task2 = taskGraph.AddGuardedTask([&value](){ value = 1; });
    task1.precede(task2);
    taskGraph.RunAsync(executor).wait();
    EXPECT_EQ(value, 1);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
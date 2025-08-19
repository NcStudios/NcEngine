#include "gtest/gtest.h"
#include "ncutility/SpscQueue.h"

#include <algorithm>
#include <memory>
#include <ranges>
#include <thread>

TEST(SpscQueueTest, emplace_constructsInPlace)
{
    auto uut = nc::spsc_queue<std::string, 1>{};
    EXPECT_TRUE(uut.emplace(5, 'x'));

    auto actual = std::string{};
    EXPECT_TRUE(uut.pop(actual));
    EXPECT_EQ("xxxxx", actual);
}

TEST(SpscQueueTest, push_lValueOverload_returnsTrueUntilFull)
{
    constexpr auto n = 4;
    const auto value = 42;
    auto uut = nc::spsc_queue<int, n>{};
    for (auto i = 0; i < n; ++i)
    {
        EXPECT_TRUE(uut.push(value));
    }

    EXPECT_FALSE(uut.push(value));
}

TEST(SpscQueueTest, push_rValueOverload_handlesMoveOnlyType)
{
    constexpr auto value = 42;
    auto uut = nc::spsc_queue<std::unique_ptr<int>, 2>{};
    EXPECT_TRUE(uut.push(std::make_unique<int>(value)));

    auto actual = std::unique_ptr<int>{};
    EXPECT_TRUE(uut.pop(actual));
    EXPECT_EQ(*actual, value);
}

TEST(SpscQueueTest, push_indexWrapping)
{
    constexpr auto n = 2;
    auto uut = nc::spsc_queue<int, n>{};

    for (int round = 0; round < 5; ++round)
    {
        for (int i = 0; i < n; ++i)
            EXPECT_TRUE(uut.push(i + round * 10));

        EXPECT_FALSE(uut.push(999));

        for (int i = 0; i < n; ++i)
        {
            int out = -1;
            EXPECT_TRUE(uut.pop(out));
            EXPECT_EQ(out, i + round * 10);
        }

        EXPECT_TRUE(uut.empty());
    }
}

TEST(SpscQueueTest, pop_returnsTrueUntilEmpty)
{
    constexpr auto n = 4;
    auto uut = nc::spsc_queue<int, n>{};
    for (auto i = 0; i < n; ++i)
    {
        EXPECT_TRUE(uut.push(i));
    }

    auto value = 0;
    for (auto i = 0; i < n; ++i)
    {
        EXPECT_TRUE(uut.pop(value));
        EXPECT_EQ(value, i);
    }

    EXPECT_FALSE(uut.pop(value));
}

TEST(SpscQueueTest, empty_returnsExpectedValue)
{
    constexpr auto n = 4;
    auto uut = nc::spsc_queue<int, n>{};
    EXPECT_TRUE(uut.empty());

    int value = 42;
    EXPECT_FALSE(uut.pop(value));
    EXPECT_TRUE(uut.empty());

    EXPECT_TRUE(uut.push(value));
    EXPECT_FALSE(uut.empty());

    EXPECT_TRUE(uut.pop(value));
    EXPECT_TRUE(uut.empty());
}

TEST(SpscQueueTest, concurrencyStressTest)
{
    constexpr auto numValues = 2048;
    constexpr auto size = 8ull;
    auto uut = nc::spsc_queue<int, size>{};
    auto values = std::vector<int>{};
    auto start = std::atomic<bool>{false};
    auto numProduced = 0;
    auto numConsumed = 0;

    auto producer = std::thread([&uut, &start, &numProduced] () {
        while (!start.load(std::memory_order_acquire))
        {
            std::this_thread::yield();
        }

        while (numProduced < numValues)
        {
            if (uut.push(numProduced))
            {
                ++numProduced;
            }
        }
    });

    auto consumer = std::thread([&uut, &values, &start, &numConsumed]() {
        while (!start.load(std::memory_order_acquire))
        {
            std::this_thread::yield();
        }

        auto value = -1;
        while (numConsumed < numValues)
        {
            if (uut.pop(value))
            {
                values.push_back(value);
                ++numConsumed;
            }
        }
    });

    start.store(true);
    consumer.join();
    producer.join();

    EXPECT_EQ(numValues, numProduced);
    EXPECT_EQ(numValues, numConsumed);
    EXPECT_EQ(numValues, values.size());
    EXPECT_TRUE(std::ranges::equal(values, std::views::iota(0, numValues)));
}

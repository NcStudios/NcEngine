#include "ncengine/utility/SparseMap.h"
#include "gtest/gtest.h"

#include <ranges>
#include <string>

TEST(SparseMapTests, emplace_withinCapacity_addsValue)
{
    auto uut = nc::sparse_map<std::string>{10};
    auto& emplacedRValue = uut.emplace(5, "test");
    EXPECT_EQ("test", emplacedRValue);
    auto lValue = std::string{"test"};
    auto& emplacedLValue = uut.emplace(0, lValue);
    EXPECT_EQ(lValue, emplacedLValue);
}

TEST(SparseMapTests, emplace_exceedsCurrentCapacity_resizes)
{
    auto uut = nc::sparse_map<std::string>{10};
    EXPECT_GT(42, uut.size_keys());
    EXPECT_NO_THROW(uut.emplace(42, "test")); // expect no throw out of range
    EXPECT_LE(42, uut.size_keys());
}

TEST(SparseMapTests, emplace_exceedsMaxKeyCapacity_resizes)
{
    auto uut = nc::sparse_map<std::string>{10, 10};
    EXPECT_THROW(uut.emplace(42, "test"), nc::NcError);
}

TEST(SparseMapTests, emplace_alreadyExists_throws)
{
    auto uut = nc::sparse_map<std::string>{10};
    uut.emplace(1, "test");
    EXPECT_THROW(uut.emplace(1, "test"), nc::NcError);
}

TEST(SparseMapTests, contains_returnsExpectedState)
{
    auto uut = nc::sparse_map<std::string>{10};
    EXPECT_FALSE(uut.contains(1));
    uut.emplace(1, "test");
    EXPECT_TRUE(uut.contains(1));
    EXPECT_FALSE(uut.contains(500)); // exceeds capacity is fine
}

TEST(SparseMapTests, erase_exists_returnsTrueAndRemoves)
{
    auto uut = nc::sparse_map<std::string>{10};
    uut.emplace(1, "test");
    EXPECT_TRUE(uut.erase(1));
    EXPECT_FALSE(uut.contains(1));
}

TEST(SparseMapTests, erase_multiple_succeeds)
{
    auto uut = nc::sparse_map<std::string>{10};
    uut.emplace(1, "test1");
    uut.emplace(2, "test2");
    uut.emplace(3, "test3");
    uut.emplace(4, "test4");

    EXPECT_TRUE(uut.erase(2));
    EXPECT_TRUE(uut.erase(1));
    EXPECT_TRUE(uut.erase(4));
    EXPECT_TRUE(uut.erase(3));

    EXPECT_FALSE(uut.contains(1));
    EXPECT_FALSE(uut.contains(2));
    EXPECT_FALSE(uut.contains(3));
    EXPECT_FALSE(uut.contains(4));
}

TEST(SparseMapTests, erase_doesNotexist_returnsFalse)
{
    auto uut = nc::sparse_map<std::string>{10};
    EXPECT_FALSE(uut.erase(1));
}

TEST(SparseMapTests, reserve_onlyIncreasesDenseCapacity)
{
    auto uut = nc::sparse_map<std::string>{10};
    uut.reserve(50);
    EXPECT_EQ(50, uut.capacity());
    EXPECT_GT(50, uut.capacity_keys());
}

TEST(SparseMapTests, reserveKeys_onlyIncreasesSparseCapacity)
{
    auto uut = nc::sparse_map<std::string>{10};
    uut.reserve_keys(50);
    EXPECT_EQ(50, uut.capacity_keys());
    EXPECT_GT(50, uut.capacity());
}

TEST(SparseMapTests, clear_removesAllAndShrinksCapacity)
{
    auto uut = nc::sparse_map<std::string>{10};
    uut.emplace(4, "1");
    uut.emplace(2, "2");
    uut.emplace(0, "3");
    uut.clear();
    EXPECT_EQ(0, uut.size());
    EXPECT_EQ(0, uut.capacity());
    EXPECT_EQ(10, uut.size_keys()); // restores to initial capacity
    EXPECT_EQ(10, uut.capacity_keys());
    EXPECT_FALSE(uut.contains(4));
    EXPECT_FALSE(uut.contains(2));
    EXPECT_FALSE(uut.contains(0));
}

TEST(SparseMapTests, at_exists_returnsValue)
{
    auto uut = nc::sparse_map<std::string>{10};
    uut.emplace(9, "test");
    EXPECT_EQ("test", uut.at(9));
    const auto& constUut = uut;
    EXPECT_EQ("test", constUut.at(9));
}

TEST(SparseMapTests, at_doesNotExist_throws)
{
    auto uut = nc::sparse_map<std::string>{10};
    EXPECT_THROW(uut.at(0), std::exception);
}

TEST(SparseMapTests, empty_returnsExpectedValue)
{
    auto uut = nc::sparse_map<std::string>{10};
    EXPECT_TRUE(uut.empty());
    uut.emplace(0, "test");
    EXPECT_FALSE(uut.empty());
    uut.erase(0);
    EXPECT_TRUE(uut.empty());
}

TEST(SparseMapTests, size_returnsSize)
{
    auto uut = nc::sparse_map<std::string>{10};
    EXPECT_EQ(0, uut.size());
    EXPECT_EQ(10, uut.size_keys());
    uut.emplace(100, "test");
    EXPECT_EQ(1, uut.size());
    EXPECT_LE(100, uut.size_keys());
    uut.emplace(0, "test");
    EXPECT_EQ(2, uut.size());
    EXPECT_LE(100, uut.size_keys());
    uut.erase(0);
    EXPECT_EQ(1, uut.size());
    EXPECT_LE(100, uut.size_keys());
}

TEST(SparseMapTests, keysAndValues_returnsOrderAdded)
{
    auto uut = nc::sparse_map<std::string>{10};
    uut.emplace(25, "first");
    uut.emplace(0, "second");
    uut.emplace(12, "third");
    const auto expectedKeys = {25, 0, 12};
    const auto expectedValues = {std::string{"first"}, std::string{"second"}, std::string{"third"}};
    EXPECT_TRUE(std::ranges::equal(uut.keys(), expectedKeys));
    EXPECT_TRUE(std::ranges::equal(uut.values(), expectedValues));

    const auto& constUut = uut;
    EXPECT_TRUE(std::ranges::equal(constUut.keys(), expectedKeys));
    EXPECT_TRUE(std::ranges::equal(constUut.values(), expectedValues));
}

TEST(SparseMapTests, iterators)
{
    auto uut = nc::sparse_map<int>{10};
    uut.emplace(2, 1);
    uut.emplace(15, 42);
    uut.emplace(10, 43);
    uut.emplace(12, 22);
    auto it = uut.begin();
    EXPECT_EQ(1, *it++);
    EXPECT_EQ(42, *it++);
    EXPECT_EQ(43, *it++);
    EXPECT_EQ(22, *it++);
    EXPECT_EQ(it, uut.end());

    // works with algorithms/views
    const auto expectedValues = {1, 42, 43, 22};
    EXPECT_TRUE(std::ranges::equal(uut, expectedValues));

    const auto& constUut = uut;
    EXPECT_TRUE(std::ranges::equal(constUut, expectedValues));

    auto filteredValues = std::views::filter(uut, [](auto&& value) { return value % 2 == 0; });
    const auto expectedFilteredValues = {42, 22};
    EXPECT_TRUE(std::ranges::equal(expectedFilteredValues, filteredValues));
}

TEST(SparseMapTests, alternativeKey_succeeds)
{
    auto uut = nc::sparse_map<float, int8_t>{10};
    EXPECT_FALSE(uut.contains(0));
    uut.emplace(0, 1.0f);
    EXPECT_TRUE(uut.contains(0));
    EXPECT_FLOAT_EQ(1.0f, uut.at(0));
    EXPECT_TRUE(uut.erase(0));
}

#include "gtest/gtest.h"
#include "ncutility/Algorithm.h"

#include <array>
#include <functional>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

auto TransformIntFunction(int x) { return x * 2; }

struct TransformIntFunctor
{
    int operator()(int x) const { return x * 2; }
};

struct TestStruct
{
    int value;

    auto GetValue() const -> const int& { return value; }
};

auto TransformTestStructFunction(const TestStruct& s) -> const int& { return s.GetValue(); }

struct TransformTestStructFunctor
{
    auto operator()(const TestStruct& s) const -> const int& { return s.GetValue(); }
};

TEST(AlgorithmTests, Transform_compilationChecks)
{
    {
        const auto input = std::vector<int>{2, -3, 1};
        nc::algo::Transform(input, [](const auto& x) { return x * 2; });
        nc::algo::Transform(input, [](auto&& x) { return x * 2;});
        nc::algo::Transform(input, [](auto x) { return x * 2;});
        nc::algo::Transform(input, &TransformIntFunction);
        nc::algo::Transform(input, TransformIntFunctor{});
        nc::algo::Transform(input, std::function{[](int x) { return x * 2; }});
    }

    {
        const auto input = std::vector<TestStruct>{TestStruct{1}, TestStruct{2}, TestStruct{3}};
        nc::algo::Transform(input, [](const auto& s) { return s.GetValue(); });
        nc::algo::Transform(input, [](auto&& s) { return s.GetValue(); });
        nc::algo::Transform(input, [](auto s) { return s.GetValue(); });
        nc::algo::Transform(input, &TransformTestStructFunction);
        nc::algo::Transform(input, TransformTestStructFunctor{});
        nc::algo::Transform(input, std::function{[](const TestStruct& s) { return s.GetValue(); }});
    }

    {
        const auto input = std::vector<std::string>{"abc", "def", "ghi"};
        nc::algo::Transform(input, [](const auto& str) { return str.at(0); });
        nc::algo::Transform(input, [](auto&& str) { return str.at(0); });
        nc::algo::Transform(input, [](auto str) { return str.at(0); });
        nc::algo::Transform(input, std::function{[](const std::string& str) { return str.at(0); }});
    }
}

TEST(AlgorithmTests, Transform_emptyInput)
{
    const auto input = std::vector<int>{};
    const auto actual = nc::algo::Transform(input, [](const auto& x) { return x * 2; });
    ASSERT_TRUE(actual.empty());
}

TEST(AlgorithmTests, Transform_identityTransformation)
{
    const auto expected = std::vector<int>{2, -3, 1};
    const auto actual = nc::algo::Transform(expected, [](const auto& x) { return x; });
    ASSERT_EQ(expected.size(), actual.size());
    EXPECT_TRUE(std::ranges::equal(expected, actual));
}

TEST(AlgorithmTests, Transform_sameTypeTransformation)
{
    auto input = std::vector<int>{2, -3, 1};
    const auto actual = nc::algo::Transform(input, [](const auto& x) { return x * 2; });
    const auto expected = std::vector<int>{4, -6, 2};
    ASSERT_EQ(expected.size(), actual.size());
    EXPECT_TRUE(std::ranges::equal(expected, actual));
}

TEST(AlgorithmTests, Transform_typeChangingTransformation)
{
    const auto input = std::vector<std::string>{"abc", "def", "ghi"};
    const auto actual = nc::algo::Transform(input, [](const auto& str) { return str.at(0); });
    const auto expected = std::vector<char>{'a', 'd', 'g'};
    ASSERT_EQ(expected.size(), actual.size());
    EXPECT_TRUE(std::ranges::equal(expected, actual));
}

TEST(AlgorithmTests, Enumerate_standardContainers_returnsCorrectIndices)
{
    for (auto [index, value] : nc::algo::Enumerate(std::vector<int>{0, 1, 2}))
    {
        EXPECT_EQ(index, value);
    }

    for (auto [index, value] : nc::algo::Enumerate(std::array<int, 3>{0, 1, 2}))
    {
        EXPECT_EQ(index, value);
    }

    constexpr auto input = std::array<int, 3>{0, 1, 2};
    for (auto [index, value] : nc::algo::Enumerate(std::span{input}))
    {
        EXPECT_EQ(index, value);
    }

    const auto map = std::unordered_map<int, std::string>{{0, "a"}, {1, "b"}, {2, "c"}};
    for (const auto [index, pair] : nc::algo::Enumerate(map))
    {
        const auto& [key, value] = pair;
        EXPECT_TRUE(index == 0 || index == 1 || index == 2);
        EXPECT_EQ(map.at(key), value);
    }
}

TEST(AlgorithmTests, Enumerate_nonConstRange_canModifyValues)
{
    auto input = std::vector<int>{2, 2, 2};
    for (auto [index, value] : nc::algo::Enumerate(input))
    {
        value = 3;
    }

    EXPECT_TRUE(std::ranges::equal(input, std::array<int, 3>{3, 3, 3}));
}

// TODO: This test seems to have issues due to experimental ranges support in clang
// TEST(AlgorithmTests, Enumerate_composes)
// {
//     const auto input = std::vector<int>{2, 2, 2, 4, 4, 4};
//     auto currentIndex = 0;
//     for (const auto [index, value] : input | nc::algo::Enumerate | std::views::take(3))
//     {
//         EXPECT_EQ(index, currentIndex++);
//         EXPECT_EQ(value, 2);
//     }
// }

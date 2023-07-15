#include "gtest/gtest.h"
#include "serialize/BinarySerialization.h"

#include <algorithm>

struct AggregateTestType
{
    struct SingleMember
    {
        int m1;
        auto operator<=>(const SingleMember&) const = default;
    };

    struct NonTrivialMember
    {
        SingleMember m1;
        std::string m2;
        auto operator<=>(const NonTrivialMember&) const = default;
    };

    struct CollectionMember
    {
        std::vector<SingleMember> m1;
        auto operator<=>(const CollectionMember&) const = default;
    };

    SingleMember m1;
    NonTrivialMember m2;
    CollectionMember m3;
    int m4, m5, m6, m7, m8, m9, m10; // give it max members
    auto operator<=>(const AggregateTestType&) const = default;
};

static_assert(std::is_aggregate_v<AggregateTestType>);
static_assert(!std::is_trivially_copyable_v<AggregateTestType>);

class NonAggregateTestType
{
    public:
        NonAggregateTestType(std::string x_, int y_)
            : x{x_}, y{y_}
        {
        }

        auto X() -> std::string&{ return x; }
        auto X() const -> const std::string& { return x;}
        auto Y() -> int& { return y; }
        auto Y() const -> const int& { return y; }

    private:
        std::string x;
        int y;
};

static_assert(!std::is_aggregate_v<NonAggregateTestType>);
static_assert(!std::is_trivially_copyable_v<NonAggregateTestType>);

namespace nc::serialize
{
void Serialize(nc::serialize::BinaryBuffer& buffer, const NonAggregateTestType& in)
{
    Serialize(buffer, in.X());
    Serialize(buffer, in.Y());
}

void Deserialize(nc::serialize::BinaryBuffer& buffer, NonAggregateTestType& in)
{
    Deserialize(buffer, in.X());
    Deserialize(buffer, in.Y());
}
} // namespace nc::serialize

TEST(BinaryBuffer, Foo2)
{
    static_assert(nc::type::TriviallyCopyable<int>);
    static_assert(!nc::type::TriviallyCopyable<AggregateTestType>);

}

TEST(BinarySerialization, Primitives_roundTrip_preservesValues)
{
    auto buffer = nc::serialize::BinaryBuffer{};

    const auto expectedInt = 42;
    const auto expectedFloat = 3.14f;
    Serialize(buffer, expectedInt);
    Serialize(buffer, expectedFloat);

    auto actualInt = 0;
    auto actualFloat = 0.0f;
    Deserialize(buffer, actualInt);
    Deserialize(buffer, actualFloat);

    EXPECT_EQ(expectedInt, actualInt);
    EXPECT_EQ(expectedFloat, actualFloat);
}

TEST(BinarySerialization, StdTypes_roundTrip_preservesValues)
{
    auto buffer = nc::serialize::BinaryBuffer{};

    const auto expectedString = std::string{"a test string"};
    const auto expectedTrivialVector = std::vector<int>{1, 2, 3};
    const auto expectedNonTrivialVector = std::vector<std::string>{"one", "two", "three"};
    const auto expectedTrivialArray = std::array<size_t, 2>{5, 9};
    const auto expectedNonTrivialArray = std::array<AggregateTestType::NonTrivialMember, 1>{{50, "test"}};
    Serialize(buffer, expectedString);
    Serialize(buffer, expectedTrivialVector);
    Serialize(buffer, expectedNonTrivialVector);
    Serialize(buffer, expectedTrivialArray);
    Serialize(buffer, expectedNonTrivialArray);

    auto actualString = std::string{};
    auto actualTrivialVector = std::vector<int>{};
    auto actualNonTrivialVector = std::vector<std::string>{};
    auto actualTrivialArray = std::array<size_t, 2>{};
    auto actualNonTrivialArray = std::array<AggregateTestType::NonTrivialMember, 1>{};
    Deserialize(buffer, actualString);
    Deserialize(buffer, actualTrivialVector);
    Deserialize(buffer, actualNonTrivialVector);
    Deserialize(buffer, actualTrivialArray);
    Deserialize(buffer, actualNonTrivialArray);

    EXPECT_EQ(expectedString, actualString);
    EXPECT_TRUE(std::ranges::equal(expectedTrivialVector, actualTrivialVector));
    EXPECT_TRUE(std::ranges::equal(expectedNonTrivialVector, actualNonTrivialVector));
    EXPECT_TRUE(std::ranges::equal(expectedTrivialArray, actualTrivialArray));
    EXPECT_TRUE(std::ranges::equal(expectedNonTrivialArray, actualNonTrivialArray));
}

TEST(BinarySerialization, AggregateTypes_roundTrip_preservesValues)
{
    auto buffer = nc::serialize::BinaryBuffer{};
    const auto expected = AggregateTestType
    {
        42,
        {59, "sample"},
        { {{1}, {2}, {3}} },
        4, 5, 6, 7, 8, 9, 10
    };

    Serialize(buffer, expected);
    auto actual = AggregateTestType{};
    Deserialize(buffer, actual);

    EXPECT_EQ(expected, actual);
}

TEST(BinarySerialization, NonAggregateTypes_roundTrip_preservesValues)
{
    auto buffer = nc::serialize::BinaryBuffer{};
    auto expected = NonAggregateTestType{"1", 2};
    Serialize(buffer, expected);
    auto actual = NonAggregateTestType{"", 0};
    Deserialize(buffer, actual);

    EXPECT_EQ(expected.X(), actual.X());
    EXPECT_EQ(expected.Y(), actual.Y());
}

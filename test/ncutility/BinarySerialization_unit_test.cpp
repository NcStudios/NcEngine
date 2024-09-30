#include "gtest/gtest.h"
#include "ncutility/BinarySerialization.h"

#include <algorithm>
#include <sstream>

namespace test
{
// Type expected to be automatically serializable via aggregate unpacking. Inner types exercise
// a variety of unpacking/serialization scenarios.
struct Aggregate
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
    int m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16; // give it max members

    auto operator<=>(const Aggregate&) const = default;
};

static_assert(std::is_aggregate_v<Aggregate>);
static_assert(!std::is_trivially_copyable_v<Aggregate>);
static_assert(nc::serialize::binary::MemberCount<Aggregate>() <= nc::serialize::g_aggregateMaxMemberCount);
static_assert(nc::serialize::cpo::HasSerializeDefault<Aggregate>);
static_assert(nc::serialize::cpo::HasDeserializeDefault<Aggregate>);

// Type non-memcpyable and with too many members to be unpacked - requires user-provided overloads.
struct BigAggregate
{
    int m1, m2,  m3,  m4,  m5,  m6,  m7,  m8,
        m9, m10, m11, m12, m13, m14, m15, m16;

    std::string m17;

    auto operator<=>(const BigAggregate&) const = default;
};

void Serialize(std::ostream& stream, const test::BigAggregate& in)
{
    nc::serialize::binary::SerializeMultiple(
        stream, in.m1, in.m2, in.m3, in.m4, in.m5, in.m6, in.m7, in.m8,
        in.m9, in.m10, in.m11, in.m12, in.m13, in.m14, in.m15, in.m16, in.m17
    );
}

void Deserialize(std::istream& stream, test::BigAggregate& in)
{
    nc::serialize::binary::DeserializeMultiple(
        stream, in.m1, in.m2, in.m3, in.m4, in.m5, in.m6, in.m7, in.m8,
        in.m9, in.m10, in.m11, in.m12, in.m13, in.m14, in.m15, in.m16, in.m17
    );
}

static_assert(std::is_aggregate_v<BigAggregate>);
static_assert(!std::is_trivially_copyable_v<BigAggregate>);
static_assert(nc::serialize::binary::MemberCount<BigAggregate>() > nc::serialize::g_aggregateMaxMemberCount);
static_assert(nc::serialize::cpo::HasSerializeAdl<BigAggregate>);
static_assert(nc::serialize::cpo::HasDeserializeAdl<BigAggregate>);

// Type which can't be memcpy'd or unpacked - requies user-provided overload for serialization.
class NonAggregate
{
    public:
        NonAggregate(std::string x_, int y_)
            : x{x_}, y{y_} {}

        auto X() -> std::string&{ return x; }
        auto X() const -> const std::string& { return x;}
        auto Y() -> int& { return y; }
        auto Y() const -> const int& { return y; }

    private:
        std::string x;
        int y;
};

void Serialize(std::ostream& stream, const test::NonAggregate& in)
{
    nc::serialize::binary::SerializeMultiple(stream, in.X(), in.Y());
}

void Deserialize(std::istream& stream, test::NonAggregate& out)
{
    nc::serialize::binary::DeserializeMultiple(stream, out.X(), out.Y());
}

static_assert(!std::is_aggregate_v<NonAggregate>);
static_assert(!std::is_trivially_copyable_v<NonAggregate>);
static_assert(nc::serialize::cpo::HasSerializeAdl<NonAggregate>);
static_assert(nc::serialize::cpo::HasDeserializeAdl<NonAggregate>);

// Type which is automatically serializable, but provides a member function overload
struct HasMemberFunc
{
    HasMemberFunc() = default;

    HasMemberFunc(int v)
        : value{v}{}

    int value = 0;

    // breadcrumbs to prove members were invoked
    mutable bool invokedSerialize = false;
    mutable bool invokedDeserialize = false;

    void Serialize(std::ostream& stream) const
    {
        invokedSerialize = true;
        nc::serialize::binary::Serialize(stream, value);
    }

    void Deserialize(std::istream& stream)
    {
        invokedDeserialize = true;
        nc::serialize::binary::Deserialize(stream, value);
    }
};

static_assert(nc::serialize::cpo::HasSerializeDefault<HasMemberFunc>);
static_assert(nc::serialize::cpo::HasDeserializeMember<HasMemberFunc>);
} // namespace test

TEST(BinarySerializationTest, Serialize_primitives_preservedRoundTrip)
{
    auto stream = std::stringstream{};
    const auto expectedInt = 42;
    const auto expectedFloat = 3.14f;
    auto actualInt = 0;
    auto actualFloat = 0.0f;
    nc::serialize::Serialize(stream, expectedInt);
    nc::serialize::Serialize(stream, expectedFloat);
    nc::serialize::Deserialize(stream, actualInt);
    nc::serialize::Deserialize(stream, actualFloat);
    EXPECT_EQ(expectedInt, actualInt);
    EXPECT_EQ(expectedFloat, actualFloat);
}

TEST(BinarySerializationTest, Serialize_string_preservedRoundTrip)
{
    auto stream = std::stringstream{};
    const auto smallString = std::string{"a test string"};
    const auto bigString = std::string('a', 32);
    const auto emptyString = std::string{};
    auto actualSmallString = std::string{};
    auto actualBigString = std::string{};
    auto actualEmptyString = std::string{};
    nc::serialize::Serialize(stream, smallString);
    nc::serialize::Serialize(stream, bigString);
    nc::serialize::Serialize(stream, emptyString);
    nc::serialize::Deserialize(stream, actualSmallString);
    nc::serialize::Deserialize(stream, actualBigString);
    nc::serialize::Deserialize(stream, actualEmptyString);
    EXPECT_EQ(smallString, actualSmallString);
    EXPECT_EQ(bigString, actualBigString);
    EXPECT_EQ(emptyString, actualEmptyString);
}

TEST(BinarySerializationTest, Serialize_array_preservedRoundTrip)
{
    auto stream = std::stringstream{};
    const auto trivialArray = std::array<size_t, 2>{5, 9};
    const auto nonTrivialArray = std::array<std::string, 2>{"test1", "test2"};
    auto actualTrivialArray = std::array<size_t, 2>{};
    auto actualNonTrivialArray = std::array<std::string, 2>{};
    nc::serialize::Serialize(stream, trivialArray);
    nc::serialize::Serialize(stream, nonTrivialArray);
    nc::serialize::Deserialize(stream, actualTrivialArray);
    nc::serialize::Deserialize(stream, actualNonTrivialArray);
    EXPECT_TRUE(std::ranges::equal(trivialArray, actualTrivialArray));
    EXPECT_TRUE(std::ranges::equal(nonTrivialArray, actualNonTrivialArray));
}

TEST(BinarySerializationTest, Serialize_vector_preservedRoundTrip)
{
    auto stream = std::stringstream{};
    const auto trivialVector = std::vector<int>{1, 2, 3};
    const auto nonTrivialVector = std::vector<std::string>{"one", "two", "three"};
    const auto emptyVector = std::vector<int>{};
    auto actualTrivialVector = std::vector<int>{};
    auto actualNonTrivialVector = std::vector<std::string>{};
    auto actualEmptyVector = std::vector<int>{};
    nc::serialize::Serialize(stream, trivialVector);
    nc::serialize::Serialize(stream, nonTrivialVector);
    nc::serialize::Serialize(stream, trivialVector);
    nc::serialize::Deserialize(stream, actualTrivialVector);
    nc::serialize::Deserialize(stream, actualNonTrivialVector);
    EXPECT_TRUE(std::ranges::equal(trivialVector, actualTrivialVector));
    EXPECT_TRUE(std::ranges::equal(nonTrivialVector, actualNonTrivialVector));
    EXPECT_TRUE(std::ranges::equal(emptyVector, actualEmptyVector));
}

TEST(BinarySerializationTest, Serialize_map_preservedRoundTrip)
{
    auto stream = std::stringstream{};
    const auto nonEmptyMap = std::unordered_map<std::string, size_t>{{std::string{"test"}, 32}};
    const auto emptyMap = std::unordered_map<std::string, size_t>{};
    auto actualNonEmptyMap = std::unordered_map<std::string, size_t>{};
    auto actualEmptyMap = std::unordered_map<std::string, size_t>{};
    nc::serialize::Serialize(stream, nonEmptyMap);
    nc::serialize::Serialize(stream, emptyMap);
    nc::serialize::Deserialize(stream, actualNonEmptyMap);
    nc::serialize::Deserialize(stream, actualEmptyMap);
    EXPECT_TRUE(std::ranges::equal(nonEmptyMap, actualNonEmptyMap));
    EXPECT_TRUE(std::ranges::equal(emptyMap, actualEmptyMap));

}

TEST(BinarySerializationTest, Serialize_optional_preservedRoundTrip)
{
    auto stream = std::stringstream{};
    const auto nonEmptyOptional = std::make_optional<test::BigAggregate>(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, "test");
    const auto emptyOptional = std::optional<std::string>{};
    auto actualNonEmptyOptional = std::optional<test::BigAggregate>{};
    auto actualEmptyOptional = std::optional<std::string>{};
    nc::serialize::Serialize(stream, nonEmptyOptional);
    nc::serialize::Serialize(stream, emptyOptional);
    nc::serialize::Deserialize(stream, actualNonEmptyOptional);
    nc::serialize::Deserialize(stream, actualEmptyOptional);
    EXPECT_EQ(nonEmptyOptional, actualNonEmptyOptional);
    EXPECT_EQ(emptyOptional, actualEmptyOptional);
}

TEST(BinarySerializationTest, Serialize_aggregate_preservedRoundTrip)
{
    auto stream = std::stringstream{};
    auto actual = test::Aggregate{};
    const auto expected = test::Aggregate
    {
        42,
        {59, "sample"},
        { {{1}, {2}, {3}} },
        4, 5, 6, 7, 8, 9, 10
    };

    nc::serialize::Serialize(stream, expected);
    nc::serialize::Deserialize(stream, actual);
    EXPECT_EQ(expected, actual);
}

TEST(BinarySerializationTest, Serialize_aggregateWithCustomOverloads_preservedRoundTrip)
{
    auto stream = std::stringstream{};
    const auto expected = test::BigAggregate{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, "test"};
    auto actual = test::BigAggregate{};
    nc::serialize::Serialize(stream, expected);
    nc::serialize::Deserialize(stream, actual);
    EXPECT_EQ(expected, actual);
}

TEST(BinarySerializationTest, Serialize_nonAggregate_preservedRoundTrip)
{
    auto stream = std::stringstream{};
    auto expected = test::NonAggregate{"1", 2};
    auto actual = test::NonAggregate{"", 0};
    nc::serialize::Serialize(stream, expected);
    nc::serialize::Deserialize(stream, actual);
    EXPECT_EQ(expected.X(), actual.X());
    EXPECT_EQ(expected.Y(), actual.Y());
}

TEST(BinarySerializationTest, Serialize_memberFunc_preservedRoundTrip)
{
    auto stream = std::stringstream{};
    const auto expected  = test::HasMemberFunc{42};
    auto actual = test::HasMemberFunc{};
    nc::serialize::Serialize(stream, expected);
    nc::serialize::Deserialize(stream, actual);
    EXPECT_EQ(expected.value, actual.value);
    EXPECT_TRUE(expected.invokedSerialize); // expect went through member func, not default serialization
    EXPECT_TRUE(actual.invokedDeserialize);
}

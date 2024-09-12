#include "gtest/gtest.h"
#include "ecs/Entity.h"

using namespace nc;

TEST(Entity_unit_tests, Null_ReturnsNullHandle)
{
    auto handle = Entity::Null();
    auto actual = static_cast<Entity::index_type>(handle);
    auto expected = Entity::NullIndex;
    EXPECT_EQ(actual, expected);
}

TEST(Entity_unit_tests, Valid_NullHandle_ReturnsFalse)
{
    auto handle = Entity::Null();
    auto actual = handle.Valid();
    EXPECT_FALSE(actual);
}

TEST(Entity_unit_tests, Valid_NonNullHandle_ReturnsTrue)
{
    auto handle = Entity{0u, 0u, Entity::Flags::None};
    auto actual = handle.Valid();
    EXPECT_TRUE(actual);
}

TEST(Entity_unit_tests, Equality_SameHandles_ReturnsTrue)
{
    auto h1 = Entity{0u, 0u, Entity::Flags::None};
    auto h2 = h1;
    EXPECT_EQ(h1, h2);
}

TEST(Entity_unit_tests, Equality_DifferentHandles_ReturnsFalse)
{
    auto h1 = Entity{0u, 0u, Entity::Flags::None};
    auto h2 = Entity{1u, 0u, Entity::Flags::None};
    EXPECT_NE(h1, h2);
}

TEST(Entity_unit_tests, Index_ExtractsIndex)
{
    Entity::index_type expected = 5u;
    auto handle = Entity{expected, 2u, 3u};
    Entity::index_type actual = handle.Index();
    EXPECT_EQ(actual, expected);
}

TEST(Entity_unit_tests, Layer_ExtractsLayer)
{
    Entity::layer_type expected = 2u;
    auto handle = Entity{3u, expected, 3u};
    Entity::layer_type actual = handle.Layer();
    EXPECT_EQ(actual, expected);
}

TEST(Entity_unit_tests, Flags_ExtractsFlags)
{
    Entity::flags_type expected = 2u;
    auto handle = Entity{0u, 1u, expected};
    Entity::flags_type actual = handle.Flags();
    EXPECT_EQ(actual, expected);
}

TEST(Entity_unit_tests, IsStatic_FlagSet_ReturnsTrue)
{
    auto flags = Entity::Flags::Static;
    auto handle = Entity{1u, 1u, flags};
    EXPECT_TRUE(handle.IsStatic());
}

TEST(Entity_unit_tests, IsStatic_FlagNotSet_ReturnsFalse)
{
    auto flags = Entity::Flags::None;
    auto handle = Entity{1u, 1u, flags};
    EXPECT_FALSE(handle.IsStatic());
}

TEST(Entity_unit_tests, IsPersistent_FlagSet_ReturnsTrue)
{
    auto flags = Entity::Flags::Persistent;
    auto handle = Entity{1u, 1u, flags};
    EXPECT_TRUE(handle.IsPersistent());
}

TEST(Entity_unit_tests, IsPersistent_FlagNotSet_ReturnsFalse)
{
    auto flags = Entity::Flags::None;
    auto handle = Entity{1u, 1u, flags};
    EXPECT_FALSE(handle.IsPersistent());
}

TEST(Entity_unit_tests, IsSerializable_NoSerializeNotSet_ReturnsTrue)
{
    auto flags = Entity::Flags::None;
    auto handle = Entity{1u, 1u, flags};
    EXPECT_TRUE(handle.IsSerializable());
}

TEST(Entity_unit_tests, IsSerializable_NoSerializeSet_ReturnsFalse)
{
    auto flags = Entity::Flags::NoSerialize;
    auto handle = Entity{1u, 1u, flags};
    EXPECT_FALSE(handle.IsSerializable());
}

TEST(Entity_unit_tests, IsInternal_FlagSet_ReturnsTrue)
{
    auto flags = Entity::Flags::Internal;
    auto handle = Entity{1u, 1u, flags};
    EXPECT_TRUE(handle.IsInternal());
}

TEST(Entity_unit_tests, IsInternal_FlagNotSet_ReturnsFalse)
{
    auto flags = Entity::Flags::None;
    auto handle = Entity{1u, 1u, flags};
    EXPECT_FALSE(handle.IsInternal());
}

TEST(Entity_unit_tests, Hash_AllValues_ReturnsExpectedHash)
{
    constexpr auto handle = Entity{uint32_t{0x01234567}, uint8_t{0x89}, uint8_t{0xAB}};
    constexpr auto uut = Entity::Hash();
    constexpr auto actual = uut(handle);
    constexpr auto expected = size_t{0x00000123456789AB}; // top 4 bytes empty | index | layer | flags
    EXPECT_EQ(expected, actual);
}

TEST(Entity_unit_tests, FromHash_NoFlags_ReconstructsEntity)
{
    constexpr auto expectedEntity = Entity{42u, 0u, 0u};
    constexpr auto hash = Entity::Hash{}(expectedEntity);
    constexpr auto actualEntity = Entity::FromHash(hash);
    EXPECT_EQ(expectedEntity, actualEntity);
}

TEST(Entity_unit_tests, FromHash_AllValues_ReconstructsEntity)
{
    constexpr auto expectedEntity = Entity{128u, 7u, nc::Entity::Flags::Static | nc::Entity::Flags::NoSerialize};
    constexpr auto hash = Entity::Hash{}(expectedEntity);
    constexpr auto actualEntity = Entity::FromHash(hash);
    EXPECT_EQ(expectedEntity, actualEntity);
}

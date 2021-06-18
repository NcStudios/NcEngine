#include "gtest/gtest.h"
#include "Entity.h"

using namespace nc;

TEST(Entity_unit_tests, Null_ReturnsNullHandle)
{
    auto handle = Entity::Null();
    auto actual = static_cast<EntityTraits::underlying_type>(handle);
    auto expected = EntityTraits::NullHandle;
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
    auto handle = Entity{0u};
    auto actual = handle.Valid();
    EXPECT_TRUE(actual);
}

TEST(Entity_unit_tests, Equality_SameHandles_ReturnsTrue)
{
    auto h1 = Entity{0u};
    auto h2 = h1;
    EXPECT_EQ(h1, h2);
}

TEST(Entity_unit_tests, Equality_DifferentHandles_ReturnsFalse)
{
    auto h1 = Entity{0u};
    auto h2 = Entity{1u};
    EXPECT_NE(h1, h2);
}

TEST(Entity_unit_tests, Join_ReturnsCorrectBitset)
{
    EntityTraits::index_type index = 1u;
    EntityTraits::version_type version = 0u;
    EntityTraits::layer_type layer = 2u;
    EntityTraits::flags_type flags = 1u;
    EntityTraits::underlying_type i_shift = static_cast<EntityTraits::underlying_type>(index);
    EntityTraits::underlying_type v_shift = static_cast<EntityTraits::underlying_type>(version) << EntityTraits::VersionShift;
    EntityTraits::underlying_type l_shift = static_cast<EntityTraits::underlying_type>(layer) << EntityTraits::LayerShift;
    EntityTraits::underlying_type f_shift = static_cast<EntityTraits::underlying_type>(flags) << EntityTraits::FlagsShift;
    EntityTraits::underlying_type expected = i_shift | v_shift | l_shift | f_shift;
    EntityTraits::underlying_type actual = EntityUtils::Join(index, version, layer, flags);
    EXPECT_EQ(actual, expected);
}

TEST(Entity_unit_tests, Index_ExtractsIndex)
{
    EntityTraits::index_type expected = 5u;
    auto handle = Entity{EntityUtils::Join(expected, 1u, 2u, 3u)};
    EntityTraits::index_type actual = EntityUtils::Index(handle);
    EXPECT_EQ(actual, expected);
}

TEST(Entity_unit_tests, Version_ExtractsVersion)
{
    EntityTraits::version_type expected = 3u;
    auto handle = Entity{EntityUtils::Join(1u, expected, 2u, 3u)};
    EntityTraits::version_type actual = EntityUtils::Version(handle);
    EXPECT_EQ(actual, expected);
}

TEST(Entity_unit_tests, Layer_ExtractsLayer)
{
    EntityTraits::layer_type expected = 2u;
    auto handle = Entity{EntityUtils::Join(3u, 1u, expected, 3u)};
    EntityTraits::layer_type actual = EntityUtils::Layer(handle);
    EXPECT_EQ(actual, expected);
}

TEST(Entity_unit_tests, Flags_ExtractsFlags)
{
    EntityTraits::flags_type expected = 2u;
    auto handle = Entity{EntityUtils::Join(0u, 1u, 2u, expected)};
    EntityTraits::flags_type actual = EntityUtils::Flags(handle);
    EXPECT_EQ(actual, expected);
}

TEST(Entity_unit_tests, IsStatic_FlagSet_ReturnsTrue)
{
    auto flags = EntityFlags::Static;
    auto handle = Entity{EntityUtils::Join(1u, 1u, 1u, flags)};
    EXPECT_TRUE(EntityUtils::IsStatic(handle));
}

TEST(Entity_unit_tests, IsStatic_FlagNotSet_ReturnsFalse)
{
    auto flags = EntityFlags::None;
    auto handle = Entity{EntityUtils::Join(1u, 1u, 1u, flags)};
    EXPECT_FALSE(EntityUtils::IsStatic(handle));
}

TEST(Entity_unit_tests, SetVersion_OnlyVersionChanged)
{
    EntityTraits::index_type index = 1u;
    EntityTraits::version_type version = 8u;
    EntityTraits::layer_type layer = 3u;
    EntityTraits::flags_type flags = 2u;
    auto handle = Entity{EntityUtils::Join(index, version, layer, flags)};
    EntityTraits::version_type newVersion = 1u;
    auto updatedHandle = EntityUtils::SetVersion(handle, newVersion);
    EXPECT_EQ(index, EntityUtils::Index(updatedHandle));
    EXPECT_EQ(newVersion, EntityUtils::Version(updatedHandle));
    EXPECT_EQ(layer, EntityUtils::Layer(updatedHandle));
    EXPECT_EQ(flags, EntityUtils::Flags(updatedHandle));
}

TEST(Entity_unit_tests, Recyle_ValuesUpdated)
{
    EntityTraits::index_type index = 0u;
    EntityTraits::version_type version = 2u;
    EntityTraits::layer_type layer = 0u;
    EntityTraits::flags_type flags = 1u;
    auto handle = Entity{EntityUtils::Join(index, version, layer, flags)};
    EntityTraits::layer_type newLayer = 1u;
    EntityTraits::flags_type newFlags = 0u;
    auto updatedHandle = EntityUtils::Recycle(handle, newLayer, newFlags);
    EXPECT_EQ(index, EntityUtils::Index(updatedHandle));
    EXPECT_EQ(version + 1u, EntityUtils::Version(updatedHandle));
    EXPECT_EQ(newLayer, EntityUtils::Layer(updatedHandle));
    EXPECT_EQ(newFlags, EntityUtils::Flags(updatedHandle));
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
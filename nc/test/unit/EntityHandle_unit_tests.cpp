#include "gtest/gtest.h"
#include "entity/HandleUtils.h"

using namespace nc;

TEST(EntityHandle_unit_tests, Null_ReturnsNullHandle)
{
    auto handle = EntityHandle::Null();
    auto actual = static_cast<HandleTraits::handle_type>(handle);
    auto expected = HandleTraits::NullHandle;
    EXPECT_EQ(actual, expected);
}

TEST(EntityHandle_unit_tests, Valid_NullHandle_ReturnsFalse)
{
    auto handle = EntityHandle::Null();
    auto actual = handle.Valid();
    EXPECT_FALSE(actual);
}

TEST(EntityHandle_unit_tests, Valid_NonNullHandle_ReturnsTrue)
{
    auto handle = EntityHandle{0u};
    auto actual = handle.Valid();
    EXPECT_TRUE(actual);
}

TEST(EntityHandle_unit_tests, Equality_SameHandles_ReturnsTrue)
{
    auto h1 = EntityHandle{0u};
    auto h2 = h1;
    EXPECT_EQ(h1, h2);
}

TEST(EntityHandle_unit_tests, Equality_DifferentHandles_ReturnsFalse)
{
    auto h1 = EntityHandle{0u};
    auto h2 = EntityHandle{1u};
    EXPECT_NE(h1, h2);
}

TEST(EntityHandle_unit_tests, Join_ReturnsCorrectBitset)
{
    HandleTraits::index_type index = 1u;
    HandleTraits::version_type version = 0u;
    HandleTraits::layer_type layer = 2u;
    HandleTraits::flags_type flags = 1u;
    HandleTraits::handle_type i_shift = static_cast<HandleTraits::handle_type>(index);
    HandleTraits::handle_type v_shift = static_cast<HandleTraits::handle_type>(version) << HandleTraits::VersionShift;
    HandleTraits::handle_type l_shift = static_cast<HandleTraits::handle_type>(layer) << HandleTraits::LayerShift;
    HandleTraits::handle_type f_shift = static_cast<HandleTraits::handle_type>(flags) << HandleTraits::FlagsShift;
    HandleTraits::handle_type expected = i_shift | v_shift | l_shift | f_shift;
    HandleTraits::handle_type actual = HandleUtils::Join(index, version, layer, flags);
    EXPECT_EQ(actual, expected);
}

TEST(EntityHandle_unit_tests, Index_ExtractsIndex)
{
    HandleTraits::index_type expected = 5u;
    auto handle = EntityHandle{HandleUtils::Join(expected, 1u, 2u, 3u)};
    HandleTraits::index_type actual = HandleUtils::Index(handle);
    EXPECT_EQ(actual, expected);
}

TEST(EntityHandle_unit_tests, Version_ExtractsVersion)
{
    HandleTraits::version_type expected = 3u;
    auto handle = EntityHandle{HandleUtils::Join(1u, expected, 2u, 3u)};
    HandleTraits::version_type actual = HandleUtils::Version(handle);
    EXPECT_EQ(actual, expected);
}

TEST(EntityHandle_unit_tests, Layer_ExtractsLayer)
{
    HandleTraits::layer_type expected = 2u;
    auto handle = EntityHandle{HandleUtils::Join(3u, 1u, expected, 3u)};
    HandleTraits::layer_type actual = HandleUtils::Layer(handle);
    EXPECT_EQ(actual, expected);
}

TEST(EntityHandle_unit_tests, Flags_ExtractsFlags)
{
    HandleTraits::flags_type expected = 2u;
    auto handle = EntityHandle{HandleUtils::Join(0u, 1u, 2u, expected)};
    HandleTraits::flags_type actual = HandleUtils::Flags(handle);
    EXPECT_EQ(actual, expected);
}

TEST(EntityHandle_unit_tests, IsStatic_FlagSet_ReturnsTrue)
{
    auto flags = EntityFlags::Static;
    auto handle = EntityHandle{HandleUtils::Join(1u, 1u, 1u, flags)};
    EXPECT_TRUE(HandleUtils::IsStatic(handle));
}

TEST(EntityHandle_unit_tests, IsStatic_FlagNotSet_ReturnsFalse)
{
    auto flags = EntityFlags::None;
    auto handle = EntityHandle{HandleUtils::Join(1u, 1u, 1u, flags)};
    EXPECT_FALSE(HandleUtils::IsStatic(handle));
}

TEST(EntityHandle_unit_tests, SetVersion_OnlyVersionChanged)
{
    HandleTraits::index_type index = 1u;
    HandleTraits::version_type version = 8u;
    HandleTraits::layer_type layer = 3u;
    HandleTraits::flags_type flags = 2u;
    auto handle = EntityHandle{HandleUtils::Join(index, version, layer, flags)};
    HandleTraits::version_type newVersion = 1u;
    auto updatedHandle = HandleUtils::SetVersion(handle, newVersion);
    EXPECT_EQ(index, HandleUtils::Index(updatedHandle));
    EXPECT_EQ(newVersion, HandleUtils::Version(updatedHandle));
    EXPECT_EQ(layer, HandleUtils::Layer(updatedHandle));
    EXPECT_EQ(flags, HandleUtils::Flags(updatedHandle));
}

TEST(EntityHandle_unit_tests, Recyle_ValuesUpdated)
{
    HandleTraits::index_type index = 0u;
    HandleTraits::version_type version = 2u;
    HandleTraits::layer_type layer = 0u;
    HandleTraits::flags_type flags = 1u;
    auto handle = EntityHandle{HandleUtils::Join(index, version, layer, flags)};
    HandleTraits::layer_type newLayer = 1u;
    HandleTraits::flags_type newFlags = 0u;
    auto updatedHandle = HandleUtils::Recycle(handle, newLayer, newFlags);
    EXPECT_EQ(index, HandleUtils::Index(updatedHandle));
    EXPECT_EQ(version + 1u, HandleUtils::Version(updatedHandle));
    EXPECT_EQ(newLayer, HandleUtils::Layer(updatedHandle));
    EXPECT_EQ(newFlags, HandleUtils::Flags(updatedHandle));
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
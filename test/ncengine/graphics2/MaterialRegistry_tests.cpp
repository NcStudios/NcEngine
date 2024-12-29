#include "gtest/gtest.h"
#include "graphics2/frontend/subsystem/MaterialRegistry.h"
#include "graphics2/ShaderTypes.h"

#include "ncutility/NcError.h"

TEST(MaterialRegistryTest, CreateInstance_constructsValidInstance)
{
    const auto expectedDesc = nc::MaterialDesc{
        .name = "test",
        .passes = nc::MaterialPassFlag::Toon,
        .properties = nc::MaterialProperties{
            .diffuseTexture = nc::asset::TextureView{
                .id = 42,
                .index = 2
            },
            .normalTexture = nc::asset::TextureView{
                .id = 10,
                .index = 13
            },
            .gradientStart = nc::Vector3::One(),
            .gradientEnd = nc::Vector3::Zero(),
            .normalIntensity = 1.0f
        }
    };

    auto uut = nc::graphics::MaterialRegistry{5u};
    const auto actualIndex = uut.CreateInstance(expectedDesc);
    uut.BuildState();
    const auto actualProperties = uut.GetInstanceData(actualIndex);
    EXPECT_EQ(0u, actualIndex);
    EXPECT_EQ(expectedDesc.properties.diffuseTexture.index, actualProperties.diffuseTexIndex);
    EXPECT_EQ(expectedDesc.properties.normalTexture.index, actualProperties.normalTexIndex);
    EXPECT_EQ(expectedDesc.properties.gradientStart, actualProperties.gradientStart);
    EXPECT_EQ(expectedDesc.properties.gradientEnd, actualProperties.gradientEnd);
}

TEST(MaterialRegistryTest, CreateInstance_allocatesSequentialIndices)
{
    auto uut = nc::graphics::MaterialRegistry{5u};
    auto first = uut.CreateInstance();
    auto second = uut.CreateInstance();
    auto third = uut.CreateInstance();
    EXPECT_EQ(0, first);
    EXPECT_EQ(1, second);
    EXPECT_EQ(2, third);
}

TEST(MaterialRegistryTest, CreateInstance_exceedsMaxInstances_throws)
{
    auto uut = nc::graphics::MaterialRegistry{2u};
    uut.CreateInstance();
    uut.CreateInstance();
    EXPECT_THROW(uut.CreateInstance(), nc::NcError);
}

TEST(MaterialRegistryTest, DestroyInstance_recyclesIndex)
{
    auto uut = nc::graphics::MaterialRegistry{5u};
    uut.CreateInstance();
    auto second = uut.CreateInstance();
    uut.CreateInstance();
    uut.DestroyInstance(second);
    auto recycled = uut.CreateInstance();
    EXPECT_EQ(1, recycled);
}

TEST(MaterialRegistryTest, SetInstanceName_doesNotSetDirty)
{
    auto uut = nc::graphics::MaterialRegistry{5u};
    auto instance = uut.CreateInstance();
    uut.BuildState();

    uut.SetInstanceName(instance, "updated");
    const auto info = uut.BuildState();
    EXPECT_TRUE(info.instances.empty());
    EXPECT_TRUE(info.dirtyRanges.empty());
}

TEST(MaterialRegistryTest, BuildState_multipleWritesToSameInstance_reportsOnce)
{
    auto uut = nc::graphics::MaterialRegistry{3};
    auto instance = uut.CreateInstance();
    uut.SetInstanceProperties(instance, nc::MaterialProperties{});
    const auto info = uut.BuildState();
    ASSERT_EQ(1, info.dirtyRanges.size());
    const auto& [offset, count] = info.dirtyRanges[0];
    EXPECT_EQ(0, offset);
    EXPECT_EQ(1, count);
}

TEST(MaterialRegistryTest, BuildState_multipleInstances_reportsAsRanges)
{
    auto uut = nc::graphics::MaterialRegistry{3};
    uut.CreateInstance();
    uut.CreateInstance();
    const auto info = uut.BuildState();
    ASSERT_EQ(1, info.dirtyRanges.size());
    const auto& [offset, count] = info.dirtyRanges[0];
    EXPECT_EQ(0, offset);
    EXPECT_EQ(2, count);
}

TEST(MaterialRegistryTest, BuildState_nonContiguousInstanceUpdates_reportsCorrectly)
{
    auto uut = nc::graphics::MaterialRegistry{3};
    auto first = uut.CreateInstance();
    uut.CreateInstance();
    auto third = uut.CreateInstance();
    uut.BuildState();

    uut.SetInstanceProperties(first, nc::MaterialProperties{});
    uut.SetInstanceProperties(third, nc::MaterialProperties{});
    const auto info = uut.BuildState();
    ASSERT_EQ(2, info.dirtyRanges.size());
    const auto& [firstOffset, firstCount] = info.dirtyRanges[0];
    EXPECT_EQ(0, firstOffset);
    EXPECT_EQ(1, firstCount);
    const auto& [secondOffset, secondCount] = info.dirtyRanges[1];
    EXPECT_EQ(2, secondOffset);
    EXPECT_EQ(1, secondCount);
}

TEST(MaterialRegistryTest, AllMethods_indexOutOfBounds_throws)
{
    auto uut = nc::graphics::MaterialRegistry{3};
    const auto badIndex = nc::MaterialInstanceHandle{0};
    EXPECT_THROW(uut.DestroyInstance(badIndex), nc::NcError);
    EXPECT_THROW(uut.GetInstanceDesc(badIndex), nc::NcError);
    EXPECT_THROW(uut.SetInstanceProperties(badIndex, nc::MaterialProperties()), nc::NcError);
    EXPECT_THROW(uut.GetInstanceData(badIndex), nc::NcError);
}

TEST(MaterialRegistryTest, MaterialInstance_wrapsFunctions)
{
    const auto originalDesc = nc::MaterialDesc{.name = "original"};
    auto uut = nc::graphics::MaterialRegistry{3};
    auto first = nc::MaterialInstance{originalDesc};
    auto second = first.Clone();
    first.SetName("new");

    EXPECT_EQ("new", first.GetName());
    EXPECT_EQ(originalDesc.name, second.GetName());
}

TEST(MaterialRegistryTest, MaterialInstance_desctructor_destroysInstanceIfOwner)
{
    auto uut = nc::graphics::MaterialRegistry{3};

    {
        auto instance = nc::MaterialInstance{};
        EXPECT_EQ(0, instance.GetHandle());
    }

    {
        auto instance1 = nc::MaterialInstance{};
        auto instance2 = nc::MaterialInstance{};
        EXPECT_EQ(0, instance1.GetHandle());
        EXPECT_EQ(1, instance2.GetHandle());

        instance2 = std::move(instance1);
        EXPECT_EQ(nc::NullMaterialInstanceHandle, instance1.GetHandle());
        EXPECT_EQ(0, instance2.GetHandle());

        auto moveConstructed = nc::MaterialInstance{std::move(instance2)};
        EXPECT_EQ(nc::NullMaterialInstanceHandle, instance2.GetHandle());
        EXPECT_EQ(0, moveConstructed.GetHandle());
    }

    auto instance = nc::MaterialInstance{};
    EXPECT_EQ(0, instance.GetHandle());
}

#include "gtest/gtest.h"
#include "graphics2/frontend/subsystem/MaterialRegistry.h"
#include "graphics2/ShaderTypes.h"

#include "ncutility/NcError.h"

struct UpdateListener
{
    nc::graphics::BufferUpdateInfo<nc::graphics::MaterialData> receivedInfo;

    auto MakeCallback()
    {
        return [this](const nc::graphics::BufferUpdateInfo<nc::graphics::MaterialData>& info)
        {
            this->receivedInfo = info;
        };
    };

    void Clear()
    {
        receivedInfo = nc::graphics::BufferUpdateInfo<nc::graphics::MaterialData>{};
    }
};

TEST(MaterialRegistryTest, CreateInstance_constructsValidInstance)
{
    const auto expectedDesc = nc::MaterialDesc{
        .name = "test",
        .passes = nc::MaterialPass::Toon,
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
        .outlineColor = nc::Vector3::Splat(0.5f),
        .outlineWidth = 0.2f
    };

    auto uut = nc::graphics::MaterialRegistry{5u};
    const auto actualIndex = uut.CreateInstance(expectedDesc);
    const auto actualProperties = uut.GetInstanceData(actualIndex);
    EXPECT_EQ(0u, actualIndex);
    EXPECT_EQ(expectedDesc.diffuseTexture.index, actualProperties.diffuseTexIndex);
    EXPECT_EQ(expectedDesc.normalTexture.index, actualProperties.normalTexIndex);
    EXPECT_EQ(expectedDesc.gradientStart, actualProperties.gradientStart);
    EXPECT_EQ(expectedDesc.gradientEnd, actualProperties.gradientEnd);
    EXPECT_EQ(expectedDesc.outlineColor, actualProperties.outlineColor);
    EXPECT_EQ(expectedDesc.outlineWidth, actualProperties.outlineWidth);
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

TEST(MaterialRegistryTest, HasPendingChanges_returnsExpectedValue)
{
    auto uut = nc::graphics::MaterialRegistry{3};
    auto listener = UpdateListener{};
    EXPECT_FALSE(uut.HasPendingChanges());
    auto instance = uut.CreateInstance();
    EXPECT_TRUE(uut.HasPendingChanges());
    uut.CommitPendingChanges(listener.MakeCallback());
    EXPECT_FALSE(uut.HasPendingChanges());
    uut.SetInstanceDesc(instance, nc::MaterialDesc{});
    EXPECT_TRUE(uut.HasPendingChanges());
}

TEST(MaterialRegistryTest, CommitPendingChanges_multipleWritesToSameInstance_reportsOnce)
{
    auto uut = nc::graphics::MaterialRegistry{3};
    auto listener = UpdateListener{};
    auto instance = uut.CreateInstance();
    uut.SetInstanceDesc(instance, nc::MaterialDesc{});
    uut.CommitPendingChanges(listener.MakeCallback());
    ASSERT_EQ(1, listener.receivedInfo.dirtyRanges.size());
    const auto& [offset, count] = listener.receivedInfo.dirtyRanges[0];
    EXPECT_EQ(0, offset);
    EXPECT_EQ(1, count);
}

TEST(MaterialRegistryTest, CommitPendingChanges_multipleInstances_reportsAsRanges)
{
    auto uut = nc::graphics::MaterialRegistry{3};
    auto listener = UpdateListener{};
    uut.CreateInstance();
    uut.CreateInstance();
    uut.CommitPendingChanges(listener.MakeCallback());
    ASSERT_EQ(1, listener.receivedInfo.dirtyRanges.size());
    const auto& [offset, count] = listener.receivedInfo.dirtyRanges[0];
    EXPECT_EQ(0, offset);
    EXPECT_EQ(2, count);
}

TEST(MaterialRegistryTest, CommitPendingChanges_nonContiguousInstanceUpdates_reportsCorrectly)
{
    auto uut = nc::graphics::MaterialRegistry{3};
    auto listener = UpdateListener{};
    auto first = uut.CreateInstance();
    uut.CreateInstance();
    auto third = uut.CreateInstance();
    uut.CommitPendingChanges(listener.MakeCallback());

    uut.SetInstanceDesc(first, nc::MaterialDesc{});
    uut.SetInstanceDesc(third, nc::MaterialDesc{});
    uut.CommitPendingChanges(listener.MakeCallback());
    ASSERT_EQ(2, listener.receivedInfo.dirtyRanges.size());
    const auto& [firstOffset, firstCount] = listener.receivedInfo.dirtyRanges[0];
    EXPECT_EQ(0, firstOffset);
    EXPECT_EQ(1, firstCount);
    const auto& [secondOffset, secondCount] = listener.receivedInfo.dirtyRanges[1];
    EXPECT_EQ(2, secondOffset);
    EXPECT_EQ(1, secondCount);
}

TEST(MaterialRegistryTest, AllMethods_indexOutOfBounds_throws)
{
    auto uut = nc::graphics::MaterialRegistry{3};
    const auto badIndex = nc::MaterialInstanceHandle{0};
    EXPECT_THROW(uut.DestroyInstance(badIndex), nc::NcError);
    EXPECT_THROW(uut.GetInstanceDesc(badIndex), nc::NcError);
    EXPECT_THROW(uut.SetInstanceDesc(badIndex, nc::MaterialDesc()), nc::NcError);
    EXPECT_THROW(uut.GetInstanceData(badIndex), nc::NcError);
}

TEST(MaterialRegistryTest, MaterialInstance_wrapsFunctions)
{
    const auto originalDesc = nc::MaterialDesc{.name = "original"};
    const auto newDesc = nc::MaterialDesc{.name = "original"};
    auto uut = nc::graphics::MaterialRegistry{3};
    auto first = nc::MaterialInstance{originalDesc};
    auto second = first.Clone();
    first.SetDesc(newDesc);

    EXPECT_EQ(newDesc.name, first.GetDesc().name);
    EXPECT_EQ(originalDesc.name, second.GetDesc().name);
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

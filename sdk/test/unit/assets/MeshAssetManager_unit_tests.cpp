#include "gtest/gtest.h"

#include "graphics/GpuAllocator.h"
#include "graphics/Graphics.h"
#include "graphics/resources/ImmutableBuffer.h"
#include "assets/MeshAssetManager.h"

using namespace nc;

const auto MeshPath_3Vertices = "mesh3Verts.nca";
const auto MeshPath_6Vertices = "mesh6Verts.nca";
const auto MeshPath_9Vertices = "mesh9Verts.nca";

namespace nc::graphics
{
    auto GpuAllocator::CreateBuffer(uint32_t, vk::BufferUsageFlags, vma::MemoryUsage) -> GpuAllocation<vk::Buffer> { return {}; }
    void GpuAllocator::Destroy(const GpuAllocation<vk::Buffer>&) const {}
    ImmutableBuffer::ImmutableBuffer() {}
    ImmutableBuffer::ImmutableBuffer(Base*, GpuAllocator*, const std::vector<uint32_t>&) {}
    ImmutableBuffer::ImmutableBuffer(Base*, GpuAllocator*, const std::vector<Vertex>&) {}
    void ImmutableBuffer::Clear() noexcept {}
    auto Graphics::GetBasePtr() const noexcept -> Base* { return nullptr; }
    auto Graphics::GetAllocatorPtr() const noexcept -> GpuAllocator* { return nullptr; }
}

class MeshAssetManager_unit_tests : public ::testing::Test
{
    public:
        std::unique_ptr<MeshAssetManager> assetManager;

    protected:
        void SetUp() override
        {
            assetManager = std::make_unique<MeshAssetManager>(nullptr, NC_TEST_COLLATERAL_DIRECTORY);
        }

        void TearDown() override
        {
            assetManager = nullptr;
        }
};

TEST_F(MeshAssetManager_unit_tests, Load_NotLoaded_ReturnsTrue)
{
    auto actual = assetManager->Load(MeshPath_3Vertices, false);
    EXPECT_TRUE(actual);
}

TEST_F(MeshAssetManager_unit_tests, Load_IsLoaded_ReturnsFalse)
{
    assetManager->Load(MeshPath_3Vertices, false);
    auto actual = assetManager->Load(MeshPath_3Vertices, false);
    EXPECT_FALSE(actual);
}

TEST_F(MeshAssetManager_unit_tests, Load_BadPath_Throws)
{
    EXPECT_THROW(assetManager->Load("bad/path", false), std::runtime_error);
}

TEST_F(MeshAssetManager_unit_tests, Load_Collection_ReturnsTrue)
{
    std::array<std::string, 3u> paths {MeshPath_3Vertices, MeshPath_6Vertices, MeshPath_9Vertices};
    auto actual = assetManager->Load(paths, false);
    EXPECT_TRUE(actual);
}

TEST_F(MeshAssetManager_unit_tests, Load_Collection_OffsetsAccessors)
{
    std::array<std::string, 3u> paths {MeshPath_3Vertices, MeshPath_6Vertices, MeshPath_9Vertices};
    assetManager->Load(paths, false);

    auto view3 = assetManager->Acquire(MeshPath_3Vertices);
    auto view6 = assetManager->Acquire(MeshPath_6Vertices);
    auto view9 = assetManager->Acquire(MeshPath_9Vertices);

    EXPECT_EQ(view3.firstIndex, 0u);
    EXPECT_EQ(view3.vertexCount, 3u);
    EXPECT_EQ(view3.firstVertex, 0u);
    EXPECT_EQ(view3.indexCount, 3u);
    EXPECT_EQ(view6.firstIndex, 3u);
    EXPECT_EQ(view6.vertexCount, 6u);
    EXPECT_EQ(view6.firstVertex, 3u);
    EXPECT_EQ(view6.indexCount, 6u);
    EXPECT_EQ(view9.firstIndex, 9u);
    EXPECT_EQ(view9.vertexCount, 9u);
    EXPECT_EQ(view9.firstVertex, 9u);
    EXPECT_EQ(view9.indexCount, 9u);
}

TEST_F(MeshAssetManager_unit_tests, Unload_Loaded_ReturnsTrue)
{
    assetManager->Load(MeshPath_3Vertices, false);
    auto actual = assetManager->Unload(MeshPath_3Vertices);
    EXPECT_TRUE(actual);
}

TEST_F(MeshAssetManager_unit_tests, Unload_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->Unload(MeshPath_3Vertices);
    EXPECT_FALSE(actual);
}

TEST_F(MeshAssetManager_unit_tests, IsLoaded_Loaded_ReturnsTrue)
{
    assetManager->Load(MeshPath_3Vertices, false);
    auto actual = assetManager->IsLoaded(MeshPath_3Vertices);
    EXPECT_TRUE(actual);
}

TEST_F(MeshAssetManager_unit_tests, IsLoaded_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->IsLoaded(MeshPath_3Vertices);
    EXPECT_FALSE(actual);
}

TEST_F(MeshAssetManager_unit_tests, UnloadAll_HasAssets_RemovesAssets)
{
    std::array<std::string, 3u> paths {MeshPath_3Vertices, MeshPath_6Vertices, MeshPath_9Vertices};
    assetManager->Load(paths, false);
    assetManager->UnloadAll();
    EXPECT_FALSE(assetManager->IsLoaded(MeshPath_3Vertices));
    EXPECT_FALSE(assetManager->IsLoaded(MeshPath_6Vertices));
    EXPECT_FALSE(assetManager->IsLoaded(MeshPath_9Vertices));
}

TEST_F(MeshAssetManager_unit_tests, UnloadAll_Empty_Completes)
{
    assetManager->UnloadAll();
}

TEST_F(MeshAssetManager_unit_tests, Unload_FromBeginning_UpdatesAccesors)
{
    std::array<std::string, 3u> paths {MeshPath_3Vertices, MeshPath_6Vertices, MeshPath_9Vertices};
    assetManager->Load(paths, false);
    assetManager->Unload(MeshPath_3Vertices);
    auto view6 = assetManager->Acquire(MeshPath_6Vertices);
    auto view9 = assetManager->Acquire(MeshPath_9Vertices);
    EXPECT_EQ(view6.firstIndex, 0u);
    EXPECT_EQ(view6.firstVertex, 0u);
    EXPECT_EQ(view6.indexCount, 6u);
    EXPECT_EQ(view9.firstIndex, 6u);
    EXPECT_EQ(view9.firstVertex, 6u);
    EXPECT_EQ(view9.indexCount, 9u);
}

TEST_F(MeshAssetManager_unit_tests, Unload_FromMiddle_UpdatesAccesors)
{
    std::array<std::string, 3u> paths {MeshPath_3Vertices, MeshPath_6Vertices, MeshPath_9Vertices};
    assetManager->Load(paths, false);
    assetManager->Unload(MeshPath_6Vertices);
    auto view3 = assetManager->Acquire(MeshPath_3Vertices);
    auto view9 = assetManager->Acquire(MeshPath_9Vertices);
    EXPECT_EQ(view3.firstVertex, 0u);
    EXPECT_EQ(view3.vertexCount, 3u);
    EXPECT_EQ(view3.firstIndex, 0u);
    EXPECT_EQ(view3.indexCount, 3u);
    EXPECT_EQ(view9.firstVertex, 3u);
    EXPECT_EQ(view9.vertexCount, 9u);
    EXPECT_EQ(view9.firstIndex, 3u);
    EXPECT_EQ(view9.indexCount, 9u);
}

TEST_F(MeshAssetManager_unit_tests, Unload_FromEnd_UpdatesAccesors)
{
    std::array<std::string, 3u> paths {MeshPath_3Vertices, MeshPath_6Vertices, MeshPath_9Vertices};
    assetManager->Load(paths, false);
    assetManager->Unload(MeshPath_9Vertices);
    auto view3 = assetManager->Acquire(MeshPath_3Vertices);
    auto view6 = assetManager->Acquire(MeshPath_6Vertices);
    EXPECT_EQ(view3.firstIndex, 0u);
    EXPECT_EQ(view3.firstVertex, 0u);
    EXPECT_EQ(view3.indexCount, 3u);
    EXPECT_EQ(view6.firstIndex, 3u);
    EXPECT_EQ(view6.firstVertex, 3u);
    EXPECT_EQ(view6.indexCount, 6u);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
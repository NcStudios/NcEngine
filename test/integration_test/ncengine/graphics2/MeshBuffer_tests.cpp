#include "DiligentEngineFixture.inl"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "ncasset/Assets.h"

#include <ranges>

class MeshBufferTest : public DiligentEngineFixture
{
    protected:
        std::unique_ptr<nc::graphics::MeshBuffer> uut;

        MeshBufferTest()
        {
            uut = std::make_unique<nc::graphics::MeshBuffer>();
        }

        ~MeshBufferTest()
        {
            FailIfHasErrorOutput();
        }
};

constexpr auto g_pixelShader = std::string_view{
R"(struct PSInput
{
    float4 Pos         : SV_POSITION;
    float3 Normal      : NORMAL;
    float2 UV          : TEXCOORD0;
    float4 BoneWeights : BONE_WEIGHTS;
    uint4  BoneIds     : BONE_IDS;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

void main(in PSInput PSIn, out PSOutput PSOut)
{
    PSOut.Color = PSIn.Pos;

    // use other values to suppress 'output not consumed'
    float dummy = 0.0;
    dummy += PSIn.Normal.x;
    dummy += PSIn.UV.x;
    dummy += PSIn.BoneWeights.x;
    dummy += float(PSIn.BoneIds.x);
}
)"};

constexpr auto g_vertexShader = std::string_view{
R"(struct VSInput
{
    float3 Pos         : ATTRIB0;
    float3 Normal      : ATTRIB1;
    float2 UV          : ATTRIB2;
    float4 BoneWeights : ATTRIB3;
    uint4  BoneIds     : ATTRIB4;
};

struct PSInput
{
    float4 Pos         : SV_POSITION;
    float3 Normal      : NORMAL;
    float2 UV          : TEXCOORD0;
    float4 BoneWeights : BONE_WEIGHTS;
    uint4  BoneIds     : BONE_IDS;
};

void main(in VSInput VSIn, out PSInput PSIn)
{
    PSIn.Pos = float4(VSIn.Pos, 1);
    PSIn.Normal = VSIn.Normal;
    PSIn.UV = VSIn.UV;
    PSIn.BoneWeights = VSIn.BoneWeights;
    PSIn.BoneIds = VSIn.BoneIds;
}
)"};

constexpr auto g_vertex1 = nc::asset::MeshVertex{
    .position = nc::Vector3::Up()
};

constexpr auto g_vertex2 = nc::asset::MeshVertex{
    .position = nc::Vector3::Down()
};

constexpr auto g_vertex3 = nc::asset::MeshVertex{
    .position = nc::Vector3::Right()
};

TEST(MeshBufferUtilityTest, GetMeshVertexLayoutElements_elementsPositionedCorrectly)
{
    const auto expectedSlot = 1u;
    const auto expectedOffset = 3u;
    const auto actual = nc::graphics::GetMeshVertexLayoutElements(
        nc::graphics::VertexAttribute::All,
        expectedSlot,
        expectedOffset
    );

    for (const auto& [i, actualElement] : std::views::enumerate(actual))
    {
        EXPECT_EQ(expectedSlot, actualElement.BufferSlot);
        EXPECT_EQ(i + expectedOffset, actualElement.InputIndex);
    }
}

TEST(MeshBufferUtilityTest, GetMeshVertexLayoutElements_setsRelativeOffsets)
{
    constexpr auto attributeSizes = std::array{
        sizeof(nc::asset::MeshVertex::position),
        sizeof(nc::asset::MeshVertex::normal),
        sizeof(nc::asset::MeshVertex::uv),
        sizeof(nc::asset::MeshVertex::boneWeights),
        sizeof(nc::asset::MeshVertex::boneIds)
    };

    const auto actual = nc::graphics::GetMeshVertexLayoutElements(
        nc::graphics::VertexAttribute::Pos         |
        nc::graphics::VertexAttribute::Normal      |
        nc::graphics::VertexAttribute::UV          |
        nc::graphics::VertexAttribute::BoneWeights |
        nc::graphics::VertexAttribute::BoneIds
    );

    ASSERT_EQ(attributeSizes.size(), actual.size());

    constexpr auto expectedStride = sizeof(nc::asset::MeshVertex);
    auto relativeOffset = 0ull;
    for (const auto [size, layoutElement] : std::views::zip(attributeSizes, actual))
    {
        EXPECT_EQ(expectedStride, layoutElement.Stride);
        EXPECT_EQ(relativeOffset, layoutElement.RelativeOffset);
        relativeOffset += size;
    }
}

TEST(MeshBufferUtilityTest, GetMeshVertexLayoutElements_nonContiguousAttributes_setsOffsets)
{
    const auto expectedOffset = static_cast<uint32_t>(
        sizeof(nc::asset::MeshVertex::position) +
        sizeof(nc::asset::MeshVertex::normal)
    );

    const auto actual = nc::graphics::GetMeshVertexLayoutElements(
        nc::graphics::VertexAttribute::Pos |
        nc::graphics::VertexAttribute::UV
    );

    ASSERT_EQ(2, actual.size());
    const auto& actualPos = actual.at(0);
    const auto& actualUV = actual.at(1);
    EXPECT_EQ(0, actualPos.RelativeOffset);
    EXPECT_EQ(expectedOffset, actualUV.RelativeOffset);
}


TEST_F(MeshBufferTest, Load_initialCall_succeeds)
{
    const auto vertices = std::array{g_vertex1, g_vertex2, g_vertex3};
    const auto indices = std::array{0u, 1u, 2u};
    EXPECT_NO_THROW(uut->Load(vertices, indices, engine->GetContext(), engine->GetDevice()));
}

TEST_F(MeshBufferTest, Load_overwriteBuffer_succeeds)
{
    auto& context = engine->GetContext();
    auto& device = engine->GetDevice();

    const auto initialVertices = std::array{g_vertex1};
    const auto initialIndices = std::array{0u};
    uut->Load(initialVertices, initialIndices, context, device);

    const auto overwriteVertices = std::array{g_vertex1, g_vertex2, g_vertex3, g_vertex3, g_vertex2, g_vertex1};
    const auto overwriteIndices = std::array{0u, 1u, 2u, 3u, 4u, 5u};
    EXPECT_NO_THROW(uut->Load(overwriteVertices, overwriteIndices, context, device));
}

TEST_F(MeshBufferTest, Load_empty_throws)
{
    const auto vertices = std::span<const nc::asset::MeshVertex>{};
    const auto indices = std::span<const uint32_t>{};
    EXPECT_THROW(uut->Load(vertices, indices, engine->GetContext(), engine->GetDevice()), nc::NcError);
    ClearErrorOutput();
}

TEST_F(MeshBufferTest, SetBuffers_multipleCalls_succeed)
{
    auto& context = engine->GetContext();
    const auto vertices = std::array{g_vertex1, g_vertex2, g_vertex3};
    const auto indices = std::array{0u, 1u, 2u};
    uut->Load(vertices, indices, context, engine->GetDevice());
    EXPECT_NO_THROW(uut->SetBuffers(context));
    EXPECT_NO_THROW(uut->SetBuffers(context));
    EXPECT_NO_THROW(uut->SetBuffers(context));
}

TEST_F(MeshBufferTest, SetBuffers_noDataLoaded_throws)
{
    auto& context = engine->GetContext();
    EXPECT_THROW(uut->SetBuffers(context), nc::NcError);
    ClearErrorOutput();
}

TEST_F(MeshBufferTest, Load_and_SetBuffers_subsequentDrawCallsSucceed)
{
    auto& context = engine->GetContext();
    auto& device = engine->GetDevice();
    auto& swapChain = engine->GetSwapChain();

    auto layout = nc::graphics::GetMeshVertexLayoutElements(nc::graphics::VertexAttribute::All);
    auto pso = CreateTestGraphicsPipelineState(
        std::span{g_vertexShader},
        std::span{g_pixelShader},
        layout
    );

    auto* pRTV = swapChain.GetCurrentBackBufferRTV();
    auto* pDSV = swapChain.GetDepthBufferDSV();
    context.SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    context.SetPipelineState(pso);

    auto attrs = Diligent::DrawIndexedAttribs{
        0u,
        Diligent::VT_UINT32,
        Diligent::DRAW_FLAG_VERIFY_ALL
    };

    {
        const auto vertices = std::array{g_vertex1, g_vertex2, g_vertex3};
        const auto indices = std::array{0u, 1u, 2u};
        uut->Load(vertices, indices, context, engine->GetDevice());
        uut->SetBuffers(context);
        attrs.NumIndices = static_cast<uint32_t>(indices.size());
        context.DrawIndexed(attrs);
    }

    {
        const auto vertices = std::array{g_vertex1, g_vertex2, g_vertex3, g_vertex1, g_vertex2, g_vertex3};
        const auto indices = std::array{0u, 1u, 2u, 3u, 4u, 5u};
        uut->Load(vertices, indices, context, device);
        uut->SetBuffers(context);
        attrs.NumIndices = static_cast<uint32_t>(indices.size());
        context.DrawIndexed(attrs);
    }
}

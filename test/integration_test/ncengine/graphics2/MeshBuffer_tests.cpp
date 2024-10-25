#include "DiligentEngineParameterizedFixture.inl"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "ncasset/Assets.h"


#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"

class MeshBufferTest : public DiligentEngineParameterizedFixture
{
    protected:
        std::unique_ptr<nc::graphics::MeshBuffer> uut;

        void SetUp() override
        {
            INITIALIZE_DILIGENT_FIXTURE;
            uut = std::make_unique<nc::graphics::MeshBuffer>();
        }

        void TearDown() override
        {
            FailIfHasErrorOutput();
        }
};

INSTANTIATE_TEST_SUITE_P(AllApis, MeshBufferTest, g_apiParams);

constexpr auto g_pixelShader = std::string_view{
R"(struct PSInput
{
    float4 Pos      : SV_POSITION;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

void main(in  PSInput  PSIn,
          out PSOutput PSOut)
{
    PSOut.Color = float4(PSIn, 1);
}
)"};

constexpr auto g_vertexShader = std::string_view{
R"(struct VSInput
{
    float3 Pos         : ATTRIB0;
    float3 Normal      : ATTRIB1;
    float2 UV          : ATTRIB2;
    float3 Tangent     : ATTRIB3;
    float3 Bitangent   : ATTRIB4;
    float4 BoneWeights : ATTRIB5;
    uint4  BoneIds     : ATTRIB6;
};

struct PSInput
{
    float4 Pos : SV_POSITION;
};

void main(in VSInput VSIn, out PSInput PSIn)
{
    PSIn.Pos = VSIn.Pos;
}
)"};

auto CreateTestPipelineState(Diligent::IRenderDevice& device,
                             Diligent::ISwapChain& swapChain,
                             nc::graphics::ShaderFactory& shaderFactory) -> Diligent::RefCntAutoPtr<Diligent::IPipelineState>
{
    using namespace Diligent;
    auto createInfo = GraphicsPipelineStateCreateInfo{};
    createInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    createInfo.PSODesc.Name = "Test PSO";

    createInfo.GraphicsPipeline.NumRenderTargets             = 1;
    createInfo.GraphicsPipeline.RTVFormats[0]                = swapChain.GetDesc().ColorBufferFormat;
    createInfo.GraphicsPipeline.DSVFormat                    = swapChain.GetDesc().DepthBufferFormat;
    createInfo.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    createInfo.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    createInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;

    auto vertexShader = shaderFactory.MakeShaderFromSource(
        std::span{g_vertexShader},
        "Cube VS",
        Diligent::SHADER_TYPE_VERTEX
    );

    auto pixelShader = shaderFactory.MakeShaderFromSource(
        std::span{g_pixelShader},
        "Cube PS",
        Diligent::SHADER_TYPE_PIXEL
    );

    createInfo.pVS = vertexShader;
    createInfo.pPS = pixelShader;

    auto LayoutElems = std::array{
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        LayoutElement{1, 0, 3, VT_FLOAT32, False},
        LayoutElement{2, 0, 2, VT_FLOAT32, False},
        LayoutElement{3, 0, 3, VT_FLOAT32, False},
        LayoutElement{4, 0, 3, VT_FLOAT32, False},
        LayoutElement{5, 0, 4, VT_FLOAT32, False},
        LayoutElement{6, 0, 4, VT_UINT32,  False},
    };

    createInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems.data();
    createInfo.GraphicsPipeline.InputLayout.NumElements    = static_cast<uint32_t>(LayoutElems.size());

    auto pso = Diligent::RefCntAutoPtr<Diligent::IPipelineState>{};
    device.CreateGraphicsPipelineState(createInfo, &pso);
    NC_ASSERT(pso, "Failed to create pipeline state object");
    return pso;
}



constexpr auto g_vertex1 = nc::asset::MeshVertex{
    .position = nc::Vector3::Up()
};

constexpr auto g_vertex2 = nc::asset::MeshVertex{
    .position = nc::Vector3::Down()
};

constexpr auto g_vertex3 = nc::asset::MeshVertex{
    .position = nc::Vector3::Right()
};

TEST_P(MeshBufferTest, SetBuffers_empty_succeeds)
{
    
}

TEST_P(MeshBufferTest, SetBuffers_setsData)
{
    auto pso = CreateTestPipelineState(
        engine->GetDevice(),
        engine->GetSwapChain(),
        engine->GetShaderFactory()
    );

    const auto vertices = std::array{g_vertex1, g_vertex2, g_vertex3};
    const auto indices = std::array{0u, 1u, 2u};

    uut->Load(vertices, indices, engine->GetContext(), engine->GetDevice());

    engine->GetContext().SetPipelineState(pso);

    Diligent::DrawIndexedAttribs drawAttrs;
    drawAttrs.IndexType  = Diligent::VT_UINT32;   // Assuming indices are of uint32 type
    drawAttrs.NumIndices = 3/* number of indices in the buffer */;
    drawAttrs.Flags      = Diligent::DRAW_FLAG_VERIFY_ALL;

    // Perform the draw call
    engine->GetContext().DrawIndexed(drawAttrs);
}

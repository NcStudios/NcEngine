#include "TestPipeline.h"
#include "resource/GlobalMeshBuffer.h"
#include "ncasset/Assets.h"
#include "graphics2/frontend/FrontendRenderState.h"

#include "Graphics/GraphicsEngine/interface/PipelineState.h"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"
#include "Graphics/GraphicsTools/interface/ShaderMacroHelper.hpp"

#include <array>

using namespace Diligent;

namespace
{
constexpr auto g_pixelShader = std::string_view{
R"(#ifdef VULKAN
// NonUniformResourceIndex is not supported by GLSLang
#   define NonUniformResourceIndex(x) x
#endif

Texture2D     TextureBufferData[];
SamplerState  TextureBufferData_sampler; // By convention, texture samplers must use the '_sampler' suffix

struct MaterialData
{
    uint diffuseTexture;
    uint normalIndex;
    float3 gradientStart;
    float3 gradientEnd;
    float3 outlineColor;
    float outlineWidth;
};

StructuredBuffer<MaterialData> MaterialBufferData : register(t1);

struct PSInput 
{ 
    float4 Pos      : SV_POSITION; 
    float2 UV       : TEX_COORD; 
    uint   TexIndex : TEX_ARRAY_INDEX;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

void main(in  PSInput  PSIn,
          out PSOutput PSOut)
{
    float4 Color;
    Color = TextureBufferData[NonUniformResourceIndex(PSIn.TexIndex)].Sample(TextureBufferData_sampler, PSIn.UV);
    PSOut.Color = Color;
})"};

constexpr auto g_vertexShader = std::string_view{
R"(struct VSInput
{
    // Vertex attributes
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
    float4 Pos      : SV_POSITION;
    float2 UV       : TEX_COORD;
    uint   TexIndex : TEX_ARRAY_INDEX;
};

struct MeshRendererData
{
    float4x4 model;
};

StructuredBuffer<MeshRendererData> MeshRendererBufferData;

cbuffer EnvironmentBufferData
{
    float4x4 cameraViewProjection;
};

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID,  out PSInput PSIn)
{
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), MeshRendererBufferData[InstanceID].model);
    PSIn.Pos = mul(TransformedPos, cameraViewProjection);
    PSIn.UV  = VSIn.UV;
    PSIn.TexIndex = 0;
}
)"};
} // anonymous namespace

namespace nc::graphics
{
TestPipeline::TestPipeline(IRenderDevice& device,
                           ISwapChain& swapChain,
                           ShaderFactory& shaderFactory,
                           Diligent::IPipelineResourceSignature& globalResourceSignature,
                           Diligent::IPipelineResourceSignature& componentResourceSignature,
                           Diligent::IPipelineResourceSignature& materialResourceSignature)
{
    CreatePipelineState(device, swapChain, shaderFactory, globalResourceSignature, componentResourceSignature, materialResourceSignature);
}

void TestPipeline::CreatePipelineState(IRenderDevice& device,
                                       ISwapChain& swapChain,
                                       ShaderFactory& shaderFactory,
                                       Diligent::IPipelineResourceSignature& globalResourceSignature,
                                       Diligent::IPipelineResourceSignature& componentResourceSignature,
                                       Diligent::IPipelineResourceSignature& materialResourceSignature)
{
    auto createInfo = GraphicsPipelineStateCreateInfo{};
    createInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    createInfo.PSODesc.Name = "Test PSO";

    auto signatures = std::array{&globalResourceSignature, &componentResourceSignature, &materialResourceSignature};
    createInfo.ppResourceSignatures = signatures.data();
    createInfo.ResourceSignaturesCount = static_cast<uint32_t>(signatures.size());

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

    const auto vertexElements = GetMeshVertexLayoutElements(0);
    auto LayoutElems = std::array{
        // Per-vertex data - first buffer slot
        vertexElements.at(0),
        vertexElements.at(1),
        vertexElements.at(2),
        vertexElements.at(3),
        vertexElements.at(4),
        vertexElements.at(5),
        vertexElements.at(6),
    };

    createInfo.PSODesc.ResourceLayout.DefaultVariableType =  SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
    createInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems.data();
    createInfo.GraphicsPipeline.InputLayout.NumElements    = static_cast<uint32_t>(LayoutElems.size());

    device.CreateGraphicsPipelineState(createInfo, &m_pBindlessPSO);

    NC_ASSERT(m_pBindlessPSO, "Failed to create pipeline state object");
}

void TestPipeline::Render(Diligent::IDeviceContext& context,
                          ecs::ExplicitEcs<ToonRenderer> ecs,
                          const nc::graphics::FrontendRenderState& renderState)
{
    context.SetPipelineState(m_pBindlessPSO);

    auto i = 0u;
    for (auto entity : renderState.meshRendererState.entities)
    {
        const auto& renderer = ecs.Get<ToonRenderer>(entity);
        const auto& meshView = renderer.GetMeshView();

        const auto attributes = DrawIndexedAttribs{
            meshView.indexCount,
            VT_UINT32,
            DRAW_FLAG_VERIFY_ALL | DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT,
            1,
            meshView.firstIndex,
            meshView.firstVertex,
            i++
        };

        context.DrawIndexed(attributes);
    }
}
} // namespace nc::graphics

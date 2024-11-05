#include "TestPipeline.h"
#include "resource/GlobalMeshBuffer.h"
#include "ncasset/Assets.h"

#include "Graphics/GraphicsEngine/interface/PipelineState.h"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"
#include "Graphics/GraphicsTools/interface/ShaderMacroHelper.hpp"

#include <array>
#include <random>

using namespace Diligent;

namespace
{
constexpr auto g_pixelShader = std::string_view{
R"(#ifdef VULKAN
// NonUniformResourceIndex is not supported by GLSLang
#   define NonUniformResourceIndex(x) x
#endif

Texture2D     Textures[];
SamplerState  Textures_sampler; // By convention, texture samplers must use the '_sampler' suffix

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
    Color = Textures[NonUniformResourceIndex(PSIn.TexIndex)].Sample(Textures_sampler, PSIn.UV);
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

    // Instance attributes
    float4 MtrxRow0  : ATTRIB7;
    float4 MtrxRow1  : ATTRIB8;
    float4 MtrxRow2  : ATTRIB9;
    float4 MtrxRow3  : ATTRIB10;
    uint   TexArrInd : ATTRIB11;
};

struct PSInput 
{
    float4 Pos      : SV_POSITION;
    float2 UV       : TEX_COORD;
    uint   TexIndex : TEX_ARRAY_INDEX;
};

cbuffer EnvironmentData
{
    float4x4 cameraViewProjection;
};

void main(in  VSInput VSIn, out PSInput PSIn)
{
    float4x4 InstanceMatr = MatrixFromRows(VSIn.MtrxRow0, VSIn.MtrxRow1, VSIn.MtrxRow2, VSIn.MtrxRow3);
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), InstanceMatr);
    PSIn.Pos = mul(TransformedPos, cameraViewProjection);
    PSIn.UV  = VSIn.UV;
    PSIn.TexIndex = VSIn.TexArrInd;
}
)"};
} // anonymous namespace

namespace nc::graphics
{
TestPipeline::TestPipeline(IDeviceContext& context,
                           IRenderDevice& device,
                           ISwapChain& swapChain,
                           ShaderFactory& shaderFactory,
                           Diligent::IPipelineResourceSignature& globalResourceSignature,
                           Diligent::IPipelineResourceSignature& componentResourceSignature)
{
    CreatePipelineState(device, swapChain, shaderFactory, globalResourceSignature, componentResourceSignature);
    CreateInstanceBuffer(context, device);
}

void TestPipeline::CreatePipelineState(IRenderDevice& device,
                                       ISwapChain& swapChain,
                                       ShaderFactory& shaderFactory,
                                       Diligent::IPipelineResourceSignature& globalResourceSignature,
                                       Diligent::IPipelineResourceSignature& componentResourceSignature)
{
    auto createInfo = GraphicsPipelineStateCreateInfo{};
    createInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    createInfo.PSODesc.Name = "Test PSO";

    auto signatures = std::array<Diligent::IPipelineResourceSignature*, 2>
    {
        &globalResourceSignature,
        &componentResourceSignature
    };

    createInfo.ppResourceSignatures = signatures.data();
    createInfo.ResourceSignaturesCount = 2;

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

        // Per-instance data - second buffer slot
        // We will use four attributes to encode instance-specific 4x4 transformation matrix
        LayoutElement{7, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
        LayoutElement{8, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
        LayoutElement{9, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
        LayoutElement{10, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
        LayoutElement{11, 1, 1, VT_UINT32,  False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
    };

    createInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems.data();
    createInfo.GraphicsPipeline.InputLayout.NumElements    = static_cast<uint32_t>(LayoutElems.size());

    device.CreateGraphicsPipelineState(createInfo, &m_pBindlessPSO);
    NC_ASSERT(m_pBindlessPSO, "Failed to create pipeline state object");
}

void TestPipeline::CreateInstanceBuffer(IDeviceContext& context, IRenderDevice& device)
{
    // Create instance data buffer that will store transformation matrices
    BufferDesc InstBuffDesc;
    InstBuffDesc.Name = "Instance data buffer";
    // Use default usage as this buffer will only be updated when grid size changes
    InstBuffDesc.Usage     = USAGE_DEFAULT;
    InstBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
    InstBuffDesc.Size      = sizeof(InstanceData) * MaxInstances;
    device.CreateBuffer(InstBuffDesc, nullptr, &m_InstanceBuffer);
    PopulateInstanceBuffer(context);
}

void TestPipeline::PopulateInstanceBuffer(IDeviceContext& context)
{
    // Populate instance data buffer
    const auto zGridSize = static_cast<size_t>(m_GridSize);
    m_InstanceData.resize(zGridSize * zGridSize * zGridSize);
    m_GeometryType.resize(zGridSize * zGridSize * zGridSize);

    float fGridSize = static_cast<float>(m_GridSize);

    std::mt19937 gen;
    std::uniform_real_distribution<float> scale_distr(0.3f, 1.0f);
    std::uniform_real_distribution<float> offset_distr(-0.15f, +0.15f);
    std::uniform_real_distribution<float> rot_distr(-3.14f, 3.14f);
    std::uniform_int_distribution<uint32_t> tex_distr(0, 19 - 1); // note: just based on sample assets
    std::uniform_int_distribution<uint32_t> geom_type_distr(0, static_cast<uint32_t>(10)); // note: lower bound on sample asset; probably more available

    float BaseScale = 0.6f / fGridSize;
    int   instId    = 0;
    for (int x = 0; x < m_GridSize; ++x)
    {
        for (int y = 0; y < m_GridSize; ++y)
        {
            for (int z = 0; z < m_GridSize; ++z)
            {
                float xOffset = 2.f * ((float)x + 0.5f + offset_distr(gen)) / fGridSize - 1.f;
                float yOffset = 2.f * ((float)y + 0.5f + offset_distr(gen)) / fGridSize - 1.f;
                float zOffset = 3.0f + 2.f * ((float)z + 0.5f + offset_distr(gen)) / fGridSize - 1.f;
                float scale = BaseScale * scale_distr(gen);
                auto matrix = DirectX::XMMatrixMultiply(
                    DirectX::XMMatrixMultiply(
                        DirectX::XMMatrixRotationRollPitchYaw(rot_distr(gen), rot_distr(gen), rot_distr(gen)),
                        DirectX::XMMatrixScaling(scale, scale, scale)
                    ),
                    DirectX::XMMatrixTranslation(xOffset, yOffset, zOffset)
                );

                auto& CurrInst = m_InstanceData[instId];
                DirectX::XMStoreFloat4x4(&CurrInst.Matrix, matrix);
                CurrInst.TextureInd = tex_distr(gen);
                m_GeometryType[instId++] = geom_type_distr(gen);
            }
        }
    }

    auto DataSize = static_cast<uint32_t>(sizeof(InstanceData) * m_InstanceData.size());
    context.UpdateBuffer(m_InstanceBuffer, 0, DataSize, m_InstanceData.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    StateTransitionDesc Barrier(m_InstanceBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
    context.TransitionResourceStates(1, &Barrier);
}

void TestPipeline::Render(Diligent::IDeviceContext& context,
                          ecs::ExplicitEcs<ToonRenderer> ecs)
{
    auto instanceBuffer = m_InstanceBuffer.RawPtr();
    context.SetVertexBuffers(
        1,
        1,
        &instanceBuffer,
        nullptr,
        RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
        SET_VERTEX_BUFFERS_FLAG_NONE
    );

    context.SetPipelineState(m_pBindlessPSO);

    const auto renderers = ecs.GetAll<ToonRenderer>();
    const auto numObjects = std::min(renderers.size(), (size_t)(m_GridSize * m_GridSize * m_GridSize));
    for (auto i = 0ull; i < numObjects; ++i)
    {
        const auto& meshView = renderers[i].GetMeshView();
        const auto attributes = DrawIndexedAttribs{
            meshView.indexCount,
            VT_UINT32,
            DRAW_FLAG_VERIFY_ALL | DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT,
            1,
            meshView.firstIndex,
            meshView.firstVertex,
            static_cast<uint32_t>(i)
        };

        context.DrawIndexed(attributes);
    }
}
} // namespace nc::graphics

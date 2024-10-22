#include "TestPipeline.h"

#include "Graphics/GraphicsEngine/interface/PipelineState.h"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"
#include "Graphics/GraphicsTools/interface/ShaderMacroHelper.hpp"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"

#include <array>
#include <random>

using namespace Diligent;

namespace
{
struct Vertex
{
    nc::Vector3 pos;
    nc::Vector2 uv;
};

constexpr auto g_pixelShader = std::string_view{
R"(#ifdef VULKAN
// NonUniformResourceIndex is not supported by GLSLang
#   define NonUniformResourceIndex(x) x
#endif

Texture2D     g_textures[];
SamplerState  g_textures_sampler; // By convention, texture samplers must use the '_sampler' suffix

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
    Color = g_textures[NonUniformResourceIndex(PSIn.TexIndex)].Sample(g_textures_sampler, PSIn.UV);
    PSOut.Color = Color;
})"};

constexpr auto g_vertexShader = std::string_view{
R"(struct VSInput
{
    // Vertex attributes
    float3 Pos      : ATTRIB0;
    float2 UV       : ATTRIB1;

    // Instance attributes
    float4 MtrxRow0  : ATTRIB2;
    float4 MtrxRow1  : ATTRIB3;
    float4 MtrxRow2  : ATTRIB4;
    float4 MtrxRow3  : ATTRIB5;
    uint   TexArrInd : ATTRIB6;
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
TestPipeline::ObjectGeometry AddCube(std::vector<Vertex>& Vertices,
                                                     std::vector<uint32_t>& Indices,
                                                     const Vector3&        f3TopScale,
                                                     const Vector3&        f3BottomScale)
{
    auto BaseVertex = static_cast<uint32_t>(Vertices.size());

    Vertices.insert(Vertices.end(),
        {
            {HadamardProduct(Vector3(-1,-1,-1), f3BottomScale), Vector2(0,1)},
            {HadamardProduct(Vector3(-1,+1,-1), f3BottomScale), Vector2(0,0)},
            {HadamardProduct(Vector3(+1,+1,-1), f3BottomScale), Vector2(1,0)},
            {HadamardProduct(Vector3(+1,-1,-1), f3BottomScale), Vector2(1,1)},
            {HadamardProduct(Vector3(-1,-1,-1), f3BottomScale), Vector2(0,1)},
            {HadamardProduct(Vector3(-1,-1,+1), f3TopScale),    Vector2(0,0)},
            {HadamardProduct(Vector3(+1,-1,+1), f3TopScale),    Vector2(1,0)},
            {HadamardProduct(Vector3(+1,-1,-1), f3BottomScale), Vector2(1,1)},
            {HadamardProduct(Vector3(+1,-1,-1), f3BottomScale), Vector2(0,1)},
            {HadamardProduct(Vector3(+1,-1,+1), f3TopScale),    Vector2(1,1)},
            {HadamardProduct(Vector3(+1,+1,+1), f3TopScale),    Vector2(1,0)},
            {HadamardProduct(Vector3(+1,+1,-1), f3BottomScale), Vector2(0,0)},
            {HadamardProduct(Vector3(+1,+1,-1), f3BottomScale), Vector2(0,1)},
            {HadamardProduct(Vector3(+1,+1,+1), f3TopScale),    Vector2(0,0)},
            {HadamardProduct(Vector3(-1,+1,+1), f3TopScale),    Vector2(1,0)},
            {HadamardProduct(Vector3(-1,+1,-1), f3BottomScale), Vector2(1,1)},
            {HadamardProduct(Vector3(-1,+1,-1), f3BottomScale), Vector2(1,0)},
            {HadamardProduct(Vector3(-1,+1,+1), f3TopScale),    Vector2(0,0)},
            {HadamardProduct(Vector3(-1,-1,+1), f3TopScale),    Vector2(0,1)},
            {HadamardProduct(Vector3(-1,-1,-1), f3BottomScale), Vector2(1,1)},
            {HadamardProduct(Vector3(-1,-1,+1), f3TopScale),    Vector2(1,1)},
            {HadamardProduct(Vector3(+1,-1,+1), f3TopScale),    Vector2(0,1)},
            {HadamardProduct(Vector3(+1,+1,+1), f3TopScale),    Vector2(0,0)},
            {HadamardProduct(Vector3(-1,+1,+1), f3TopScale),    Vector2(1,0)}
        }
    );

    TestPipeline::ObjectGeometry Geometry;
    Geometry.FirstIndex = static_cast<uint32_t>(Indices.size());
    Indices.insert(Indices.end(),
        {
            2,0,1,    2,3,0,
            4,6,5,    4,7,6,
            8,10,9,   8,11,10,
            12,14,13, 12,15,14,
            16,18,17, 16,19,18,
            20,21,22, 20,22,23
        }
    );

    Geometry.NumIndices = static_cast<uint32_t>(Indices.size()) - Geometry.FirstIndex;
    for (uint32_t i = Geometry.FirstIndex; i < Geometry.FirstIndex + Geometry.NumIndices; ++i)
        Indices[i] += BaseVertex;
    return Geometry;
}

TestPipeline::TestPipeline(IDeviceContext& context,
                           IRenderDevice& device,
                           ISwapChain& swapChain,
                           ShaderFactory& shaderFactory,
                           Diligent::IPipelineResourceSignature& textureResourceSignature)
{
    CreatePipelineState(device, swapChain, shaderFactory, textureResourceSignature);
    CreateGeometryBuffers(context, device);
    CreateInstanceBuffer(context, device);
}

void TestPipeline::CreatePipelineState(IRenderDevice& device,
                                       ISwapChain& swapChain,
                                       ShaderFactory& shaderFactory,
                                       Diligent::IPipelineResourceSignature& textureResourceSignature)
{
    auto createInfo = GraphicsPipelineStateCreateInfo{};
    createInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    createInfo.PSODesc.Name = "Test PSO";

    auto signatures = &textureResourceSignature;
    createInfo.ppResourceSignatures = &signatures;
    createInfo.ResourceSignaturesCount = 1;

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
        // Per-vertex data - first buffer slot
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        LayoutElement{1, 0, 2, VT_FLOAT32, False},

        // Per-instance data - second buffer slot
        // We will use four attributes to encode instance-specific 4x4 transformation matrix
        LayoutElement{2, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
        LayoutElement{3, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
        LayoutElement{4, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
        LayoutElement{5, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
        LayoutElement{6, 1, 1, VT_UINT32,  False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
    };

    createInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems.data();
    createInfo.GraphicsPipeline.InputLayout.NumElements    = static_cast<uint32_t>(LayoutElems.size());

    device.CreateGraphicsPipelineState(createInfo, &m_pBindlessPSO);
    NC_ASSERT(m_pBindlessPSO, "Failed to create pipeline state object");
}

void TestPipeline::CreateGeometryBuffers(Diligent::IDeviceContext& context, Diligent::IRenderDevice& device)
{
    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;

    m_Geometries.emplace_back(AddCube(Vertices, Indices, Vector3(1, 1, 1), Vector3(1, 1, 1)));
    m_Geometries.emplace_back(AddCube(Vertices, Indices, Vector3(1, 1, 1), Vector3(1, 1, 1.f)));
    m_Geometries.emplace_back(AddCube(Vertices, Indices, Vector3(1, 1, 1), Vector3(1, 1, 1.f)));
    m_Geometries.emplace_back(AddCube(Vertices, Indices, Vector3(1, 1, 1), Vector3(1, 1, 1)));

    {
        BufferDesc VertBuffDesc;
        VertBuffDesc.Name      = "Geometry vertex buffer";
        VertBuffDesc.Usage     = USAGE_IMMUTABLE;
        VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
        VertBuffDesc.Size      = static_cast<Uint32>(sizeof(Vertex) * Vertices.size());
        BufferData VBData;
        VBData.pData    = Vertices.data();
        VBData.DataSize = VertBuffDesc.Size;
        device.CreateBuffer(VertBuffDesc, &VBData, &m_VertexBuffer);
    }

    {
        BufferDesc IndBuffDesc;
        IndBuffDesc.Name      = "Geometry index buffer";
        IndBuffDesc.Usage     = USAGE_IMMUTABLE;
        IndBuffDesc.BindFlags = BIND_INDEX_BUFFER;
        IndBuffDesc.Size      = static_cast<Uint32>(sizeof(Indices[0]) * Indices.size());
        BufferData IBData;
        IBData.pData    = Indices.data();
        IBData.DataSize = IndBuffDesc.Size;
        device.CreateBuffer(IndBuffDesc, &IBData, &m_IndexBuffer);
    }

    auto Barriers = std::array{
        StateTransitionDesc(m_VertexBuffer.RawPtr(), RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE),
        StateTransitionDesc(m_IndexBuffer.RawPtr(),  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_INDEX_BUFFER,  STATE_TRANSITION_FLAG_UPDATE_STATE)
    };

    context.TransitionResourceStates(static_cast<uint32_t>(Barriers.size()), Barriers.data());
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
    std::uniform_int_distribution<uint32_t> tex_distr(0, 19 - 1); // todo: not great, max is def larger than available
    std::uniform_int_distribution<uint32_t> geom_type_distr(0, static_cast<uint32_t>(m_Geometries.size()) - 1);

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

void TestPipeline::Render(Diligent::IDeviceContext& context)
{
    auto pBuffs = std::array{m_VertexBuffer.RawPtr(), m_InstanceBuffer.RawPtr()};
    context.SetVertexBuffers(0, static_cast<uint32_t>(pBuffs.size()), pBuffs.data(), nullptr, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
    context.SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    context.SetPipelineState(m_pBindlessPSO);

    auto NumObjects = m_GridSize * m_GridSize * m_GridSize;
    for (int i = 0; i < NumObjects; ++i)
    {
        const auto& Geometry = m_Geometries[m_GeometryType[i]];

        DrawIndexedAttribs DrawAttrs;
        DrawAttrs.IndexType             = VT_UINT32;
        DrawAttrs.NumIndices            = Geometry.NumIndices;
        DrawAttrs.FirstIndexLocation    = Geometry.FirstIndex;
        DrawAttrs.FirstInstanceLocation = static_cast<uint32_t>(i);
        // Verify the state of vertex and index buffers
        // Also use DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT flag to inform the engine that
        // none of the dynamic buffers have changed since the last draw command.
        DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL | DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT;
        context.DrawIndexed(DrawAttrs);
    }
}
} // namespace nc::graphics

#include "TestSetup.h"
#include "ncengine/graphics/Camera.h"

#include "Graphics/GraphicsEngine/interface/PipelineState.h"

#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"
#include "Graphics/GraphicsTools/interface/ShaderMacroHelper.hpp"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"
#include <random>

using namespace Diligent;


namespace
{
struct Vertex
{
    float3 pos;
    float2 uv;
};
}

namespace nc::graphics
{
TestSetup::ObjectGeometry AddCube(std::vector<Vertex>& Vertices,
                                                     std::vector<Uint32>& Indices,
                                                     const float3&        f3TopScale,
                                                     const float3&        f3BottomScale)
{
    // Cube vertices

    //      (-1,+1,+1)________________(+1,+1,+1)
    //               /|              /|
    //              / |             / |
    //             /  |            /  |
    //            /   |           /   |
    //(-1,-1,+1) /____|__________/(+1,-1,+1)
    //           |    |__________|____|
    //           |   /(-1,+1,-1) |    /(+1,+1,-1)
    //           |  /            |   /
    //           | /             |  /
    //           |/              | /
    //           /_______________|/
    //        (-1,-1,-1)       (+1,-1,-1)
    //

    auto BaseVertex = static_cast<Uint32>(Vertices.size());

    // clang-format off
    Vertices.insert(Vertices.end(),
        {
            {float3(-1,-1,-1) * f3BottomScale, float2(0,1)},
            {float3(-1,+1,-1) * f3BottomScale, float2(0,0)},
            {float3(+1,+1,-1) * f3BottomScale, float2(1,0)},
            {float3(+1,-1,-1) * f3BottomScale, float2(1,1)},

            {float3(-1,-1,-1) * f3BottomScale, float2(0,1)},
            {float3(-1,-1,+1) * f3TopScale,    float2(0,0)},
            {float3(+1,-1,+1) * f3TopScale,    float2(1,0)},
            {float3(+1,-1,-1) * f3BottomScale, float2(1,1)},

            {float3(+1,-1,-1) * f3BottomScale, float2(0,1)},
            {float3(+1,-1,+1) * f3TopScale,    float2(1,1)},
            {float3(+1,+1,+1) * f3TopScale,    float2(1,0)},
            {float3(+1,+1,-1) * f3BottomScale, float2(0,0)},

            {float3(+1,+1,-1) * f3BottomScale, float2(0,1)},
            {float3(+1,+1,+1) * f3TopScale,    float2(0,0)},
            {float3(-1,+1,+1) * f3TopScale,    float2(1,0)},
            {float3(-1,+1,-1) * f3BottomScale, float2(1,1)},

            {float3(-1,+1,-1) * f3BottomScale, float2(1,0)},
            {float3(-1,+1,+1) * f3TopScale,    float2(0,0)},
            {float3(-1,-1,+1) * f3TopScale,    float2(0,1)},
            {float3(-1,-1,-1) * f3BottomScale, float2(1,1)},

            {float3(-1,-1,+1) * f3TopScale,    float2(1,1)},
            {float3(+1,-1,+1) * f3TopScale,    float2(0,1)},
            {float3(+1,+1,+1) * f3TopScale,    float2(0,0)},
            {float3(-1,+1,+1) * f3TopScale,    float2(1,0)}
        }
    );
    // clang-format on

    TestSetup::ObjectGeometry Geometry;
    Geometry.FirstIndex = static_cast<Uint32>(Indices.size());
    // clang-format off
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
    // clang-format on

    Geometry.NumIndices = static_cast<Uint32>(Indices.size()) - Geometry.FirstIndex;
    for (Uint32 i = Geometry.FirstIndex; i < Geometry.FirstIndex + Geometry.NumIndices; ++i)
        Indices[i] += BaseVertex;
    return Geometry;
}

TestSetup::TestSetup(IDeviceContext* context, IRenderDevice* device, ISwapChain* swapChain)
{
    CreatePipelineState(context, device, swapChain);
    CreateGeometryBuffers(context, device);
    CreateInstanceBuffer(context, device);
}

void TestSetup::CreatePipelineState(IDeviceContext* context, IRenderDevice* device, ISwapChain* swapChain)
{
    #if EXPLICITLY_LOAD_ENGINE_VK_DLL
        auto* GetEngineFactoryVk = LoadGraphicsEngineVk();
    #endif
    auto* engineFactory = GetEngineFactoryVk();

    // #if ENGINE_DLL
    //     auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
    // #endif

    // EngineD3D12CreateInfo engineCI;
    // auto* engineFactory = GetEngineFactoryD3D12();


    // Pipeline state object encompasses configuration of all GPU stages

    GraphicsPipelineStateCreateInfo PSOCreateInfo;
    PipelineStateDesc&              PSODesc = PSOCreateInfo.PSODesc;

    // This is a graphics pipeline
    PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSODesc.Name = "Cube PSO";

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets             = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0]                = swapChain->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat                    = swapChain->GetDesc().DepthBufferFormat;
    // PSOCreateInfo.GraphicsPipeline.DSVFormat                    = Diligent::TEX_FORMAT_D32_FLOAT;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;

    // Pack matrices in row-major order
    ShaderCI.CompileFlags = SHADER_COMPILE_FLAG_PACK_MATRIX_ROW_MAJOR;

    // Create a shader source stream factory to load shaders from files.
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    engineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
    // Create a vertex shader
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Cube VS";
        ShaderCI.FilePath        = "cube.vsh";
        device->CreateShader(ShaderCI, &pVS);
        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        CreateUniformBuffer(device, sizeof(float4x4) * 2, "VS constants CB", &m_VSConstants);
        StateTransitionDesc Barrier{m_VSConstants, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE};
        context->TransitionResourceStates(1, &Barrier);
    }

    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS, pBindlessPS;
    {
        ShaderMacroHelper Macros;
        // Presentation engine always expects input in gamma space. Normally, pixel shader output is
        // converted from linear to gamma space by the GPU. However, some platforms (e.g. Android in GLES mode,
        // or Emscripten in WebGL mode) do not support gamma-correction. In this case the application
        // has to do the conversion manually.
        Macros.Add("CONVERT_PS_OUTPUT_TO_GAMMA", m_ConvertPSOutputToGamma);
        ShaderCI.Macros = Macros;

        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Cube PS";
        ShaderCI.FilePath        = "cube.psh";
        device->CreateShader(ShaderCI, &pPS);

        if (device->GetDeviceInfo().Features.BindlessResources)
        {
            Macros.Add("BINDLESS", 1);
            Macros.Add("NUM_TEXTURES", NumTextures);
            ShaderCI.Macros = Macros;
            device->CreateShader(ShaderCI, &pBindlessPS);
            ShaderCI.Macros = {};
        }
    }

    // clang-format off
    // Define vertex shader input layout
    // This tutorial uses two types of input: per-vertex data and per-instance data.
    LayoutElement LayoutElems[] =
    {
        // Per-vertex data - first buffer slot
        // Attribute 0 - vertex position
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        // Attribute 1 - texture coordinates
        LayoutElement{1, 0, 2, VT_FLOAT32, False},

        // Per-instance data - second buffer slot
        // We will use four attributes to encode instance-specific 4x4 transformation matrix
        // Attribute 2 - first row
        LayoutElement{2, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
        // Attribute 3 - second row
        LayoutElement{3, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
        // Attribute 4 - third row
        LayoutElement{4, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
        // Attribute 5 - fourth row
        LayoutElement{5, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
        // Attribute 6 - texture array index
        LayoutElement{6, 1, 1, VT_UINT32,  False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
    };
    // clang-format on

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements    = _countof(LayoutElems);

    // Define variable type that will be used by default
    PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    // clang-format off
    // Shader variables should typically be mutable, which means they are expected
    // to change on a per-instance basis
    ShaderResourceVariableDesc Vars[] = 
    {
        {SHADER_TYPE_PIXEL, "g_Texture", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
    };
    // clang-format on
    PSODesc.ResourceLayout.Variables    = Vars;
    PSODesc.ResourceLayout.NumVariables = _countof(Vars);

    // clang-format off
    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, 
        TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
    };
    ImmutableSamplerDesc ImtblSamplers[] = 
    {
        {SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc}
    };
    // clang-format on
    PSODesc.ResourceLayout.ImmutableSamplers    = ImtblSamplers;
    PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);

    device->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);

    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
    // never change and are bound directly to the pipeline state object.
    m_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_VSConstants);

    // Since we are using mutable variable, we must create a shader resource binding object
    // http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/

    for (Uint32 i = 0; i < NumTextures; ++i)
    {
        m_pPSO->CreateShaderResourceBinding(&m_SRB[i], true);
    }

    if (pBindlessPS)
    {
        PSOCreateInfo.pPS = pBindlessPS;
        device->CreateGraphicsPipelineState(PSOCreateInfo, &m_pBindlessPSO);
        m_pBindlessPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_VSConstants);
        m_pBindlessPSO->CreateShaderResourceBinding(&m_BindlessSRB, true);
        m_BindlessMode = true;
    }
}

void TestSetup::CreateGeometryBuffers(Diligent::IDeviceContext* context, Diligent::IRenderDevice* device)
{
    std::vector<Vertex> Vertices;
    std::vector<Uint32> Indices;

    m_Geometries.emplace_back(AddCube(Vertices, Indices, float3(1, 1, 1), float3(1, 1, 1)));
    m_Geometries.emplace_back(AddCube(Vertices, Indices, float3(1, 1, 1), float3(0.5f, 0.5f, 1.f)));
    m_Geometries.emplace_back(AddCube(Vertices, Indices, float3(0.5f, 1, 1), float3(1, 0.5f, 1.f)));
    m_Geometries.emplace_back(AddCube(Vertices, Indices, float3(1, 1, 1), float3(1, 0.5f, 0.5f)));


    {
        BufferDesc VertBuffDesc;
        VertBuffDesc.Name      = "Geometry vertex buffer";
        VertBuffDesc.Usage     = USAGE_IMMUTABLE;
        VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
        VertBuffDesc.Size      = static_cast<Uint32>(sizeof(Vertex) * Vertices.size());
        BufferData VBData;
        VBData.pData    = Vertices.data();
        VBData.DataSize = VertBuffDesc.Size;
        device->CreateBuffer(VertBuffDesc, &VBData, &m_VertexBuffer);
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
        device->CreateBuffer(IndBuffDesc, &IBData, &m_IndexBuffer);
    }
    // clang-format off
    StateTransitionDesc Barriers[2] =
    {
        {m_VertexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE},
        {m_IndexBuffer,  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_INDEX_BUFFER,  STATE_TRANSITION_FLAG_UPDATE_STATE}
    };
    // clang-format on
    context->TransitionResourceStates(_countof(Barriers), Barriers);
}

void TestSetup::CreateInstanceBuffer(IDeviceContext* context, IRenderDevice* device)
{
    // Create instance data buffer that will store transformation matrices
    BufferDesc InstBuffDesc;
    InstBuffDesc.Name = "Instance data buffer";
    // Use default usage as this buffer will only be updated when grid size changes
    InstBuffDesc.Usage     = USAGE_DEFAULT;
    InstBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
    InstBuffDesc.Size      = sizeof(InstanceData) * MaxInstances;
    device->CreateBuffer(InstBuffDesc, nullptr, &m_InstanceBuffer);
    PopulateInstanceBuffer(context);
}

void TestSetup::PopulateInstanceBuffer(IDeviceContext* context)
{
    // Populate instance data buffer
    const auto zGridSize = static_cast<size_t>(m_GridSize);
    m_InstanceData.resize(zGridSize * zGridSize * zGridSize);
    m_GeometryType.resize(zGridSize * zGridSize * zGridSize);

    float fGridSize = static_cast<float>(m_GridSize);

    std::mt19937 gen; // Standard mersenne_twister_engine. Use default seed
                      // to generate consistent distribution.

    std::uniform_real_distribution<float> scale_distr(0.3f, 1.0f);
    std::uniform_real_distribution<float> offset_distr(-0.15f, +0.15f);
    std::uniform_real_distribution<float> rot_distr(-PI_F, +PI_F);
    std::uniform_int_distribution<Uint32> tex_distr(0, NumTextures - 1);
    std::uniform_int_distribution<Uint32> geom_type_distr(0, static_cast<Uint32>(m_Geometries.size()) - 1);

    float BaseScale = 0.6f / fGridSize;
    int   instId    = 0;
    for (int x = 0; x < m_GridSize; ++x)
    {
        for (int y = 0; y < m_GridSize; ++y)
        {
            for (int z = 0; z < m_GridSize; ++z)
            {
                // Add random offset from central position in the grid
                float xOffset = 2.f * (x + 0.5f + offset_distr(gen)) / fGridSize - 1.f;
                float yOffset = 2.f * (y + 0.5f + offset_distr(gen)) / fGridSize - 1.f;
                float zOffset = 2.f * (z + 0.5f + offset_distr(gen)) / fGridSize - 1.f;
                // Random scale
                float scale = BaseScale * scale_distr(gen);
                // Random rotation
                float4x4 rotation = float4x4::RotationX(rot_distr(gen));
                rotation *= float4x4::RotationY(rot_distr(gen));
                rotation *= float4x4::RotationZ(rot_distr(gen));
                // Combine rotation, scale and translation
                float4x4 matrix   = rotation * float4x4::Scale(scale, scale, scale) * float4x4::Translation(xOffset, yOffset, zOffset);
                auto&    CurrInst = m_InstanceData[instId];
                CurrInst.Matrix   = matrix;
                // Texture array index
                CurrInst.TextureInd = tex_distr(gen);

                m_GeometryType[instId++] = geom_type_distr(gen);
            }
        }
    }
    // Update instance data buffer
    Uint32 DataSize = static_cast<Uint32>(sizeof(InstanceData) * m_InstanceData.size());
    context->UpdateBuffer(m_InstanceBuffer, 0, DataSize, m_InstanceData.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    StateTransitionDesc Barrier(m_InstanceBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
    context->TransitionResourceStates(1, &Barrier);
}

void TestSetup::Render(Diligent::IDeviceContext* context, Diligent::IRenderDevice*, Diligent::ISwapChain* swapChain, Camera* camera)
{
    auto* pRTV = swapChain->GetCurrentBackBufferRTV();
    auto* pDSV = swapChain->GetDepthBufferDSV();

    context->SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);


    // Clear the back buffer
    float4 ClearColor = {0.350f, 0.350f, 0.350f, 1.0f};
    // if (m_ConvertPSOutputToGamma)
    // {
    //     // If manual gamma correction is required, we need to clear the render target with sRGB color
    //     ClearColor = LinearToSRGB(ClearColor);
    // }
    context->ClearRenderTarget(pRTV, ClearColor.Data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    context->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    // didn't see this in sample...

    {
        // Map the buffer and write current world-view-projection matrix
        auto camView = camera->ViewMatrix();
        auto camProj = camera->ProjectionMatrix();
        auto camViewProj = DirectX::XMMatrixMultiply(camView, camProj);
        auto dst = DirectX::XMFLOAT4X4{};
        DirectX::XMStoreFloat4x4(&dst, camViewProj);

        auto viewProjMatrix = float4x4{
            dst.m[0][0], dst.m[0][1], dst.m[0][2], dst.m[0][3],
            dst.m[1][0], dst.m[1][1], dst.m[1][2], dst.m[1][3],
            dst.m[2][0], dst.m[2][1], dst.m[2][2], dst.m[2][3],
            dst.m[3][0], dst.m[3][1], dst.m[3][2], dst.m[3][3]
        };

        // float4x4 View = float4x4::RotationX(-0.6f) * float4x4::Translation(0.f, 0.f, 4.0f);
        // auto SrfPreTransform = GetSurfacePretransformMatrix(float3{0, 0, 1});
        // auto Proj = GetAdjustedProjectionMatrix(PI_F / 4.0f, 0.1f, 100.f);
        // viewProjMatrix = View * SrfPreTransform * Proj;
        auto rotationMatrix = float4x4::RotationY(1.0f) * float4x4::RotationX(-0.25f);


        MapHelper<float4x4> CBConstants(context, m_VSConstants, MAP_WRITE, MAP_FLAG_DISCARD);
        CBConstants[0] = viewProjMatrix;
        CBConstants[1] = rotationMatrix;
    }

    // Bind vertex, instance and index buffers
    IBuffer* pBuffs[] = {m_VertexBuffer, m_InstanceBuffer};
    context->SetVertexBuffers(0, _countof(pBuffs), pBuffs, nullptr, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
    context->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    // Set the pipeline state
    context->SetPipelineState(m_BindlessMode ? m_pBindlessPSO : m_pPSO);
    // Commit shader resources. RESOURCE_STATE_TRANSITION_MODE_TRANSITION mode
    // makes sure that resources are transitioned to required states.
    if (m_BindlessMode)
        context->CommitShaderResources(m_BindlessSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        // context->CommitShaderResources(m_BindlessSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    auto NumObjects = m_GridSize * m_GridSize * m_GridSize;
    for (int i = 0; i < NumObjects; ++i)
    {
        if (!m_BindlessMode)
        {
            auto TexId = m_InstanceData[i].TextureInd;
            context->CommitShaderResources(m_SRB[TexId], RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        }

        const auto& Geometry = m_Geometries[m_GeometryType[i]];

        DrawIndexedAttribs DrawAttrs;
        DrawAttrs.IndexType             = VT_UINT32;
        DrawAttrs.NumIndices            = Geometry.NumIndices;
        DrawAttrs.FirstIndexLocation    = Geometry.FirstIndex;
        DrawAttrs.FirstInstanceLocation = static_cast<Uint32>(i);
        // Verify the state of vertex and index buffers
        // Also use DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT flag to inform the engine that
        // none of the dynamic buffers have changed since the last draw command.
        DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL | DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT;
        context->DrawIndexed(DrawAttrs);
    }


    swapChain->Present();
}

} // namespace nc::graphics

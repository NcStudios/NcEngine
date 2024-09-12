#include "NcGraphics3Impl.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/utility/Log.h"
#include "ncengine/window/Window.h"
#include "config/Config.h"
#include "time/Time.h"
#include "input/Input.h"

#define GLFW_EXPOSE_NATIVE_WIN32

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "imgui/imgui.h"
#include "optick.h"
#include "DirectXMath.h"

// Diligent
#include "MapHelper.hpp"
#include "GraphicsUtilities.h"
#include "TextureUtilities.h"

namespace
{
    struct NcGraphicsStub : nc::graphics::NcGraphics
    {
        NcGraphicsStub(nc::Registry*)
        {
            // client app may still make imgui calls (font loading, etc.), so we need a context
            ImGui::CreateContext();
        }

        ~NcGraphicsStub() noexcept
        {
            ImGui::DestroyContext();
        }

        void OnBuildTaskGraph(nc::task::UpdateTasks& update, nc::task::RenderTasks& render)
        {
            update.Add(
                nc::update_task_id::ParticleEmitterUpdate,
                "ParticleEmitterUpdate(stub)",
                []{}
            );

            update.Add(
                nc::update_task_id::ParticleEmitterSync,
                "ParticleEmitterSync(stub)",
                []{},
                {nc::update_task_id::CommitStagedChanges}
            );

            render.Add(
                nc::render_task_id::Render,
                "Render(stub)",
                []{}
            );
        }

        void SetCamera(nc::graphics::Camera*) noexcept override {}
        auto GetCamera() noexcept -> nc::graphics::Camera* override { return nullptr; }
        void SetUi(nc::ui::IUI*) noexcept override {}
        bool IsUiHovered() const noexcept override { return false; }
        void SetSkybox(const std::string&) override {}
        void ClearEnvironment() override {}
    };

    DirectX::XMMATRIX GetSurfacePretransformMatrixInternal(const DirectX::XMVECTOR& f3CameraViewAxis,
                                                           const Diligent::ISwapChain* swapchain)
    {
        const auto& SCDesc = swapchain->GetDesc();
        switch (SCDesc.PreTransform)
        {
            case Diligent::SURFACE_TRANSFORM_ROTATE_90:
                // The image content is rotated 90 degrees clockwise.
                return DirectX::XMMatrixRotationAxis(f3CameraViewAxis, -DirectX::XM_PI / 2.f);

            case Diligent::SURFACE_TRANSFORM_ROTATE_180:
                // The image content is rotated 180 degrees clockwise.
                return DirectX::XMMatrixRotationAxis(f3CameraViewAxis, -DirectX::XM_PI);

            case Diligent::SURFACE_TRANSFORM_ROTATE_270:
                // The image content is rotated 270 degrees clockwise.
                return DirectX::XMMatrixRotationAxis(f3CameraViewAxis, -DirectX::XM_PI * 3.f / 2.f);

            case Diligent::SURFACE_TRANSFORM_OPTIMAL:
                UNEXPECTED("SURFACE_TRANSFORM_OPTIMAL is only valid as parameter during swap chain initialization.");
                return DirectX::XMMatrixIdentity();

            case Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR:
            case Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90:
            case Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180:
            case Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270:
                UNEXPECTED("Mirror transforms are not supported");
                return DirectX::XMMatrixIdentity();

            default:
                return DirectX::XMMatrixIdentity();
        }
    }

    DirectX::XMMATRIX GetAdjustedProjectionMatrix(float FOV,
                                                  float NearPlane,
                                                  float FarPlane,
                                                  const Diligent::ISwapChain* swapchain)
    {
        const auto& SCDesc = swapchain->GetDesc();

        float AspectRatio = static_cast<float>(SCDesc.Width) / static_cast<float>(SCDesc.Height);
        float XScale, YScale;
        if (SCDesc.PreTransform == Diligent::SURFACE_TRANSFORM_ROTATE_90 ||
            SCDesc.PreTransform == Diligent::SURFACE_TRANSFORM_ROTATE_270 ||
            SCDesc.PreTransform == Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90 ||
            SCDesc.PreTransform == Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270)
        {
            // When the screen is rotated, vertical FOV becomes horizontal FOV
            XScale = 1.f / std::tan(FOV / 2.f);
            // Aspect ratio is inversed
            YScale = XScale * AspectRatio;
        }
        else
        {
            YScale = 1.f / std::tan(FOV / 2.f);
            XScale = YScale / AspectRatio;
        }

        auto perspective =
            DirectX::XMMatrixPerspectiveFovRH(FOV, 1.0, NearPlane, FarPlane) *
            DirectX::XMMatrixScaling(XScale, YScale, 1.0f);
        return perspective;
    }
} // anonymous namespace

namespace nc::graphics
{
#ifdef NC_USE_DILIGENT
    auto BuildGraphicsModule(const config::ProjectSettings& projectSettings,
                             const config::GraphicsSettings& graphicsSettings,
                             const config::MemorySettings& memorySettings,
                             ModuleProvider modules,
                             Registry* registry,
                             SystemEvents& events) -> std::unique_ptr<NcGraphics>
    {
        (void)projectSettings;
        (void)memorySettings;
        (void)events;

        if (graphicsSettings.enabled)
        {
            auto ncAsset = modules.Get<asset::NcAsset>();
            auto ncWindow = modules.Get<window::NcWindow>();
            NC_ASSERT(ncAsset, "NcGraphics requires NcAsset to be registered before it.");
            NC_ASSERT(ncWindow, "NcGraphics requires NcWindow to be registered before it.");
            NC_ASSERT(modules.Get<NcScene>(), "NcGraphics requires NcScene to be registered before it.");
            
            NC_LOG_TRACE("Building NcGraphics module");
            return std::make_unique<NcGraphics3Impl>(graphicsSettings, memorySettings, registry, modules, events, *ncWindow);
        }

        NC_LOG_TRACE("Graphics disabled - building NcGraphics stub");
        return std::make_unique<NcGraphicsStub>(registry);
    }
#endif

    NcGraphics3Impl::NcGraphics3Impl(const config::GraphicsSettings& graphicsSettings,
                                   const config::MemorySettings& memorySettings,
                                   Registry* registry,
                                   ModuleProvider modules,
                                   SystemEvents& events,
                                   window::NcWindow& window)
        : m_registry{registry},
          m_onResizeConnection{window.OnResize().Connect(this, &NcGraphics3Impl::OnResize)},
          m_elapsedTime{0.0f}
    {
        (void)graphicsSettings;
        (void)memorySettings;
        (void)modules;
        (void)events;

        using namespace Diligent;

        ImGui::CreateContext();

        /* Initialize Diligent Engine */
        SwapChainDesc SCDesc;
        auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
        EngineD3D12CreateInfo engineCI;
        auto* pFactoryD3D12 = GetEngineFactoryD3D12();
        pFactoryD3D12->CreateDeviceAndContextsD3D12(engineCI, &m_pDevice, &m_pImmediateContext);
        auto win32Handle = glfwGetWin32Window(window.GetWindowHandle());
        Win32NativeWindow win32Window{win32Handle};
        pFactoryD3D12->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, win32Window, &m_pSwapChain);
        
        /* Reusable shaders setup */
        auto defaultShadersPath = nc::config::GetAssetSettings().shadersPath;

        ShaderCreateInfo shaderCI;
        shaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        shaderCI.Desc.UseCombinedTextureSamplers = true;

        RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
        pFactoryD3D12->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
        shaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;

        /* TUTORIAL 01 - Triangle */
        SetupTriangle01(defaultShadersPath, shaderCI);

        /* Constant buffer holding MVP matrix */
        CreateUniformBuffer(m_pDevice, sizeof(DirectX::XMMATRIX), "VS Constants CB", &m_VSConstants);

        /* TUTORIAL 02 - Cube */
        SetupCube02(defaultShadersPath, shaderCI);
        
        /* TUTORIAL 03 - Texture */
        SetupTexturedCube03(defaultShadersPath, shaderCI);
    }

    void NcGraphics3Impl::SetupTriangle01(std::string_view defaultShadersPath, Diligent::ShaderCreateInfo shaderCI)
    {
        using namespace Diligent;

        /* Create Pipeline */
        GraphicsPipelineStateCreateInfo psoCI;
        psoCI.PSODesc.Name = "Triangle PSO";
        psoCI.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

        psoCI.GraphicsPipeline.NumRenderTargets             = 1;
        psoCI.GraphicsPipeline.RTVFormats[0]                = m_pSwapChain->GetDesc().ColorBufferFormat;
        psoCI.GraphicsPipeline.DSVFormat                    = m_pSwapChain->GetDesc().DepthBufferFormat;
        psoCI.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        psoCI.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_NONE;
        psoCI.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;

        auto vertPath = (std::filesystem::path(defaultShadersPath) / std::filesystem::path("Triangle.vsh")).string();
        auto fragPath = (std::filesystem::path(defaultShadersPath) / std::filesystem::path("Triangle.psh")).string();

        RefCntAutoPtr<IShader> pVS;
        {
            shaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
            shaderCI.EntryPoint      = "main";
            shaderCI.Desc.Name       = "Triangle Vertex";
            shaderCI.FilePath        = vertPath.data();
            shaderCI.SourceLength    = vertPath.size();
            m_pDevice->CreateShader(shaderCI, &pVS);
        }

        RefCntAutoPtr<IShader> pPS;
        {
            shaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
            shaderCI.EntryPoint      = "main";
            shaderCI.Desc.Name       = "Triangle Fragment";
            shaderCI.FilePath        = fragPath.data();
            shaderCI.SourceLength    = fragPath.size();
            m_pDevice->CreateShader(shaderCI, &pPS);
        }

        psoCI.pVS = pVS;
        psoCI.pPS = pPS;
        m_pDevice->CreateGraphicsPipelineState(psoCI, &m_pPSO);
    }

    void NcGraphics3Impl::SetupCube02(std::string_view defaultShadersPath, Diligent::ShaderCreateInfo shaderCI)
    {
        using namespace Diligent;

        /* Create Pipeline */
        GraphicsPipelineStateCreateInfo psoCubeCI;
        psoCubeCI.PSODesc.Name = "Cube PSO";
        psoCubeCI.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

        psoCubeCI.GraphicsPipeline.NumRenderTargets             = 1;
        psoCubeCI.GraphicsPipeline.RTVFormats[0]                = m_pSwapChain->GetDesc().ColorBufferFormat;
        psoCubeCI.GraphicsPipeline.DSVFormat                    = m_pSwapChain->GetDesc().DepthBufferFormat;
        psoCubeCI.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;
        psoCubeCI.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;

        /* Shaders */
        auto vertPathCube = (std::filesystem::path(defaultShadersPath) / std::filesystem::path("Cube.vsh")).string();
        auto fragPathCube = (std::filesystem::path(defaultShadersPath) / std::filesystem::path("Cube.psh")).string();

        RefCntAutoPtr<IShader> pVSCube;
        {
            shaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
            shaderCI.EntryPoint      = "main";
            shaderCI.Desc.Name       = "Cube Vertex";
            shaderCI.FilePath        = vertPathCube.data();
            shaderCI.SourceLength    = vertPathCube.size();
            shaderCI.CompileFlags    = SHADER_COMPILE_FLAG_PACK_MATRIX_ROW_MAJOR;
            m_pDevice->CreateShader(shaderCI, &pVSCube);
        }

        RefCntAutoPtr<IShader> pPSCube;
        {
            shaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
            shaderCI.EntryPoint      = "main";
            shaderCI.Desc.Name       = "Cube Fragment";
            shaderCI.FilePath        = fragPathCube.data();
            shaderCI.SourceLength    = fragPathCube.size();
            shaderCI.CompileFlags    = SHADER_COMPILE_FLAG_PACK_MATRIX_ROW_MAJOR;
            m_pDevice->CreateShader(shaderCI, &pPSCube);
        }

        psoCubeCI.pVS = pVSCube;
        psoCubeCI.pPS = pPSCube;

        psoCubeCI.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        /* Input layout for vertices */
        std::array<LayoutElement, 2> layoutElems =
        {
            LayoutElement{0, 0, 3, VT_FLOAT32, false}, // Position, Attribute 0
            LayoutElement{1, 0, 4, VT_FLOAT32, false}  // Color, Attribute 1
        };

        psoCubeCI.GraphicsPipeline.InputLayout.LayoutElements = layoutElems.data();
        psoCubeCI.GraphicsPipeline.InputLayout.NumElements    = static_cast<uint32_t>(layoutElems.size());

        m_pDevice->CreateGraphicsPipelineState(psoCubeCI, &m_pPSOCube);

        /* Resources */
        m_pPSOCube->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_VSConstants);

        /* Create a shader resource binding object and bind all static resources in it */
        m_pPSOCube->CreateShaderResourceBinding(&m_pCubeSRB, true);

        struct DiligentVertexType
        {
            DirectX::XMFLOAT3 pos;
            DirectX::XMFLOAT4 color;
        };

        /* Create vertex buffer */
        std::array<DiligentVertexType, 8> cubeVerts = 
        {{
            {DirectX::XMFLOAT3(-1,-1,-1), DirectX::XMFLOAT4(1,0,0,1)},
            {DirectX::XMFLOAT3(-1,+1,-1), DirectX::XMFLOAT4(0,1,0,1)},
            {DirectX::XMFLOAT3(+1,+1,-1), DirectX::XMFLOAT4(0,0,1,1)},
            {DirectX::XMFLOAT3(+1,-1,-1), DirectX::XMFLOAT4(1,1,1,1)},

            {DirectX::XMFLOAT3(-1,-1,+1), DirectX::XMFLOAT4(1,1,0,1)},
            {DirectX::XMFLOAT3(-1,+1,+1), DirectX::XMFLOAT4(0,1,1,1)},
            {DirectX::XMFLOAT3(+1,+1,+1), DirectX::XMFLOAT4(1,0,1,1)},
            {DirectX::XMFLOAT3(+1,-1,+1), DirectX::XMFLOAT4(0.2f,0.2f,0.2f,1)}
        }};

        BufferDesc vertexBufferDesc;
        vertexBufferDesc.Name = "Cube Vertex Buffer";
        vertexBufferDesc.Usage = USAGE_IMMUTABLE;
        vertexBufferDesc.BindFlags = BIND_VERTEX_BUFFER;
        vertexBufferDesc.Size = sizeof(cubeVerts);
        BufferData vertexBufferData;
        vertexBufferData.pData = cubeVerts.data();
        vertexBufferData.DataSize = sizeof(cubeVerts);
        m_pDevice->CreateBuffer(vertexBufferDesc, &vertexBufferData, &m_pCubeVertexBuffer);

        /* Create index buffer */
        constexpr std::array<uint32_t, 36> cubeIndices =
        {
            2,0,1, 2,3,0,
            4,6,5, 4,7,6,
            0,7,4, 0,3,7,
            1,0,4, 1,4,5,
            1,5,2, 5,6,2,
            3,6,7, 3,2,6
        };

        BufferDesc indexBufferDesc;
        indexBufferDesc.Name = "Cube Index Buffer";
        indexBufferDesc.Usage = USAGE_IMMUTABLE;
        indexBufferDesc.BindFlags = BIND_INDEX_BUFFER;
        indexBufferDesc.Size = sizeof(cubeIndices);
        BufferData indexBufferData;
        indexBufferData.pData = cubeIndices.data();
        indexBufferData.DataSize = sizeof(cubeIndices);
        m_pDevice->CreateBuffer(indexBufferDesc, &indexBufferData, &m_pCubeIndexBuffer);
    }

    void NcGraphics3Impl::SetupTexturedCube03(std::string_view defaultShadersPath, Diligent::ShaderCreateInfo shaderCI)
    {
        using namespace Diligent;

        /* Create Pipeline */
        GraphicsPipelineStateCreateInfo psoTextCubeCI;
        psoTextCubeCI.PSODesc.Name = "Texture Cube PSO";
        psoTextCubeCI.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

        psoTextCubeCI.GraphicsPipeline.NumRenderTargets             = 1;
        psoTextCubeCI.GraphicsPipeline.RTVFormats[0]                = m_pSwapChain->GetDesc().ColorBufferFormat;
        psoTextCubeCI.GraphicsPipeline.DSVFormat                    = m_pSwapChain->GetDesc().DepthBufferFormat;
        psoTextCubeCI.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;
        psoTextCubeCI.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;

        /* Shaders */
        auto vertPathTextCube = (std::filesystem::path(defaultShadersPath) / std::filesystem::path("TexturedCube.vsh")).string();
        auto fragPathTextCube = (std::filesystem::path(defaultShadersPath) / std::filesystem::path("TexturedCube.psh")).string();

        RefCntAutoPtr<IShader> pVSTextCube;
        {
            shaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
            shaderCI.EntryPoint      = "main";
            shaderCI.Desc.Name       = "Textured Cube Vertex";
            shaderCI.FilePath        = vertPathTextCube.data();
            shaderCI.SourceLength    = vertPathTextCube.size();
            shaderCI.CompileFlags    = SHADER_COMPILE_FLAG_PACK_MATRIX_ROW_MAJOR;
            m_pDevice->CreateShader(shaderCI, &pVSTextCube);
        }

        RefCntAutoPtr<IShader> pPSTextCube;
        {
            shaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
            shaderCI.EntryPoint      = "main";
            shaderCI.Desc.Name       = "Cube Fragment";
            shaderCI.FilePath        = fragPathTextCube.data();
            shaderCI.SourceLength    = fragPathTextCube.size();
            shaderCI.CompileFlags    = SHADER_COMPILE_FLAG_PACK_MATRIX_ROW_MAJOR;
            m_pDevice->CreateShader(shaderCI, &pPSTextCube);
        }

        psoTextCubeCI.pVS = pVSTextCube;
        psoTextCubeCI.pPS = pPSTextCube;

        std::array<LayoutElement, 2> layoutElems 
        {
            LayoutElement{0, 0, 3, VT_FLOAT32, false}, // ATTR 0, Vertex Pos
            LayoutElement{1, 0, 2, VT_FLOAT32, false}  // ATTR 1, UV
        };

        psoTextCubeCI.GraphicsPipeline.InputLayout.LayoutElements = layoutElems.data();
        psoTextCubeCI.GraphicsPipeline.InputLayout.NumElements = static_cast<uint32_t>(layoutElems.size());
        psoTextCubeCI.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        /* Create mutable texture SRV. Mutable textures are expected to be changed per-instance. */
        std::array<ShaderResourceVariableDesc, 1> shaderVariables
        {
            {{SHADER_TYPE_PIXEL, "g_Texture", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}}
        };

        psoTextCubeCI.PSODesc.ResourceLayout.Variables = shaderVariables.data();
        psoTextCubeCI.PSODesc.ResourceLayout.NumVariables = static_cast<uint32_t>(shaderVariables.size());

        /* Create immutable sampler. */
        SamplerDesc linearClampedSampler
        {
            FILTER_TYPE_LINEAR,    FILTER_TYPE_LINEAR,    FILTER_TYPE_LINEAR,
            TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
        };

        std::array<ImmutableSamplerDesc, 1> immutableSamplers
        {
            {{SHADER_TYPE_PIXEL, "g_Texture", linearClampedSampler}}
        };

        psoTextCubeCI.PSODesc.ResourceLayout.ImmutableSamplers = immutableSamplers.data();
        psoTextCubeCI.PSODesc.ResourceLayout.NumImmutableSamplers = static_cast<uint32_t>(immutableSamplers.size());

        m_pDevice->CreateGraphicsPipelineState(psoTextCubeCI, &m_pPSOTextCube);

        /* Since we did not explicitly specify the type for 'Constants' variable, default
        type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
        never change and are bound directly through the pipeline state object. */
        m_pPSOTextCube->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_VSConstants);

        /* Since we are using a mutable variable, we must create a shader resource binding object
        http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/ */
        m_pPSOTextCube->CreateShaderResourceBinding(&m_pTextCubeSRB, true);

        struct DiligentVertexType
        {
            DirectX::XMFLOAT3 pos;
            DirectX::XMFLOAT2 uv;
        };

        /* Vertex Buffer */
        /* This time we have to duplicate vertices because texture coordinates cannot be shared */
        std::array<DiligentVertexType, 24> cubeVerts =
        {{
            {DirectX::XMFLOAT3{-1, -1, -1}, DirectX::XMFLOAT2{0, 1}},
            {DirectX::XMFLOAT3{-1, +1, -1}, DirectX::XMFLOAT2{0, 0}},
            {DirectX::XMFLOAT3{+1, +1, -1}, DirectX::XMFLOAT2{1, 0}},
            {DirectX::XMFLOAT3{+1, -1, -1}, DirectX::XMFLOAT2{1, 1}},

            {DirectX::XMFLOAT3{-1, -1, -1}, DirectX::XMFLOAT2{0, 1}},
            {DirectX::XMFLOAT3{-1, -1, +1}, DirectX::XMFLOAT2{0, 0}},
            {DirectX::XMFLOAT3{+1, -1, +1}, DirectX::XMFLOAT2{1, 0}},
            {DirectX::XMFLOAT3{+1, -1, -1}, DirectX::XMFLOAT2{1, 1}},

            {DirectX::XMFLOAT3{+1, -1, -1}, DirectX::XMFLOAT2{0, 1}},
            {DirectX::XMFLOAT3{+1, -1, +1}, DirectX::XMFLOAT2{1, 1}},
            {DirectX::XMFLOAT3{+1, +1, +1}, DirectX::XMFLOAT2{1, 0}},
            {DirectX::XMFLOAT3{+1, +1, -1}, DirectX::XMFLOAT2{0, 0}},

            {DirectX::XMFLOAT3{+1, +1, -1}, DirectX::XMFLOAT2{0, 1}},
            {DirectX::XMFLOAT3{+1, +1, +1}, DirectX::XMFLOAT2{0, 0}},
            {DirectX::XMFLOAT3{-1, +1, +1}, DirectX::XMFLOAT2{1, 0}},
            {DirectX::XMFLOAT3{-1, +1, -1}, DirectX::XMFLOAT2{1, 1}},

            {DirectX::XMFLOAT3{-1, +1, -1}, DirectX::XMFLOAT2{1, 0}},
            {DirectX::XMFLOAT3{-1, +1, +1}, DirectX::XMFLOAT2{0, 0}},
            {DirectX::XMFLOAT3{-1, -1, +1}, DirectX::XMFLOAT2{0, 1}},
            {DirectX::XMFLOAT3{-1, -1, -1}, DirectX::XMFLOAT2{1, 1}},

            {DirectX::XMFLOAT3{-1, -1, +1}, DirectX::XMFLOAT2{1, 1}},
            {DirectX::XMFLOAT3{+1, -1, +1}, DirectX::XMFLOAT2{0, 1}},
            {DirectX::XMFLOAT3{+1, +1, +1}, DirectX::XMFLOAT2{0, 0}},
            {DirectX::XMFLOAT3{-1, +1, +1}, DirectX::XMFLOAT2{1, 0}},
        }};

        BufferDesc vertexBufferDesc;
        vertexBufferDesc.Name = "Cube vertex buffer";
        vertexBufferDesc.Usage = USAGE_IMMUTABLE;
        vertexBufferDesc.BindFlags = BIND_VERTEX_BUFFER;
        vertexBufferDesc.Size = sizeof(cubeVerts);
        BufferData vbData;
        vbData.pData = cubeVerts.data();
        vbData.DataSize = sizeof(cubeVerts);
        m_pDevice->CreateBuffer(vertexBufferDesc, &vbData, &m_pTextCubeVertexBuffer);

        /* Index Buffer */
        std::array<uint32_t, 36> cubeIndices
        {{        
            2,0,1,    2,3,0,
            4,6,5,    4,7,6,
            8,10,9,   8,11,10,
            12,14,13, 12,15,14,
            16,18,17, 16,19,18,
            20,21,22, 20,22,23
        }};

        BufferDesc indexBufferDesc;
        indexBufferDesc.Name = "Cube index buffer";
        indexBufferDesc.Usage = USAGE_IMMUTABLE;
        indexBufferDesc.BindFlags = BIND_INDEX_BUFFER;
        indexBufferDesc.Size = sizeof(cubeIndices);
        BufferData ibData;
        ibData.pData = cubeIndices.data();
        ibData.DataSize = sizeof(cubeIndices);
        m_pDevice->CreateBuffer(indexBufferDesc, &ibData, &m_pTextCubeIndexBuffer);

        TextureLoadInfo textureLI;
        textureLI.IsSRGB = true;
        CreateTextureFromFile("raw/image/sphere/marble/BaseColor.png", textureLI, m_pDevice, &m_pTexture);

        m_pTextureSRV = m_pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
        m_pTextCubeSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(m_pTextureSRV);

    }

    void NcGraphics3Impl::UpdateCube02()
    {
        using namespace Diligent;
    }

    void NcGraphics3Impl::UpdateTexturedCube03()
    {
        using namespace Diligent;
    }

    void NcGraphics3Impl::RenderTriangle01()
    {
        using namespace Diligent;
        m_pImmediateContext->SetPipelineState(m_pPSO);

        DrawAttribs drawAttribs;
        drawAttribs.NumVertices = 3;
        m_pImmediateContext->Draw(drawAttribs);
    }

    void NcGraphics3Impl::RenderCube02()
    {
        using namespace Diligent;

        /* Bind vertex and index buffers */
        const uint64_t offset = 0;
        std::array<IBuffer*, 1> pBuffs = {m_pCubeVertexBuffer};
        m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs.data(), &offset, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
        m_pImmediateContext->SetIndexBuffer(m_pCubeIndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        /* Set the pipeline state. */ 
        m_pImmediateContext->SetPipelineState(m_pPSOCube);

        /* Commit shader resources. RESOURCE_STATE_TRANSITION_MODE_TRANSITION mode makes sure that resources are transitioned to required states. */
        m_pImmediateContext->CommitShaderResources(m_pCubeSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        DrawIndexedAttribs drawIndexedAttribs;
        drawIndexedAttribs.IndexType = VT_UINT32;
        drawIndexedAttribs.NumIndices = 36;
        drawIndexedAttribs.Flags = DRAW_FLAG_VERIFY_ALL;
        
        m_pImmediateContext->DrawIndexed(drawIndexedAttribs);
    }

    void NcGraphics3Impl::RenderTexturedCube03()
    {
        using namespace Diligent;
        const uint64_t offset = 0;
        std::array<IBuffer*, 1> pBuffers
        {
            {m_pTextCubeVertexBuffer}
        };

        m_pImmediateContext->SetVertexBuffers(0, 1, pBuffers.data(), &offset, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
        m_pImmediateContext->SetIndexBuffer(m_pTextCubeIndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        m_pImmediateContext->SetPipelineState(m_pPSOTextCube);
        m_pImmediateContext->CommitShaderResources(m_pTextCubeSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        DrawIndexedAttribs drawAttribs;
        drawAttribs.IndexType = VT_UINT32;
        drawAttribs.NumIndices = 36;
        drawAttribs.Flags = DRAW_FLAG_VERIFY_ALL;
        m_pImmediateContext->DrawIndexed(drawAttribs);
    }

    NcGraphics3Impl::~NcGraphics3Impl()
    {
        m_pImmediateContext->Flush();
        Clear();
    }

    void NcGraphics3Impl::SetCamera(Camera* camera) noexcept
    {
        (void)camera;
    }

    auto NcGraphics3Impl::GetCamera() noexcept -> Camera*
    {
        return nullptr;
    }

    void NcGraphics3Impl::SetUi(ui::IUI* ui) noexcept
    {
        (void)ui;
    }

    bool NcGraphics3Impl::IsUiHovered() const noexcept
    {
        return false;
    }

    void NcGraphics3Impl::SetSkybox(const std::string& path)
    {
        (void)path;
    }

    void NcGraphics3Impl::ClearEnvironment()
    {
    }

    void NcGraphics3Impl::Clear() noexcept
    {
    }

    void NcGraphics3Impl::OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks& render)
    {
        NC_LOG_TRACE("Building NcGraphics Tasks");

            update.Add(
                nc::update_task_id::ParticleEmitterUpdate,
                "ParticleEmitterUpdate(stub)",
                []{}
            );

            update.Add(
                nc::update_task_id::ParticleEmitterSync,
                "ParticleEmitterSync(stub)",
                []{},
                {nc::update_task_id::CommitStagedChanges}
            );

        update.Add(
            update_task_id::UpdateRenderData,
            "Update",
            [this]{ Update(); }
        );

        render.Add(
            render_task_id::Render,
            "Render",
            [this]{ Run(); }
        );
    }

    void NcGraphics3Impl::Update()
    {
        const float dt = time::DeltaTime();
        m_elapsedTime += dt;
        if (m_elapsedTime > std::numeric_limits<float>::max() - 0.05f)
        {
            m_elapsedTime = 0.0f;
        }

        auto modelTransform = DirectX::XMMatrixIdentity();
        modelTransform *= DirectX::XMMatrixRotationY(m_elapsedTime * 1.0f) * DirectX::XMMatrixRotationX(-DirectX::XM_PI);
        auto view = DirectX::XMMatrixTranslation(0.0f, 0.0f, -10.0f);
        auto srfPreTransform = GetSurfacePretransformMatrixInternal(DirectX::XMVectorSet(0, 0, 1, 1), m_pSwapChain.ConstPtr());
        auto proj = GetAdjustedProjectionMatrix(DirectX::XM_PI / 4.0f,
                                                0.1f,
                                                100.0f,
                                                m_pSwapChain.ConstPtr());
        m_worldViewProj = modelTransform * view * srfPreTransform * proj;
    }

    void NcGraphics3Impl::Run()
    {
        OPTICK_CATEGORY("Render", Optick::Category::Rendering);
        using namespace Diligent;
        auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
        auto* pDSV = m_pSwapChain->GetDepthBufferDSV();
        m_pImmediateContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        const float clearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
        m_pImmediateContext->ClearRenderTarget(pRTV, clearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        /* Tutorial 1 */
        if (input::KeyHeld(input::KeyCode::One))
            RenderTriangle01();

        /* Map the buffer and write current world-view-projection matrix */
        {
            MapHelper<DirectX::XMMATRIX> cbConstants(m_pImmediateContext, m_VSConstants, MAP_WRITE, MAP_FLAG_DISCARD);
            *cbConstants = m_worldViewProj;
        }

        /* Tutorial 2 */
        if (input::KeyHeld(input::KeyCode::Two))
            RenderCube02();

        /* Tutorial 3 */
        if (input::KeyHeld(input::KeyCode::Three))
            RenderTexturedCube03();

        m_pSwapChain->Present();
    }

    void NcGraphics3Impl::OnResize(const Vector2& dimensions, bool isMinimized)
    {
        (void)isMinimized;

        if (m_pSwapChain)
            m_pSwapChain->Resize(static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y));
    }
} // namespace nc::graphics

#include "NcGraphics3Impl.h"
#include "graphics/PerFrameRenderState.h"
#include "graphics/shader_resource/ShaderResourceBus.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/config/Config.h"
#include "ncengine/debug/DebugRendering.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/View.h"
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/utility/Log.h"
#include "ncengine/window/Window.h"
#include "config/Config.h"
#include "graphics/api/vulkan/ShaderUtilities.h"

#define GLFW_EXPOSE_NATIVE_WIN32

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "imgui/imgui.h"
#include "optick.h"


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
} // anonymous namespace

namespace nc::graphics
{
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

    NcGraphics3Impl::NcGraphics3Impl(const config::GraphicsSettings& graphicsSettings,
                                   const config::MemorySettings& memorySettings,
                                   Registry* registry,
                                   ModuleProvider modules,
                                   SystemEvents& events,
                                   window::NcWindow& window)
        : m_registry{registry},
          m_onResizeConnection{window.OnResize().Connect(this, &NcGraphics3Impl::OnResize)}
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

        /* Shaders */
        auto defaultShaderPath = nc::config::GetAssetSettings().shadersPath;

        ShaderCreateInfo shaderCI;
        shaderCI.SourceLanguage                  = SHADER_SOURCE_LANGUAGE_HLSL;
        shaderCI.Desc.UseCombinedTextureSamplers = true;

        RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
        pFactoryD3D12->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
        shaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;

        auto vertPath = (std::filesystem::path(defaultShaderPath) / std::filesystem::path("Triangle.vsh")).string();
        auto fragPath = (std::filesystem::path(defaultShaderPath) / std::filesystem::path("Triangle.psh")).string();

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
            update_task_id::ParticleEmitterUpdate,
            "Dummy",
            [this]{ ClearEnvironment(); }
        );

        render.Add(
            render_task_id::Render,
            "Render",
            [this]{ Run(); }
        );
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

        m_pImmediateContext->SetPipelineState(m_pPSO);

        /** COMMIT SHADER RESOURCES GOES HERE ONCE WE HAVE RESOURCES TO COMMIT */

        DrawAttribs drawAttribs;
        drawAttribs.NumVertices = 3;
        m_pImmediateContext->Draw(drawAttribs);

        m_pSwapChain->Present();
    }

    void NcGraphics3Impl::OnResize(const Vector2& dimensions, bool isMinimized)
    {
        (void)isMinimized;

        if (m_pSwapChain)
            m_pSwapChain->Resize(static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y));
    }
} // namespace nc::graphics

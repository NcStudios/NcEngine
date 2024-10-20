#include "NcGraphicsImpl2.h"
#include "ncengine/config/Config.h"
#include "ncengine/debug/Profile.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/utility/Log.h"
#include "config/Config.h"
#include "window/NcWindowImpl.h"
#include "graphics2/diligent/RendererSystem.h"

#include "imgui/imgui.h"
#include "DirectXMath.h"

namespace
{
struct NcGraphicsStub2 : nc::graphics::NcGraphics
{
    NcGraphicsStub2(nc::Registry*)
    {
        // client app may still make imgui calls (font loading, etc.), so we need a context
        ImGui::CreateContext();
    }

    ~NcGraphicsStub2() noexcept
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

auto MakeEngineCreateInfo() -> Diligent::EngineCreateInfo
{
    auto engineCI = Diligent::EngineCreateInfo{};
    engineCI.Features.BindlessResources = Diligent::DEVICE_FEATURE_STATE_ENABLED;
    engineCI.Features.ShaderResourceRuntimeArrays = Diligent::DEVICE_FEATURE_STATE_ENABLED;
    return engineCI;
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
            auto ncWindow = modules.Get<window::NcWindow>();
            NC_ASSERT(modules.Get<asset::NcAsset>(), "NcGraphics requires NcAsset to be registered before it.");
            NC_ASSERT(ncWindow, "NcGraphics requires NcWindow to be registered before it.");
            NC_ASSERT(modules.Get<NcScene>(), "NcGraphics requires NcScene to be registered before it.");

            ncWindow->SetWindow(window::WindowInfo
            {
                .dimensions = Vector2{static_cast<float>(graphicsSettings.screenWidth), static_cast<float>(graphicsSettings.screenHeight)},
                .isHeadless = graphicsSettings.isHeadless,
                .useNativeResolution = graphicsSettings.useNativeResolution,
                .launchInFullScreen = graphicsSettings.launchInFullscreen,
                .isResizable = false
            });

            NC_LOG_TRACE("Building NcGraphics module");
            return std::make_unique<NcGraphicsImpl2>(graphicsSettings, memorySettings, registry, modules, events, *ncWindow);
        }

        NC_LOG_TRACE("Graphics disabled - building NcGraphics stub");
        return std::make_unique<NcGraphicsStub2>(registry);
    }
#endif

NcGraphicsImpl2::NcGraphicsImpl2(const config::GraphicsSettings& graphicsSettings,
                                 const config::MemorySettings& memorySettings,
                                 Registry* registry,
                                 ModuleProvider modules,
                                 SystemEvents& events,
                                 window::NcWindow& window)
        : m_registry{registry},
          m_onResizeConnection{window.OnResize().Connect(this, &NcGraphicsImpl2::OnResize)},
          m_engine{graphicsSettings, MakeEngineCreateInfo(), window.GetWindowHandle(), GetSupportedApis()},
          m_shaderBindings{m_engine.GetDevice(), memorySettings.maxTextures},
          m_assetDispatch{
            m_engine.GetContext(),
            m_engine.GetDevice(),
            m_shaderBindings.GetGlobalSignature().GetGlobalTextureBuffer(),
            modules.Get<asset::NcAsset>()->OnTextureUpdate()
          },
          m_testPipeline{
            m_engine.GetContext(),
            m_engine.GetDevice(),
            m_engine.GetSwapChain(),
            m_engine.GetShaderFactory(),
            m_shaderBindings.GetGlobalSignature().GetResourceSignature()
          },
          m_graphicsSettings{graphicsSettings}
{
    (void)memorySettings;
    (void)modules;
    (void)events;

    ImGui::CreateContext();
}

NcGraphicsImpl2::~NcGraphicsImpl2()
{
}

void NcGraphicsImpl2::SetCamera(Camera* camera) noexcept
{
    m_mainCamera = camera;
}

auto NcGraphicsImpl2::GetCamera() noexcept -> Camera*
{
    return m_mainCamera;
}

void NcGraphicsImpl2::SetUi(ui::IUI* ui) noexcept
{
    (void)ui;
}

bool NcGraphicsImpl2::IsUiHovered() const noexcept
{
    return false;
}

void NcGraphicsImpl2::SetSkybox(const std::string& path)
{
    (void)path;
}

void NcGraphicsImpl2::ClearEnvironment()
{
}

void NcGraphicsImpl2::Clear() noexcept
{
    m_mainCamera = nullptr;
}

void NcGraphicsImpl2::OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks& render)
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

    render.Add(
        render_task_id::Render,
        "Render",
        [this]{ Run(); }
    );
}

void NcGraphicsImpl2::Update()
{
}

void NcGraphicsImpl2::Run()
{
    NC_PROFILE_TASK("Render", Optick::Category::Rendering);

    auto& context = m_engine.GetContext();
    auto& swapChain = m_engine.GetSwapChain();

    auto* pRTV = swapChain.GetCurrentBackBufferRTV();
    auto* pDSV = swapChain.GetDepthBufferDSV();
    context.SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    constexpr auto ClearColor = Vector4{0.050f, 0.050f, 0.050f, 1.0f};
    context.ClearRenderTarget(pRTV, &ClearColor.x, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    context.ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    if (m_generateLightCullingMap)
    {
        GenerateLightCullingMap();
    }

    /** Useful discussion on committing resources https://github.com/DiligentGraphics/DiligentEngine/discussions/231  */
    /** Commits the global descriptor set. 
     *  Only needed once per frame unless binding a PSO whose signature is incompatible with this one,
     *  or the bindings in the global signature change. (Set or SetArray is called). Changing the data within a mapped buffer does not require recommit, so long as the buffer signature hasn't changed. **/
    m_shaderBindings.GetGlobalSignature().Commit(context); 

    /*
    RENDER "GRAPH" EXECUTION:

    Example of terminology:
    Material: ToonWithOutlineShadow
    MaterialPass: Shadow, Toon, Alpha, Outline  (These map to PSO). Order Matters.

    for (auto renderOrderIndex : MaxMaterialPassCount) // 0...1...2...3
    {
        // The first material pass would be shadow mapping
        for (const auto& materialPass : renderGraph.MaterialPassRegistry.at(renderOrderIndex))
        {
            // Bind the MaterialPass array signature to the context. (each object has it's own "material instance" data in this array)
            materialPass.GetMaterialSignature().Commit(context); // Still to figure out impl details here.
            context.SetPipelineState(materialPass.PSO);
            
            for (const auto& meshRenderer : materialPass)
            {
                DrawIndexedAttribs drawAttrs{};
                context.DrawIndexed(DrawAttrs);
            }
        }
    }
    */

    m_testPipeline.Render(context);
    swapChain.Present();
}

void NcGraphicsImpl2::OnAddRemoveLight()
{
    if (m_graphicsSettings.cullLights)
        m_generateLightCullingMap = true;
}

void NcGraphicsImpl2::GenerateLightCullingMap()
{
    /* Inputs:   The geometry buffers (vertex and index), all light buffers, the camera, cell dimension properties. 
     * Outputs:  A buffer that maps screen-space cell index to one or more light indices (light buffer index, light index). 
     *           A debug buffer that can be used to render the dimensions of the cells 
     * Function: Binds the inputs to a compute shader and populates the cell/light mapping buffer via execution of the compute shader.
     *           This map buffer must be bound to the graphics pipelines below when this feature is enabled.
     * Remarks:  This doesn't do any culling yet, it just generates a map of influences so only the lights influencing a pixel region will be calculated.
     *           While this doesn't have to happen every frame, just when our light list changes, we have to be careful about when we execute this pipeline in relation to 
     *           other running pipelines. */
    m_generateLightCullingMap = false;
}

void NcGraphicsImpl2::OnResize(const Vector2& dimensions, bool isMinimized)
{
    (void)isMinimized;
    m_engine.GetSwapChain().Resize(static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y));
}
} // namespace nc::graphics

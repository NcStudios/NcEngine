#include "NcGraphicsImpl2.h"
#include "diligent/pass/MaterialPass.h"
#include "diligent/pass/PassUtilities.h"
#include "diligent/pass/WireframePass.h"
#include "diligent/resource/PostProcessSinkIndexBufferResource.h"
#include "frontend/FrontendRenderState.h"

#include "ncengine/asset/NcAsset.h"
#include "ncengine/config/Config.h"
#include "ncengine/debug/Profile.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/graphics/GraphicsUtility.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/utility/Log.h"

#include "imgui.h"
#include "DirectXMath.h"

#include <array>

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
            []{},
            {nc::update_task_id::CommitStagedChanges}
        );

        update.Add(
            nc::update_task_id::SkeletalAnimationUpdate,
            "SkeletalAnimationUpdate(stub)",
            []{},
            {nc::update_task_id::CommitStagedChanges}
        );

        update.Add(
            nc::update_task_id::ParticleEmitterSync,
            "ParticleEmitterSync(stub)",
            []{},
            {nc::update_task_id::UpdateTransforms}
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
    auto IsPostProcessEffectEnabled(nc::PostProcessEffectId) const -> bool override { return false; }
    void SetPostProcessEffectEnabled(nc::PostProcessEffectId, bool) override {}
    void SetPostProcessEffectProperties(nc::PostProcessEffectId,
                                        nc::PostProcessPassFlag::type,
                                        const nc::PostProcessPassProperties&) override {}
    auto GetPostProcessEffectProperties(nc::PostProcessEffectId,
                                        nc::PostProcessPassFlag::type) const -> const nc::PostProcessPassProperties& override
    {
        static auto dummy = nc::PostProcessPassProperties{};
        return dummy;
    }
};

auto MakeEngineCreateInfo(bool enableValidation) -> Diligent::EngineCreateInfo
{
    auto engineCI = Diligent::EngineCreateInfo{};
    engineCI.EnableValidation = enableValidation;
    engineCI.Features.BindlessResources = Diligent::DEVICE_FEATURE_STATE_ENABLED;
    engineCI.Features.ShaderResourceRuntimeArrays = Diligent::DEVICE_FEATURE_STATE_ENABLED;
    engineCI.Features.WireframeFill = Diligent::DEVICE_FEATURE_STATE_ENABLED;
    return engineCI;
}

void LogCallback(Diligent::DEBUG_MESSAGE_SEVERITY severity,
                 const char* msg,
                 const char*,
                 const char* file,
                 int line)
{
    constexpr auto subsystem = "Diligent";
    if (!file) file = "";
    switch (severity)
    {
        case Diligent::DEBUG_MESSAGE_SEVERITY_INFO:
            NC_LOG_TRACE_EXT(subsystem, file, line, msg);
            break;
        case Diligent::DEBUG_MESSAGE_SEVERITY_WARNING:
            NC_LOG_WARNING_EXT(subsystem, file, line, msg);
            break;
        case Diligent::DEBUG_MESSAGE_SEVERITY_ERROR:
        case Diligent::DEBUG_MESSAGE_SEVERITY_FATAL_ERROR:
            NC_LOG_ERROR_EXT(subsystem, file, line, msg);
            break;
    }
}
} // anonymous namespace

namespace nc::graphics
{
#ifdef NC_USE_DILIGENT
    auto BuildGraphicsModule(const config::ProjectSettings& projectSettings,
                             const config::AssetSettings& assetSettings,
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
                .useNativeResolution = graphicsSettings.useNativeResolution,
                .launchInFullScreen = graphicsSettings.launchInFullscreen,
                .isResizable = false
            });

            NC_LOG_TRACE("Building NcGraphics module");
            return std::make_unique<NcGraphicsImpl2>(graphicsSettings, memorySettings, assetSettings.shadersPath, registry, modules, events, *ncWindow);
        }

        NC_LOG_TRACE("Graphics disabled - building NcGraphics stub");
        return std::make_unique<NcGraphicsStub2>(registry);
    }
#endif

NcGraphicsImpl2::NcGraphicsImpl2(const config::GraphicsSettings& graphicsSettings,
                                 const config::MemorySettings& memorySettings,
                                 std::string_view shadersPath,
                                 Registry* registry,
                                 ModuleProvider modules,
                                 SystemEvents& events,
                                 window::NcWindow& window)
        : m_world{registry->GetEcs()},
          m_engine{
            MakeEngineCreateInfo(graphicsSettings.useValidationLayers),
            window.GetWindowHandle(),
            shadersPath,
            ::LogCallback
          },
          m_shaderBindings{
            m_engine.GetDevice(),
            m_engine.GetContext(),
            memorySettings.maxTextures,
            memorySettings.maxRenderers,
            memorySettings.maxSpotLights,
            memorySettings.maxPointLights,
            memorySettings.maxDirectionalLights,
            memorySettings.maxBones,
            memorySettings.maxRenderers / 2,
            memorySettings.maxRenderers / 2,
            memorySettings.maxBones / 4
          },
          m_ui{
            m_engine.GetDevice(),
            m_engine.GetSwapChain().GetDesc(),
            window.GetWindowHandle(),
            modules.Get<asset::NcAsset>()->OnFontUpdate()
          },
          m_passManifest
          {
            std::vector<PassDesc>
            {
                PassDesc{
                    .id = MaterialPassFlag::Toon,
                    .name = "Toon",
                    .type = PassType::Material,
                    .shaderPaths = ShaderPaths{"Toon.psh", "Toon.vsh"},
                    .colorSink = MainColor,
                    .depthSink = MainDepth
                },
                PassDesc{
                    .id = MaterialPassFlag::Toon,
                    .name = "ToonSkinned",
                    .type = PassType::SkinnedMaterial,
                    .shaderPaths = ShaderPaths{"Toon.psh", "ToonSkinned.vsh"},
                    .colorSink = MainColor,
                    .depthSink = MainDepth
                },
                PassDesc{
                    .id = MaterialPassFlag::Normals,
                    .name = "Normals",
                    .type = PassType::Material,
                    .shaderPaths = ShaderPaths{"Normals.psh", "Toon.vsh"},
                    .colorSink = NormalsColor
                },
                PassDesc{
                    .id = MaterialPassFlag::Normals,
                    .name = "NormalsSkinned",
                    .type = PassType::SkinnedMaterial,
                    .shaderPaths = ShaderPaths{"Normals.psh", "ToonSkinned.vsh"},
                    .colorSink = NormalsColor
                },
                PassDesc{
                    .id = MiscPassFlag::Wireframe,
                    .name = "Wireframe",
                    .type = PassType::Wireframe,
                    .shaderPaths = ShaderPaths{"Wireframe.psh", "Wireframe.vsh"},
                    .colorSink = MainColor,
                    .depthSink = MainDepth
                },
                PassDesc{
                    .id = PostProcessPassFlag::Wave,
                    .name = "Post Process Wave",
                    .type = PassType::PostProcess,
                    .shaderPaths = ShaderPaths{"PPWave.psh", "PostProcess.vsh"},
                    .colorSources = SingleSource(MainColor),
                    .colorSink = PPWaveColor
                },
                PassDesc{
                    .id = PostProcessPassFlag::Outline,
                    .name = "Post Process Outline",
                    .type = PassType::PostProcess,
                    .shaderPaths = ShaderPaths{"PPOutline.psh", "PostProcess.vsh"},
                    .colorSources = std::vector{PPWaveColor, NormalsColor},
                    .depthSources = SingleSource(MainDepth),
                    .colorSink = PPOutlineColor
                }
            },
            GetImplementedMaterialPassFlags(),
            GetPostProcessPassFlags(),
            GetMiscsPassFlags()
          },
          m_passBackend{
            m_engine.GetDevice(),
            m_engine.GetSwapChain(),
            m_engine.GetShaderFactory(),
            m_shaderBindings,
            m_passManifest
          },
          m_frontend{
            m_engine.GetContext(),
            m_engine.GetDevice(),
            m_shaderBindings.GetPerFrameSignature().GetTextureBuffer(),
            m_shaderBindings.GetMeshBuffer(),
            m_world,
            modules,
            events,
            memorySettings.maxTransforms,
            memorySettings.maxRenderers,
            memorySettings.maxBones,
            graphicsSettings.initialBatchSize,
            modules.Get<asset::NcAsset>()->OnTextureUpdate(),
            modules.Get<asset::NcAsset>()->OnMeshUpdate(),
            modules.Get<asset::NcAsset>()->OnSkeletalAnimationUpdate(),
            modules.Get<asset::NcAsset>()->OnBoneUpdate()
          },
          m_onResizeConnection{window.OnResize().Connect(this, &NcGraphicsImpl2::OnResize)},
          m_resizeNeeded{false}
{
}

NcGraphicsImpl2::~NcGraphicsImpl2()
{
}

void NcGraphicsImpl2::SetCamera(Camera* camera) noexcept
{
    m_frontend.GetCameraSubsystem().Set(camera);
}

auto NcGraphicsImpl2::GetCamera() noexcept -> Camera*
{
    return m_frontend.GetCameraSubsystem().Get();
}

void NcGraphicsImpl2::SetUi(ui::IUI* ui) noexcept
{
    m_frontend.GetUISubsystem().SetClientUI(ui);
}

bool NcGraphicsImpl2::IsUiHovered() const noexcept
{
    return m_frontend.GetUISubsystem().IsHovered();
}

void NcGraphicsImpl2::SetSkybox(const std::string& path)
{
    (void)path;
}

void NcGraphicsImpl2::ClearEnvironment()
{
}

auto NcGraphicsImpl2::IsPostProcessEffectEnabled(PostProcessEffectId effectId) const -> bool
{
    return m_frontend.GetPostProcessSubsystem().IsEnabled(effectId);
}

void NcGraphicsImpl2::SetPostProcessEffectEnabled(PostProcessEffectId effectId, bool enabled)
{
    m_frontend.GetPostProcessSubsystem().SetEnabled(effectId, enabled);
}

auto NcGraphicsImpl2::GetPostProcessEffectProperties(PostProcessEffectId effectId,
                                                     PostProcessPassFlag::type pass) const -> const PostProcessPassProperties&
{
    return m_frontend.GetPostProcessSubsystem().GetProperties(effectId, pass);
}

void NcGraphicsImpl2::SetPostProcessEffectProperties(PostProcessEffectId effectId,
                                                     PostProcessPassFlag::type pass,
                                                     const PostProcessPassProperties& properties)
{
    m_frontend.GetPostProcessSubsystem().SetProperties(effectId, pass, properties);
}

void NcGraphicsImpl2::OnBeforeSceneLoad()
{
    m_frontend.OnBeforeSceneLoad();
}

void NcGraphicsImpl2::Clear() noexcept
{
    m_frontend.Clear();
}

void NcGraphicsImpl2::OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks& render)
{
    NC_LOG_TRACE("Building NcGraphics Tasks");

    update.Add(
        update_task_id::ParticleEmitterUpdate,
        "ParticleEmitterUpdate(stub)",
        []{},
        {update_task_id::CommitStagedChanges}
    );



    update.Add(
        update_task_id::ParticleEmitterSync,
        "ParticleEmitterSync(stub)",
        []{},
        {update_task_id::UpdateTransforms}
    );

    update.Add(
        update_task_id::SkeletalAnimationUpdate,
        "SkeletalAnimationUpdate",
        [this]{
            m_frontend.GetSkeletalAnimationSubsystem().Update(m_world);
        },
        {update_task_id::CommitStagedChanges}
    );

    render.Add(
        render_task_id::Render,
        "Render",
        [this]{ Run(); }
    );
}

void NcGraphicsImpl2::Run()
{
    NC_PROFILE_TASK("Render", Optick::Category::Rendering);

    if (m_resizeNeeded)
    {
        Resize();
    }

    auto& context = m_engine.GetContext();
    auto& device = m_engine.GetDevice();
    auto& swapChain = m_engine.GetSwapChain();

    m_ui.FrameBegin(swapChain);
    m_frontend.GetUISubsystem().UpdateUI(m_world);

    auto renderState = m_frontend.BuildRenderState(m_world);

    m_passBackend.Update(renderState.postProcessState);
    m_shaderBindings.Update(context, device, renderState);
    m_shaderBindings.GetPerFrameSignature().Commit(context);
    m_shaderBindings.GetPerPassSignature().Commit(context);
    m_shaderBindings.GetMeshBuffer().SetBuffers(context);

    m_passBackend.RenderMaterial(
        context,
        swapChain,
        m_shaderBindings.GetPerPassSignature(),
        renderState.meshRenderState.staticMeshBatches,
        renderState.meshRenderState.skinnedMeshBatches
    );

    m_passBackend.RenderWireframe(
        context,
        swapChain,
        m_shaderBindings.GetPerPassSignature(),
        renderState.wireframeRenderState
    );

    m_passBackend.RenderPostProcess(
        context,
        swapChain,
        m_shaderBindings.GetPerPassSignature(),
        m_shaderBindings.GetPerFrameSignature()
    );

    m_ui.Render(context);

    swapChain.Present();
    context.Flush();
    context.FinishFrame();
}

void NcGraphicsImpl2::OnResize(const Vector2& dimensions, bool isMinimized)
{
    (void)isMinimized;
    m_engine.GetSwapChain().Resize(static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y));
    m_resizeNeeded = true;
    m_dimensions = dimensions;
}

void NcGraphicsImpl2::Resize()
{
    m_engine.GetSwapChain().Resize(static_cast<uint32_t>(m_dimensions.x), static_cast<uint32_t>(m_dimensions.y));
    m_shaderBindings.GetPerPassSignature().GetPostProcessColorSinkBufferResource().Resize(m_engine.GetDevice(), static_cast<uint32_t>(m_dimensions.x), static_cast<uint32_t>(m_dimensions.y));
    m_shaderBindings.GetPerPassSignature().GetPostProcessDepthSinkBufferResource().Resize(m_engine.GetDevice(), static_cast<uint32_t>(m_dimensions.x), static_cast<uint32_t>(m_dimensions.y));
    m_resizeNeeded = false;
}
} // namespace nc::graphics

#include "NcGraphicsImpl2.h"
#include "frontend/FrontendRenderState.h"

#include "diligent/pass/MaterialPass.h"
#include "diligent/pass/PassUtilities.h"
#include "diligent/pass/WireframePass.h"
#include "diligent/resource/SinkIndexBufferResource.h"

#include "ncengine/asset/NcAsset.h"
#include "ncengine/config/Config.h"
#include "ncengine/debug/Profile.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/GraphicsUtility.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/utility/Log.h"

#include "imgui.h"
#include "DirectXMath.h"

#include <array>

namespace
{
struct NcGraphicsStub2 : nc::NcGraphics
{
    NcGraphicsStub2()
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
        using namespace nc::update_task_id;
        using namespace nc::render_task_id;
        update.Add(ParticleEmitterUpdate,   "ParticleEmitterUpdate(stub)",   []{}, {CommitStagedChanges});
        update.Add(SkeletalAnimationUpdate, "SkeletalAnimationUpdate(stub)", []{}, {CommitStagedChanges});
        render.Add(Render,                  "Render(stub)",                  []{});
    }

    void SetCamera(nc::Camera*) noexcept override {}
    auto GetCamera() noexcept -> nc::Camera* override { return nullptr; }
    void SetUi(nc::ui::IUI*) noexcept override {}
    bool IsUiHovered() const noexcept override { return false; }
    auto GetSkybox() const -> nc::asset::AssetId override { return nc::asset::NullAssetId; }
    void SetSkybox(const std::string&) override {}
    void ClearSkybox() override {}
    auto GetEnvironment() const -> const nc::Environment& override { static auto dummy = nc::Environment{}; return dummy; }
    void SetEnvironment(const nc::Environment&) override {}
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
    auto GetTextureView(nc::TextureViewType, uint32_t) -> void* override { return nullptr; }
};

auto MakeEngineCreateInfo(bool enableValidation) -> Diligent::EngineCreateInfo
{
    auto engineCI = Diligent::EngineCreateInfo{};
    engineCI.EnableValidation = enableValidation;
    engineCI.Features.BindlessResources = Diligent::DEVICE_FEATURE_STATE_ENABLED;
    engineCI.Features.ShaderResourceRuntimeArrays = Diligent::DEVICE_FEATURE_STATE_ENABLED;
    engineCI.Features.WireframeFill = Diligent::DEVICE_FEATURE_STATE_ENABLED;
    engineCI.Features.DepthClamp = Diligent::DEVICE_FEATURE_STATE_ENABLED;
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

namespace nc
{
auto BuildGraphicsModule(const config::ProjectSettings&,
                         const config::AssetSettings& assetSettings,
                         const config::GraphicsSettings& graphicsSettings,
                         const config::MemorySettings& memorySettings,
                         ModuleProvider modules,
                         ecs::Ecs world,
                         SystemEvents& events) -> std::unique_ptr<NcGraphics>
{
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
        return std::make_unique<graphics::NcGraphicsImpl2>(graphicsSettings, memorySettings, assetSettings.shadersPath, world, modules, events, *ncWindow);
    }

    NC_LOG_TRACE("Graphics disabled - building NcGraphics stub");
    return std::make_unique<NcGraphicsStub2>();
}

namespace graphics
{
NcGraphicsImpl2::NcGraphicsImpl2(const config::GraphicsSettings& graphicsSettings,
                                 const config::MemorySettings& memorySettings,
                                 std::string_view shadersPath,
                                 ecs::Ecs world,
                                 ModuleProvider modules,
                                 SystemEvents& events,
                                 window::NcWindow& window)
        : m_world{world},
          m_engine{
            MakeEngineCreateInfo(graphicsSettings.useValidationLayers),
            DeviceCapability{.msaaSampleCount = graphicsSettings.antialiasing},
            window.GetWindowHandle(),
            shadersPath,
            ::LogCallback
          },
          m_shaderBindings{
            m_engine.GetDevice(),
            m_engine.GetContext(),
            memorySettings
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
                    .flag = MiscPassFlag::Skybox,
                    .name = "Skybox",
                    .type = PassType::Skybox,
                    .layoutElements = VertexAttribute::Pos,
                    .shaderPaths = ShaderPaths{shader::SkyboxPixel, shader::SkyboxVertex},
                    .colorSink = ColorTarget::Main,
                    .depthSink = DepthTarget::Main,
                    .useDepthTest = true
                },
                PassDesc{
                    .flag = MaterialPassFlag::UniShadow,
                    .name = "UniShadow",
                    .type = PassType::Material,
                    .layoutElements = VertexAttribute::Pos,
                    .shaderPaths = ShaderPaths{.vertexShaderPath = shader::UniShadowMapVertex},
                    .shadowMapSink = ShadowMapTarget::Uni,
                    .isMsaa = false,
                    .useDepthTest = true
                },
                PassDesc{
                    .flag = MaterialPassFlag::UniShadow,
                    .name = "UniShadowSkinned",
                    .type = PassType::SkinnedMaterial,
                    .layoutElements = VertexAttribute::Skinning,
                    .shaderPaths = ShaderPaths{.vertexShaderPath =  shader::UniShadowMapSkinnedVertex},
                    .shadowMapSink = ShadowMapTarget::Uni,
                    .isMsaa = false,
                    .useDepthTest = true
                },
                PassDesc{
                    .flag = MaterialPassFlag::PointShadow,
                    .name = "PointShadow",
                    .type = PassType::Material,
                    .layoutElements = VertexAttribute::Pos,
                    .shaderPaths = ShaderPaths{.pixelShaderPath = shader::PointShadowMapPixel, .vertexShaderPath = shader::PointShadowMapVertex},
                    .shadowMapSink = ShadowMapTarget::Point,
                    .cullMode = CullMode::Back,
                    .isMsaa = false,
                    .useDepthTest = true
                },
                PassDesc{
                    .flag = MaterialPassFlag::PointShadow,
                    .name = "PointShadowSkinned",
                    .type = PassType::SkinnedMaterial,
                    .layoutElements = VertexAttribute::Skinning,
                    .shaderPaths = ShaderPaths{ .pixelShaderPath = shader::PointShadowMapPixel, .vertexShaderPath = shader::PointShadowMapSkinnedVertex},
                    .shadowMapSink = ShadowMapTarget::Point,
                    .cullMode = CullMode::Back,
                    .isMsaa = false,
                    .useDepthTest = true
                },
                PassDesc{
                    .flag = MaterialPassFlag::Depth,
                    .name = "Depth",
                    .type = PassType::Material,
                    .layoutElements = VertexAttribute::Texturing,
                    .shaderPaths = ShaderPaths{.vertexShaderPath = shader::ToonVertex},
                    .depthSink = DepthTarget::Main,
                    .isMsaa = false,
                    .useDepthTest = true
                },
                PassDesc{
                    .flag = MaterialPassFlag::Depth,
                    .name = "DepthSkinned",
                    .type = PassType::SkinnedMaterial,
                    .layoutElements = VertexAttribute::All,
                    .shaderPaths = ShaderPaths{.vertexShaderPath = shader::ToonSkinnedVertex},
                    .depthSink = DepthTarget::Main,
                    .isMsaa = false,
                    .useDepthTest = true
                },
                PassDesc{
                    .flag = MaterialPassFlag::Toon,
                    .name = "Toon",
                    .type = PassType::Material,
                    .layoutElements = VertexAttribute::Texturing,
                    .shaderPaths = ShaderPaths{shader::ToonPixel, shader::ToonVertex},
                    .colorSink = ColorTarget::Main,
                    .depthSink = DepthTarget::Main,
                    .useDepthTest = true,
                    .alphaBlend = true
                },
                PassDesc{
                    .flag = MaterialPassFlag::Toon,
                    .name = "ToonSkinned",
                    .type = PassType::SkinnedMaterial,
                    .layoutElements = VertexAttribute::All,
                    .shaderPaths = ShaderPaths{shader::ToonPixel, shader::ToonSkinnedVertex},
                    .colorSink = ColorTarget::Main,
                    .depthSink = DepthTarget::Main,
                    .useDepthTest = true,
                    .alphaBlend = true
                },
                PassDesc{
                    .flag = MaterialPassFlag::Normals,
                    .name = "Normals",
                    .type = PassType::Material,
                    .layoutElements = VertexAttribute::Texturing,
                    .shaderPaths = ShaderPaths{shader::NormalsPixel, shader::ToonVertex},
                    .colorSink = ColorTarget::Normals,
                    .depthSink = DepthTarget::Main,
                    .useDepthTest = true
                },
                PassDesc{
                    .flag = MaterialPassFlag::Normals,
                    .name = "NormalsSkinned",
                    .type = PassType::SkinnedMaterial,
                    .layoutElements = VertexAttribute::All,
                    .shaderPaths = ShaderPaths{shader::NormalsPixel, shader::ToonSkinnedVertex},
                    .colorSink = ColorTarget::Normals,
                    .depthSink = DepthTarget::Main,
                    .useDepthTest = true
                },
                PassDesc{
                    .flag = PostProcessPassFlag::Outline,
                    .name = "Post Process Outline",
                    .type = PassType::PostProcess,
                    .layoutElements = VertexAttribute::None,
                    .shaderPaths = ShaderPaths{shader::PPOutlinePixel, shader::PostProcessVertex},
                    .colorSources = std::vector{ColorTarget::Main, ColorTarget::Normals},
                    .depthSources = std::vector{DepthTarget::Main},
                    .postProcessSink = PostProcessTarget::PPOutline,
                    .isMsaa = false,
                    .useDepthTest = false
                },
                PassDesc{
                    .flag = PostProcessPassFlag::Fxaa,
                    .name = "Post Process FXAA",
                    .type = PassType::PostProcess,
                    .layoutElements = VertexAttribute::None,
                    .shaderPaths = ShaderPaths{shader::PPFxaaPixel, shader::PostProcessVertex},
                    .postProcessSource = PostProcessTarget::PPOutline,
                    .postProcessSink = PostProcessTarget::PPFxaa,
                    .isMsaa = false,
                    .useDepthTest = false
                },
                PassDesc{
                    .flag = PostProcessPassFlag::Noise,
                    .name = "Post Process Noise",
                    .type = PassType::PostProcess,
                    .layoutElements = VertexAttribute::None,
                    .shaderPaths = ShaderPaths{shader::PPNoisePixel, shader::PostProcessVertex},
                    .postProcessSource = PostProcessTarget::PPFxaa,
                    .postProcessSink = PostProcessTarget::PPNoise,
                    .isMsaa = false,
                    .useDepthTest = false
                },
                PassDesc{
                    .flag = MiscPassFlag::Wireframe,
                    .name = "Wireframe",
                    .type = PassType::Wireframe,
                    .layoutElements = VertexAttribute::Pos,
                    .shaderPaths = ShaderPaths{shader::WireframePixel, shader::WireframeVertex},
                    .colorSink = ColorTarget::Swapchain,
                    .depthSink = DepthTarget::Main,
                    .useDepthTest = true
                },
                PassDesc{
                    .flag = MiscPassFlag::Particle,
                    .name = "Particle",
                    .type = PassType::Particle,
                    .layoutElements = VertexAttribute::Pos | VertexAttribute::UV,
                    .shaderPaths = ShaderPaths{shader::ParticlePixel, shader::ParticleVertex},
                    .colorSink = ColorTarget::Swapchain,
                    .depthSink = DepthTarget::Main,
                    .useDepthTest = true,
                    .alphaBlend = true
                },
            },
            GetMaterialPassFlags(),
            GetPostProcessPassFlags(),
            GetMiscsPassFlags()
          },
          m_passBackend{
            m_engine.GetDevice(),
            m_engine.GetContext(),
            m_engine.GetSwapChain(),
            m_engine.GetShaderFactory(),
            m_shaderBindings,
            m_passManifest,
            graphicsSettings,
            memorySettings,
            m_engine.GetDeviceCapability().msaaSampleCount
          },
          m_frontend{
            m_engine.GetContext(),
            m_engine.GetDevice(),
            m_shaderBindings.GetPerFrameSignature().GetCubeMapBuffer(),
            m_shaderBindings.GetPerFrameSignature().GetTextureBuffer(),
            m_shaderBindings.GetMeshBuffer(),
            m_world,
            modules,
            events,
            memorySettings.maxTransforms,
            memorySettings.maxRenderers,
            memorySettings.maxBones,
            memorySettings.maxParticles,
            graphicsSettings.initialBatchSize,
            graphicsSettings.shadowMapResolution,
            modules.Get<asset::NcAsset>()->OnCubeMapUpdate(),
            modules.Get<asset::NcAsset>()->OnTextureUpdate(),
            modules.Get<asset::NcAsset>()->OnMeshUpdate(),
            modules.Get<asset::NcAsset>()->OnSkeletalAnimationUpdate(),
            modules.Get<asset::NcAsset>()->OnBoneUpdate()
          },
          m_onResizeConnection{window.OnResize().Connect(this, &NcGraphicsImpl2::OnResize)},
          m_onViewportResizeConnection{window.OnViewportResize().Connect(this, &NcGraphicsImpl2::OnViewportResize)},
          m_viewport{},
          m_isViewportDirty{false},
          m_resizeNeeded{false},
          m_numSamples{m_engine.GetDeviceCapability().msaaSampleCount},
          m_isMinimized{false}
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

auto NcGraphicsImpl2::GetSkybox() const -> nc::asset::AssetId
{
    return m_frontend.GetEnvironmentSubsystem().GetSkybox();
}

void NcGraphicsImpl2::SetSkybox(const std::string& path)
{
    m_frontend.GetEnvironmentSubsystem().SetSkybox(path);
}

void NcGraphicsImpl2::ClearSkybox()
{
    m_frontend.GetEnvironmentSubsystem().ClearSkybox();
}

auto NcGraphicsImpl2::GetEnvironment() const -> const Environment&
{
    return m_frontend.GetEnvironmentSubsystem().GetEnvironment();
}

void NcGraphicsImpl2::SetEnvironment(const Environment& environment)
{
    m_frontend.GetEnvironmentSubsystem().SetEnvironment(environment);
};

void NcGraphicsImpl2::ClearEnvironment()
{
    m_frontend.GetEnvironmentSubsystem().ClearEnvironment();
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

auto NcGraphicsImpl2::GetTextureView(TextureViewType type, uint32_t index) -> void*
{
    switch(type)
    {
        case TextureViewType::Asset:
        {
            return static_cast<void*>(m_shaderBindings.GetPerFrameSignature().GetTextureBuffer().GetTextureView(index));
        }
        case TextureViewType::ColorSink:
        {
            return static_cast<void*>(m_shaderBindings.GetPerPassSignature().GetColorSinksResource().GetTextureView(index));
        }
        case TextureViewType::DepthSink:
        {
            return static_cast<void*>(m_shaderBindings.GetPerPassSignature().GetDepthSinksResource().GetTextureView(index));
        }
        case TextureViewType::UniShadowSink:
        {
            return static_cast<void*>(m_shaderBindings.GetPerPassSignature().GetUniShadowMapSinksResource().GetTextureView(index));
        }
        case TextureViewType::PointShadowSink:
        {
            return static_cast<void*>(m_shaderBindings.GetPerPassSignature().GetPointShadowMapSinksResource().GetTextureView(index));
        }
        case TextureViewType::PostProcess:
        {
            return static_cast<void*>(m_shaderBindings.GetPerPassSignature().GetPostProcessSinkResource(index).GetTextureView(0u));
        }
        default:
        {
            throw NcError("Invalid TextureViewType.");
        }
    }
}

void NcGraphicsImpl2::OnBeforeSceneLoad(const Scene& sceneToLoad)
{
    m_frontend.OnBeforeSceneLoad(sceneToLoad);
    m_passBackend.OnBeforeSceneLoad(sceneToLoad);
}

void NcGraphicsImpl2::Clear() noexcept
{
    ClearEnvironment();
    m_frontend.Clear();
}

void NcGraphicsImpl2::OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks& render)
{
    NC_LOG_TRACE("Building NcGraphics Tasks");

    update.Add(
        update_task_id::ParticleEmitterUpdate,
        "ParticleEmitterUpdate",
        [this]{
            m_frontend.GetParticleSubsystem().Update(GetCamera());
        },
        {update_task_id::CommitStagedChanges}
    );

    update.Add(
        update_task_id::SkeletalAnimationUpdate,
        "SkeletalAnimationUpdate",
        [this]{
            m_frontend.GetSkeletalAnimationSubsystem().Update(m_world);
        },
        {update_task_id::CommitStagedChanges}
    );

    update.Add(
        update_task_id::BoneSnapperUpdate,
        "BoneSnapperUpdate",
        [this]{
            m_frontend.GetBoneSnapperSubsystem().Update(m_world);
        },
        {update_task_id::SkeletalAnimationUpdate}
    );

    render.Add(
        render_task_id::Render,
        "Render",
        [this]{ Run(); }
    );
}

void NcGraphicsImpl2::Run()
{
    NC_PROFILE_TASK("Render", ProfileCategory::Rendering);

    if (m_resizeNeeded)
    {
        Resize();
    }

    if (m_isViewportDirty)
    {
        ViewportResize();
    }

    if (m_isMinimized)
    {
        return;
    }

    auto& context = m_engine.GetContext();
    auto& device = m_engine.GetDevice();
    auto& swapChain = m_engine.GetSwapChain();

    m_ui.FrameBegin(swapChain);
    m_frontend.GetUISubsystem().UpdateUI(m_world);

    auto renderState = m_frontend.BuildRenderState(m_world);

    m_passBackend.Update(device, context, renderState.postProcessState);
    m_shaderBindings.Update(context, device, renderState);
    m_shaderBindings.GetPerFrameSignature().Commit(context);
    m_shaderBindings.GetPerPassSignature().Commit(context);
    m_shaderBindings.GetMeshBuffer().SetBuffers(context);

    m_passBackend.RenderMaterial(
        context,
        swapChain,
        renderState.meshRenderState.staticMeshBatches,
        renderState.meshRenderState.skinnedMeshBatches,
        renderState.lightRenderState.lights,
        m_viewport
    );

    m_passBackend.RenderSkybox(
        context,
        swapChain,
        renderState.environmentRenderState,
        m_viewport
    );

    m_passBackend.RenderPostProcess(
        context,
        swapChain
    );

    m_passBackend.RenderOutputToSwapchain(
        context,
        swapChain
    );

    m_passBackend.RenderWireframe(
        context,
        swapChain,
        renderState.wireframeRenderState,
        m_viewport
    );

    m_passBackend.RenderParticle(
        context,
        swapChain,
        renderState.particleRenderState,
        m_viewport
    );

    m_ui.Render(context);

    swapChain.Present();
    context.Flush();
    context.FinishFrame();
}

void NcGraphicsImpl2::OnResize(const Vector2& dimensions, bool isMinimized)
{
    if (isMinimized)
    {
        m_isMinimized = true;
        return;
    }

    m_isMinimized = false;
    m_engine.GetSwapChain().Resize(static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y));
    m_resizeNeeded = true;
    m_dimensions = dimensions;
}

void NcGraphicsImpl2::OnViewportResize(const Viewport& viewport)
{
    m_isViewportDirty = true;
    m_viewport = viewport;
}

void NcGraphicsImpl2::Resize()
{
    const auto width = static_cast<uint32_t>(m_dimensions.x);
    const auto height = static_cast<uint32_t>(m_dimensions.y);
    m_engine.GetSwapChain().Resize(width, height);
    m_shaderBindings.GetPerPassSignature().GetColorSinksResource().Resize(m_engine.GetDevice(), m_engine.GetContext(), width, height, m_numSamples);
    m_shaderBindings.GetPerPassSignature().GetDepthSinksResource().Resize(m_engine.GetDevice(),  m_engine.GetContext(), width, height, m_numSamples);
    m_resizeNeeded = false;
    m_engine.GetDevice().IdleGPU();
}

void NcGraphicsImpl2::ViewportResize()
{
    if (auto* camera = m_frontend.GetCameraSubsystem().Get())
    {
        camera->UpdateProjectionMatrix(m_viewport.Size.x, m_viewport.Size.y);
    }
    m_isViewportDirty = false;
}

} // namespace graphics
} // namespace nc

#include "NcGraphicsImpl.h"
#include "graphics/PerFrameRenderState.h"
#include "graphics/shader_resource/ShaderResourceBus.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/config/Config.h"
#include "ncengine/debug/DebugRendering.h"
#include "ncengine/debug/Profile.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/View.h"
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/utility/Log.h"
#include "ncengine/window/Window.h"
#include "window/NcWindowImpl.h"

#include "imgui/imgui.h"

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
#ifndef NC_USE_DILIGENT
    auto BuildGraphicsModule(const config::ProjectSettings& projectSettings,
                             const config::GraphicsSettings& graphicsSettings,
                             const config::MemorySettings& memorySettings,
                             ModuleProvider modules,
                             Registry* registry,
                             SystemEvents& events) -> std::unique_ptr<NcGraphics>
    {
        if (graphicsSettings.enabled)
        {
            auto ncAsset = modules.Get<asset::NcAsset>();
            auto ncWindow = modules.Get<window::NcWindow>();
            NC_ASSERT(ncAsset, "NcGraphics requires NcAsset to be registered before it.");
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

            NC_LOG_TRACE("Selecting Graphics API");
            auto graphicsApi = GraphicsFactory(projectSettings, graphicsSettings, ncAsset, *ncWindow);

            NC_LOG_TRACE("Building NcGraphics module");
            return std::make_unique<NcGraphicsImpl>(graphicsSettings, memorySettings, registry, modules, events, std::move(graphicsApi), *ncWindow);
        }

        NC_LOG_TRACE("Graphics disabled - building NcGraphics stub");
        return std::make_unique<NcGraphicsStub>(registry);
    }
#endif
    NcGraphicsImpl::NcGraphicsImpl(const config::GraphicsSettings& graphicsSettings,
                                   const config::MemorySettings& memorySettings,
                                   Registry* registry,
                                   ModuleProvider modules,
                                   SystemEvents& events,
                                   std::unique_ptr<IGraphics> graphics,
                                   window::NcWindow& window)
        : m_registry{registry},
          m_graphics{std::move(graphics)},
          m_assetResources{AssetResourcesConfig{memorySettings}, m_graphics->ResourceBus(), modules.Get<asset::NcAsset>()},
          m_systemResources{SystemResourcesConfig{graphicsSettings, memorySettings}, m_registry, m_graphics->ResourceBus(), modules, events, std::bind_front(&NcGraphics::GetCamera, this)},
          m_onResizeConnection{window.OnResize().Connect(this, &NcGraphicsImpl::OnResize)}
    {
#if NC_DEBUG_RENDERING_ENABLED
        debug::DebugRendererInitialize(registry->GetEcs());
#endif
    }

    void NcGraphicsImpl::SetCamera(Camera* camera) noexcept
    {
        NC_LOG_TRACE("Setting main camera to: {}", static_cast<void*>(camera));
        m_systemResources.cameras.Set(camera);
    }

    auto NcGraphicsImpl::GetCamera() noexcept -> Camera*
    {
        return m_systemResources.cameras.Get();
    }

    void NcGraphicsImpl::SetUi(ui::IUI* ui) noexcept
    {
        NC_LOG_TRACE("Setting UI to {}", static_cast<void*>(ui));
        m_systemResources.ui.SetClientUI(ui);
    }

    bool NcGraphicsImpl::IsUiHovered() const noexcept
    {
        return m_systemResources.ui.IsHovered();
    }

    void NcGraphicsImpl::SetSkybox(const std::string& path)
    {
        NC_LOG_TRACE("Setting skybox to {}", path);
        m_systemResources.environment.SetSkybox(path);
    }

    void NcGraphicsImpl::ClearEnvironment()
    {
        m_systemResources.environment.Clear();
    }

    void NcGraphicsImpl::Clear() noexcept
    {
        /** @note Don't clear the camera as it may be on a persistent entity. */
        /** @todo graphics::clear not marked noexcept */
        m_systemResources.particleEmitters.Clear();
        m_graphics->Clear();
        m_systemResources.cameras.Clear();
        m_systemResources.environment.Clear();
        m_systemResources.lights.Clear();
        m_systemResources.skeletalAnimations.Clear();
    }

    void NcGraphicsImpl::OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks& render)
    {
        NC_LOG_TRACE("Building NcGraphics Tasks");

#if NC_DEBUG_RENDERING_ENABLED
         // We must express FrameLogicUpdate dependency here (as a predecessor) so we don't have
         // a missing task id in prod builds where this is excluded.
        update.Add(
            update_task_id::DebugRendererNewFrame,
            "DebugRendererNewFrame",
            debug::DebugRendererNewFrame,
            {},
            {update_task_id::FrameLogicUpdate}
        );
#endif
        update.Add(
            update_task_id::ParticleEmitterUpdate,
            "ParticleEmitterUpdate",
            [this]{ m_systemResources.particleEmitters.UpdateParticles(); }
        );

        update.Add(
            update_task_id::ParticleEmitterSync,
            "ParticleEmitterSync",
            [this]{ m_systemResources.particleEmitters.ProcessFrameEvents(); },
            {update_task_id::CommitStagedChanges}
        );

        render.Add(
            render_task_id::Render,
            "Render",
            [this]{ Run(); }
        );
    }

    void NcGraphicsImpl::Run()
    {
        NC_PROFILE_TASK("Render", ProfileCategory::Rendering);

        // Wait until the frame is ready to be rendered, begin accepting ImGui commands
        if (!m_graphics->PrepareFrame())
        {
            return;
        }

        auto currentFrameIndex = m_graphics->CurrentFrameIndex();

        // Run all the systems to generate this frame's resource data.
        m_systemResources.ui.Execute(ecs::Ecs(m_registry->GetImpl()));
        auto cameraState = m_systemResources.cameras.Execute(m_registry);
        auto widgetState = m_systemResources.widgets.Execute(m_registry->GetEcs());
        auto environmentState = m_systemResources.environment.Execute(cameraState, currentFrameIndex);
        auto skeletalAnimationState = m_systemResources.skeletalAnimations.Execute(currentFrameIndex);
        auto objectState = m_systemResources.objects.Execute(currentFrameIndex, MultiView<MeshRenderer, Transform>{m_registry}, MultiView<ToonRenderer, Transform>{m_registry},
                                                                        cameraState, environmentState, skeletalAnimationState);
        auto lightState = m_systemResources.lights.Execute(currentFrameIndex, MultiView<PointLight, Transform>{m_registry}, MultiView<SpotLight, Transform>{m_registry});
        auto particleState = m_systemResources.particleEmitters.Execute(currentFrameIndex);

        // Collect all the resource data for this frame.
        auto state = PerFrameRenderState
        {
            std::move(cameraState),
            std::move(environmentState),
            std::move(lightState),
            std::move(objectState),
            std::move(widgetState),
            std::move(particleState)
        };

        auto stateData = PerFrameRenderStateData
        {
            state.environmentState.useSkybox,
            state.lightState.omniDirectionalLightCount,
            state.lightState.uniDirectionalLightCount,
            static_cast<uint32_t>(state.objectState.pbrMeshes.size()),
            static_cast<uint32_t>(state.objectState.toonMeshes.size()),
            static_cast<uint32_t>(state.widgetState.wireframeData.size()),
            state.particleState.count
        };

        // Build the pipelines and renderpasses depending on which render state was generated.
        m_graphics->BuildRenderGraph(stateData);

        // Allow the frame to begin accepting draw commands.
        if (!m_graphics->BeginFrame())
        {
            return;
        }

        // Bind mesh buffer to the current frame.
        m_assetResources.meshes.Bind(currentFrameIndex);

        // Draw all the resource data
        m_graphics->DrawFrame(state);

        // Submit the frame to be presented and increment the frame index.
        m_graphics->FrameEnd();
    }

    void NcGraphicsImpl::OnResize(const Vector2& dimensions, bool isMinimized)
    {
        m_systemResources.cameras.Get()->UpdateProjectionMatrix(dimensions.x, dimensions.y);
        m_graphics->OnResize(dimensions, isMinimized);
    }
} // namespace nc::graphics

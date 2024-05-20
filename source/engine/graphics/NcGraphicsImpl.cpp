#include "NcGraphicsImpl.h"
#include "graphics/PerFrameRenderState.h"
#include "graphics/shader_resource/ShaderResourceBus.h"
#include "window/WindowImpl.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/config/Config.h"
#include "ncengine/debug/DebugRendering.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/View.h"
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/utility/Log.h"

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
                             SystemEvents& events,
                             window::WindowImpl* window) -> std::unique_ptr<NcGraphics>
    {
        if (graphicsSettings.enabled)
        {
            auto ncAsset = modules.Get<asset::NcAsset>();
            NC_ASSERT(ncAsset, "NcGraphics requires NcAsset to be registered before it.");
            NC_ASSERT(modules.Get<NcScene>(), "NcGraphics requires NcScene to be registered before it.");

            NC_LOG_TRACE("Selecting Graphics API");
            auto resourceBus = ShaderResourceBus{};
            auto graphicsApi = GraphicsFactory(projectSettings, graphicsSettings, memorySettings, ncAsset, resourceBus, registry, window);

            NC_LOG_TRACE("Building NcGraphics module");
            return std::make_unique<NcGraphicsImpl>(graphicsSettings, memorySettings, registry, modules, events, std::move(graphicsApi), std::move(resourceBus), window);
        }

        NC_LOG_TRACE("Graphics disabled - building NcGraphics stub");
        return std::make_unique<NcGraphicsStub>(registry);
    }

    NcGraphicsImpl::NcGraphicsImpl(const config::GraphicsSettings& graphicsSettings,
                                   const config::MemorySettings& memorySettings,
                                   Registry* registry,
                                   ModuleProvider modules,
                                   SystemEvents& events,
                                   std::unique_ptr<IGraphics> graphics,
                                   ShaderResourceBus shaderResourceBus,
                                   window::WindowImpl* window)
        : m_registry{registry},
          m_graphics{std::move(graphics)},
          m_shaderResourceBus{std::move(shaderResourceBus)},
          m_assetResources{AssetResourcesConfig{memorySettings}, &m_shaderResourceBus, modules.Get<asset::NcAsset>()},
          m_postProcessResources{memorySettings.maxPointLights, &m_shaderResourceBus},
          m_systemResources{SystemResourcesConfig{graphicsSettings, memorySettings}, m_registry, &m_shaderResourceBus, modules, events, std::bind_front(&NcGraphics::GetCamera, this)}
    {
        window->BindGraphicsOnResizeCallback(std::bind_front(&NcGraphicsImpl::OnResize, this));

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
        m_postProcessResources.shadowMaps.Clear();
        m_systemResources.cameras.Clear();
        m_systemResources.environment.Clear();
        m_systemResources.pointLights.Clear();
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
        OPTICK_CATEGORY("Render", Optick::Category::Rendering);

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
        auto lightingState = m_systemResources.pointLights.Execute(currentFrameIndex, MultiView<PointLight, Transform>{m_registry});
        auto particleState = m_systemResources.particleEmitters.Execute(currentFrameIndex);

        // If any changes were made to resource layouts (point lights added or removed, textures added, etc) that require an update of that resource layout, do so now.
        m_graphics->CommitResourceLayout();

        if (lightingState.updateShadows)
        {
            m_postProcessResources.shadowMaps.Update(static_cast<uint32_t>(lightingState.viewProjections.size()), currentFrameIndex);
        }

        // Allow the frame to begin accepting draw commands.
        if (!m_graphics->BeginFrame())
        {
            return;
        }

        // Bind mesh buffer to the current frame.
        m_assetResources.meshes.Bind(currentFrameIndex);

        // Collect all the resource data for this frame.
        auto state = PerFrameRenderState
        {
            std::move(cameraState),
            std::move(environmentState),
            std::move(objectState),
            std::move(lightingState),
            std::move(widgetState),
            std::move(particleState)
        };

        // Draw all the resource data
        m_graphics->DrawFrame(state);

        // Submit the frame to be presented and increment the frame index.
        m_graphics->FrameEnd();
    }

    void NcGraphicsImpl::OnResize(float width, float height, bool isMinimized)
    {
        m_systemResources.cameras.Get()->UpdateProjectionMatrix(width, height);
        m_graphics->OnResize(width, height, isMinimized);
    }
} // namespace nc::graphics

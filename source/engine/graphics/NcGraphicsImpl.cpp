#include "NcGraphicsImpl.h"
#include "PerFrameRenderState.h"
#include "GraphicsUtilities.h"
#include "shader_resource/ShaderResourceBus.h"
#include "window/WindowImpl.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/View.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/utility/Log.h"

#include "optick.h"

#ifdef NC_DEBUG_RENDERING_ENABLED
#include "graphics/debug/DebugRenderer.h"
#endif

namespace
{
    struct NcGraphicsStub : nc::graphics::NcGraphics
    {
        NcGraphicsStub(nc::Registry*) {}

        void SetCamera(nc::graphics::Camera*) noexcept override {}
        auto GetCamera() noexcept -> nc::graphics::Camera* override { return nullptr; }
        void SetUi(nc::ui::IUI*) noexcept override {}
        bool IsUiHovered() const noexcept override { return false; }
        void SetSkybox(const std::string&) override {}
        void ClearEnvironment() override {}

        /** @todo Debug renderer is becoming a problem... */
        #ifdef NC_DEBUG_RENDERING_ENABLED
        nc::graphics::DebugRenderer debugRenderer;
        #endif
    };
} // anonymous namespace

namespace nc::graphics
{
    auto BuildGraphicsModule(const config::ProjectSettings& projectSettings,
                             const config::GraphicsSettings& graphicsSettings,
                             const config::MemorySettings& memorySettings,
                             ModuleProvider modules,
                             Registry* registry,
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
            return std::make_unique<NcGraphicsImpl>(graphicsSettings, memorySettings, registry, modules, std::move(graphicsApi), std::move(resourceBus), window);
        }

        NC_LOG_TRACE("Graphics disabled - building NcGraphics stub");
        return std::make_unique<NcGraphicsStub>(registry);
    }

    NcGraphicsImpl::NcGraphicsImpl(const config::GraphicsSettings& graphicsSettings,
                                   const config::MemorySettings& memorySettings,
                                   Registry* registry,
                                   ModuleProvider modules,
                                   std::unique_ptr<IGraphics> graphics,
                                   ShaderResourceBus shaderResourceBus,
                                   window::WindowImpl* window)
        : m_registry{registry},
          m_graphics{std::move(graphics)},
          m_shaderResourceBus{std::move(shaderResourceBus)},
          m_cameraSystem{},
          m_environmentSystem{&m_shaderResourceBus},
          m_objectSystem{&m_shaderResourceBus, memorySettings.maxRenderers},
          m_pointLightSystem{std::move(shaderResourceBus.pointLightChannel), graphicsSettings.useShadows},
          m_particleEmitterSystem{ registry, std::bind_front(&NcGraphics::GetCamera, this) },
          m_skeletalAnimationSystem{registry,
                                    modules.Get<asset::NcAsset>()->OnSkeletalAnimationUpdate(),
                                    modules.Get<asset::NcAsset>()->OnBoneUpdate(),
                                    std::move(shaderResourceBus.skeletalAnimationChannel)},
          m_widgetSystem{},
          m_uiSystem{registry->GetEcs(), modules}
    {
        window->BindGraphicsOnResizeCallback(std::bind_front(&NcGraphicsImpl::OnResize, this));
    }

    void NcGraphicsImpl::SetCamera(Camera* camera) noexcept
    {
        NC_LOG_TRACE("Setting main camera to: {}", static_cast<void*>(camera));
        m_cameraSystem.Set(camera);
    }

    auto NcGraphicsImpl::GetCamera() noexcept -> Camera*
    {
        return m_cameraSystem.Get();
    }

    void NcGraphicsImpl::SetUi(ui::IUI* ui) noexcept
    {
        NC_LOG_TRACE("Setting UI to {}", static_cast<void*>(ui));
        m_uiSystem.SetClientUI(ui);
    }

    bool NcGraphicsImpl::IsUiHovered() const noexcept
    {
        return m_uiSystem.IsHovered();
    }

    void NcGraphicsImpl::SetSkybox(const std::string& path)
    {
        NC_LOG_TRACE("Setting skybox to {}", path);
        m_environmentSystem.SetSkybox(path);
    }

    void NcGraphicsImpl::ClearEnvironment()
    {
        m_environmentSystem.Clear();
    }

    void NcGraphicsImpl::Clear() noexcept
    {
        /** @note Don't clear the camera as it may be on a persistent entity. */
        /** @todo graphics::clear not marked noexcept */
        m_graphics->Clear();
        m_cameraSystem.Clear();
        m_environmentSystem.Clear();
        m_particleEmitterSystem.Clear();
        m_skeletalAnimationSystem.Clear();
    }

    void NcGraphicsImpl::OnBuildTaskGraph(task::TaskGraph& graph)
    {
        NC_LOG_TRACE("Building NcGraphics workload");

        graph.Add(task::ExecutionPhase::Render, "NcGraphics", [this]{ Run(); });
        graph.Add(task::ExecutionPhase::Free, "ParticleEmitterSystem", [this]{ m_particleEmitterSystem.Run(); });
        graph.Add(task::ExecutionPhase::PostFrameSync, "ProcessParticleFrameEvents", [this]{ m_particleEmitterSystem.ProcessFrameEvents(); } );
    }

    void NcGraphicsImpl::Run()
    {
        OPTICK_CATEGORY("Render", Optick::Category::Rendering);
        if (!m_graphics->FrameBegin())
        {
            return;
        }

        auto currentFrameIndex = m_graphics->CurrentFrameIndex();

        auto cameraState = m_cameraSystem.Execute(m_registry);
        m_uiSystem.Execute(ecs::Ecs(m_registry->GetImpl()));
        auto widgetState = m_widgetSystem.Execute(View<physics::Collider>{m_registry});
        auto environmentState = m_environmentSystem.Execute(cameraState, currentFrameIndex);
        auto skeletalAnimationState = m_skeletalAnimationSystem.Execute();
        auto objectState = m_objectSystem.Execute(currentFrameIndex,
                                                  MultiView<MeshRenderer, Transform>{m_registry},
                                                  MultiView<ToonRenderer, Transform>{m_registry},
                                                  cameraState,
                                                  environmentState,
                                                  skeletalAnimationState);
        auto lightingState = m_pointLightSystem.Execute(currentFrameIndex, MultiView<PointLight, Transform>{m_registry});
        auto state = PerFrameRenderState
        {
            std::move(cameraState),
            std::move(environmentState),
            std::move(objectState),
            std::move(lightingState),
            std::move(widgetState),
            m_particleEmitterSystem.GetParticles()
        };

        m_graphics->Draw(state);
        m_graphics->FrameEnd();
    }

    void NcGraphicsImpl::OnResize(float width, float height, bool isMinimized)
    {
        m_cameraSystem.Get()->UpdateProjectionMatrix(width, height);
        m_graphics->OnResize(width, height, isMinimized);
    }
} // namespace nc::graphics

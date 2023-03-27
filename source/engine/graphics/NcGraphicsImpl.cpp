#include "NcGraphicsImpl.h"
#include "PerFrameRenderState.h"
#include "asset/NcAsset.h"
#include "config/Config.h"
#include "ecs/View.h"

#include "shader_resource/PointLightData.h"
#include "shader_resource/ShaderResourceBus.h"

#include "task/Job.h"
#include "utility/Log.h"
#include "window/WindowImpl.h"

#include "optick/optick.h"

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
        void Clear() noexcept override {}
        auto BuildWorkload() -> std::vector<nc::task::Job> { return {}; }

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
                             asset::NcAsset* assetModule,
                             Registry* registry,
                             window::WindowImpl* window) -> std::unique_ptr<NcGraphics>
    {
        if (graphicsSettings.enabled)
        {
            NC_LOG_TRACE("Selecting Graphics API");
            auto resourceBus = ShaderResourceBus{};
            auto graphicsApi = GraphicsFactory(projectSettings, graphicsSettings, assetModule, resourceBus, registry, window);

            NC_LOG_TRACE("Building NcGraphics module");
            return std::make_unique<NcGraphicsImpl>(graphicsSettings, registry, std::move(graphicsApi), std::move(resourceBus), window);
        }

        NC_LOG_TRACE("Graphics disabled - building NcGraphics stub");
        return std::make_unique<NcGraphicsStub>(registry);
    }

    NcGraphicsImpl::NcGraphicsImpl(const config::GraphicsSettings& graphicsSettings,
                                   Registry* registry,
                                   std::unique_ptr<IGraphics> graphics,
                                   ShaderResourceBus&& shaderResourceBus,
                                   window::WindowImpl* window)
        : m_registry{registry},
          m_graphics{std::move(graphics)},
          m_ui{window->GetWindow()},
          m_cameraSystem{},
          m_environmentSystem{std::move(shaderResourceBus.environmentChannel)},
          m_objectSystem{std::move(shaderResourceBus.objectChannel)},
          m_pointLightSystem{std::move(shaderResourceBus.pointLightChannel), graphicsSettings.useShadows},
          m_particleEmitterSystem{ registry, std::bind_front(&NcGraphics::GetCamera, this) },
          m_widgetSystem{}
    {
        m_graphics->InitializeUI();
        window->BindGraphicsOnResizeCallback(std::bind_front(&NcGraphicsImpl::OnResize, this));
    }

    void NcGraphicsImpl::SetCamera(Camera* camera) noexcept
    {
        NC_LOG_TRACE_FMT("Setting main camera to: {}", static_cast<void*>(camera));
        m_cameraSystem.Set(camera);
    }

    auto NcGraphicsImpl::GetCamera() noexcept -> Camera*
    {
        return m_cameraSystem.Get();
    }

    void NcGraphicsImpl::SetUi(ui::IUI* ui) noexcept
    {
        NC_LOG_TRACE_FMT("Setting UI to {}", static_cast<void*>(ui));
        m_ui.Set(ui);
    }

    bool NcGraphicsImpl::IsUiHovered() const noexcept
    {
        return m_ui.IsHovered();
    }

    void NcGraphicsImpl::SetSkybox(const std::string& path)
    {
        NC_LOG_TRACE_FMT("Setting skybox to {}", path);
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
        m_environmentSystem.Clear();
        m_particleEmitterSystem.Clear();
    }

    auto NcGraphicsImpl::BuildWorkload() -> std::vector<task::Job>
    {
        NC_LOG_TRACE("Building NcGraphics workload");
        return std::vector<task::Job>
        {
            task::Job{ [this] {Run(); }, "GraphicsModule", task::ExecutionPhase::Render },
            task::Job{ [this] {m_particleEmitterSystem.Run(); }, "RunParticleEmitterSystem", task::ExecutionPhase::Free },
            task::Job{ [this] {m_particleEmitterSystem.ProcessFrameEvents(); }, "ProcessParticleFrameEvents", task::ExecutionPhase::PostFrameSync }
        };
    }

    void NcGraphicsImpl::Run()
    {
        OPTICK_CATEGORY("Render", Optick::Category::Rendering);
        auto cameraState = m_cameraSystem.Execute(m_registry);
        if (!cameraState.hasCamera || !m_graphics->FrameBegin())
        {
            return;
        }

        m_ui.FrameBegin();

        #ifdef NC_EDITOR_ENABLED
        /** @todo I think the old editor can start to go away. I'm
         *  hacking dt factor in here. */
        float dtFactor = 1.0f;
        m_ui.Draw(&dtFactor, m_registry);
        auto widgetState = m_widgetSystem.Execute(View<physics::Collider>{m_registry});
        #else
        m_ui.Draw();
        auto widgetState = WidgetState{std::nullopt};
        #endif

        auto environmentState = m_environmentSystem.Execute(cameraState);
        auto objectState = m_objectSystem.Execute(MultiView<MeshRenderer, Transform>{m_registry}, cameraState, environmentState);
        auto lightingState = m_pointLightSystem.Execute(MultiView<PointLight, Transform>{m_registry});
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

    void NcGraphicsImpl::OnResize(float width, float height, float nearZ, float farZ, bool isMinimized)
    {
        m_cameraSystem.Get()->UpdateProjectionMatrix(width, height, nearZ, farZ);
        m_graphics->OnResize(width, height, isMinimized);
    }
} // namespace nc::graphics

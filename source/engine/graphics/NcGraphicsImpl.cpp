#include "NcGraphicsImpl.h"
#include "assets/AssetManagers.h"
#include "ecs/View.h"
#include "PerFrameRenderState.h"
#include "utility/Log.h"
#include "window/WindowImpl.h"

#include "optick/optick.h"

#ifdef NC_DEBUG_RENDERING_ENABLED
#include "graphics/DebugRenderer.h"
#endif

namespace
{
    struct NcGraphicsStub : nc::graphics::NcGraphics
    {
        NcGraphicsStub(nc::Registry* reg)
            : onAddConnection{reg->OnAdd<nc::graphics::PointLight>().Connect([](nc::graphics::PointLight&){})},
              onRemoveConnection{reg->OnRemove<nc::graphics::PointLight>().Connect([](nc::Entity){})}
        {
        }

        void SetCamera(nc::graphics::Camera*) noexcept override {}
        auto GetCamera() noexcept -> nc::graphics::Camera* override { return nullptr; }
        void SetUi(nc::ui::IUI*) noexcept override {}
        bool IsUiHovered() const noexcept override { return false; }
        void SetSkybox(const std::string&) override {}
        void ClearEnvironment() override {}
        void Clear() noexcept override {}
        auto BuildWorkload() -> std::vector<nc::task::Job> { return {}; }

        nc::Connection<nc::graphics::PointLight&> onAddConnection;
        nc::Connection<nc::Entity> onRemoveConnection;
        /** @todo Debug renderer is becoming a problem... */
        #ifdef NC_DEBUG_RENDERING_ENABLED
        nc::graphics::DebugRenderer debugRenderer;
        #endif
    };

    auto ToVulkanApi(nc::graphics::GraphicsApi api) -> uint32_t
    {
        switch(api)
        {
            case nc::graphics::GraphicsApi::Vulkan_1_0: return VK_API_VERSION_1_0;
            case nc::graphics::GraphicsApi::Vulkan_1_1: return VK_API_VERSION_1_1;
            case nc::graphics::GraphicsApi::Vulkan_1_2: return VK_API_VERSION_1_2;
        }

        throw nc::NcError{"Unknown GraphicsApi"};
    }
} // anonymous namespace

namespace nc::graphics
{
    auto BuildGraphicsModule(bool enableModule, Registry* registry, GraphicsInitInfo info, window::WindowImpl* window) -> std::unique_ptr<NcGraphics>
    {
        if (enableModule)
        {
            NC_LOG_TRACE("Creating NcGraphics module");
            return std::make_unique<NcGraphicsImpl>(registry, std::move(info), window);
        }

        NC_LOG_TRACE("Creating NcGraphics module stub");
        return std::make_unique<NcGraphicsStub>(registry);
    }

    NcGraphicsImpl::NcGraphicsImpl(Registry* registry, GraphicsInitInfo info, window::WindowImpl* window)
        : m_registry{ registry },
          // TODO #341: Instead of constructing here, pass in from BuildGraphicsModule
          m_graphics{ registry,
                      info.gpuAccessorSignals,
                      info.appName,
                      info.appVersion,
                      ::ToVulkanApi(info.api),
                      info.useValidationLayers,
                      window->GetGlfwWindow(),
                      window->GetDimensions()},
          m_ui{window->GetGlfwWindow()},
          m_environment{},
          m_pointLightSystem{ registry },
          m_particleEmitterSystem{ registry, std::bind_front(&NcGraphics::GetCamera, this) }
    {
        m_graphics.InitializeUI();
        window->BindGraphicsOnResizeCallback(std::bind_front(&NcGraphicsImpl::OnResize, this));
        window->BindUICallback(std::bind_front(&ui::UISystemImpl::WndProc, &m_ui));
    }

    void NcGraphicsImpl::SetCamera(Camera* camera) noexcept
    {
        NC_LOG_TRACE_FMT("Setting main camera to: {}", static_cast<void*>(camera));
        m_camera.Set(camera);
    }

    auto NcGraphicsImpl::GetCamera() noexcept -> Camera*
    {
        return m_camera.Get();
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
        m_environment.SetSkybox(path);
    }

    void NcGraphicsImpl::ClearEnvironment()
    {
        m_environment.Clear();
    }

    void NcGraphicsImpl::Clear() noexcept
    {
        /** @note Don't clear the camera as it may be on a persistent entity. */
        /** @todo graphics::clear not marked noexcept */
        m_graphics.Clear();
        m_environment.Clear();
        m_pointLightSystem.Clear();
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
        auto* camera = m_camera.Get();
        if (!camera)
        {
            return;
        }

        camera->UpdateViewMatrix();

        if(!m_graphics.FrameBegin())
            return;

        m_ui.FrameBegin();

        #ifdef NC_EDITOR_ENABLED
        /** @todo I think the old editor can start to go away. I'm
         *  hacking dt factor in here. */
        float dtFactor = 1.0f;
        m_ui.Draw(&dtFactor, m_registry);
        #else
        m_ui.Draw();
        #endif

        auto areLightsDirty = m_pointLightSystem.CheckDirtyAndReset();
        auto state = PerFrameRenderState{ m_registry, camera, areLightsDirty, &m_environment, m_particleEmitterSystem.GetParticles() };
        MapPerFrameRenderState(state);
        m_graphics.Draw(state);

        #ifdef NC_EDITOR_ENABLED
        for(auto& collider : View<physics::Collider>{m_registry}) collider.SetEditorSelection(false);
        #endif

        m_graphics.FrameEnd();
    }

    void NcGraphicsImpl::OnResize(float width, float height, float nearZ, float farZ, WPARAM windowArg)
    {
        m_camera.Get()->UpdateProjectionMatrix(width, height, nearZ, farZ);
        m_graphics.OnResize(width, height, windowArg);
    }
} // namespace nc::graphics

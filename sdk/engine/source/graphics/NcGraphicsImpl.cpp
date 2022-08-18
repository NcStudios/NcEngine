#include "NcGraphicsImpl.h"
#include "assets/AssetManagers.h"
#include "ecs/View.h"
#include "PerFrameRenderState.h"
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
} // anonymous namespace

namespace nc::graphics
{
    auto BuildGraphicsModule(bool enableModule, Registry* reg, const nc::GpuAccessorSignals& gpuAccessorSignals, window::WindowImpl* window) -> std::unique_ptr<NcGraphics>
    {
        if(enableModule) return std::make_unique<NcGraphicsImpl>(reg, gpuAccessorSignals, window);
        return std::make_unique<NcGraphicsStub>(reg);
    }

    NcGraphicsImpl::NcGraphicsImpl(Registry* registry, const nc::GpuAccessorSignals& gpuAccessorSignals, window::WindowImpl* window)
        : m_registry{ registry },
          m_camera{},
          m_graphics{ &m_camera,
                      gpuAccessorSignals,
                      window->GetHWND(),
                      window->GetHINSTANCE(),
                      window->GetDimensions() },
          m_ui{ window->GetHWND() },
          m_environment{},
          m_pointLightSystem{ registry },
          m_particleEmitterSystem{ registry, std::bind_front(&NcGraphics::GetCamera, this) }
    {
        m_graphics.InitializeUI();
        window->BindGraphicsOnResizeCallback(std::bind_front(&Graphics::OnResize, &m_graphics));
        window->BindGraphicsSetClearColorCallback(std::bind_front(&Graphics::SetClearColor, &m_graphics));
        window->BindUICallback(std::bind_front(&ui::UISystemImpl::WndProc, &m_ui));
    }

    void NcGraphicsImpl::SetCamera(Camera* camera) noexcept
    {
        m_camera.Set(camera);
    }

    auto NcGraphicsImpl::GetCamera() noexcept -> Camera*
    {
        return m_camera.Get();
    }

    void NcGraphicsImpl::SetUi(ui::IUI* ui) noexcept
    {
        m_ui.Set(ui);
    }

    bool NcGraphicsImpl::IsUiHovered() const noexcept
    {
        return m_ui.IsHovered();
    }

    void NcGraphicsImpl::SetSkybox(const std::string& path)
    {
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
} // namespace nc::graphics

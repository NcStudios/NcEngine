#include "GraphicsModuleImpl.h"
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
    struct GraphicsModuleStub : nc::GraphicsModule
    {
        GraphicsModuleStub(nc::Registry* reg)
        {
            reg->RegisterOnAddCallback<nc::PointLight>([](nc::PointLight&){});
            reg->RegisterOnRemoveCallback<nc::PointLight>([](nc::Entity){});
        }

        void SetCamera(nc::Camera*) noexcept override {}
        auto GetCamera() noexcept -> nc::Camera* override { return nullptr; }
        void SetUi(nc::ui::IUI*) noexcept override {}
        bool IsUiHovered() const noexcept override { return false; }
        void SetSkybox(const std::string&) override {}
        void ClearEnvironment() override {}
        void Clear() noexcept override {}
        auto BuildWorkload() -> std::vector<nc::Job> { return {}; }

        /** @todo Debug renderer is becoming a problem... */
        #ifdef NC_DEBUG_RENDERING_ENABLED
        nc::graphics::DebugRenderer debugRenderer;
        #endif
    };
}

namespace nc::graphics
{
    auto BuildGraphicsModule(bool enableModule, Registry* reg, const nc::GpuAccessorSignals& gpuAccessorSignals, window::WindowImpl* window, float* dt) -> std::unique_ptr<GraphicsModule>
    {
        if(enableModule) return std::make_unique<GraphicsModuleImpl>(reg, gpuAccessorSignals, window, dt);
        return std::make_unique<GraphicsModuleStub>(reg);
    }

    GraphicsModuleImpl::GraphicsModuleImpl(Registry* registry, const nc::GpuAccessorSignals& gpuAccessorSignals, window::WindowImpl* window, float* dt)
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
          m_particleEmitterSystem{ registry, dt, std::bind_front(&GraphicsModule::GetCamera, this) }
    {
        m_graphics.InitializeUI();
        window->BindGraphicsOnResizeCallback(std::bind_front(&Graphics::OnResize, &m_graphics));
        window->BindGraphicsSetClearColorCallback(std::bind_front(&Graphics::SetClearColor, &m_graphics));
        window->BindUICallback(std::bind_front(&ui::UISystemImpl::WndProc, &m_ui));
    }

    void GraphicsModuleImpl::SetCamera(Camera* camera) noexcept
    {
        m_camera.Set(camera);
    }

    auto GraphicsModuleImpl::GetCamera() noexcept -> Camera*
    {
        return m_camera.Get();
    }

    void GraphicsModuleImpl::SetUi(ui::IUI* ui) noexcept
    {
        m_ui.Set(ui);
    }

    bool GraphicsModuleImpl::IsUiHovered() const noexcept
    {
        return m_ui.IsHovered();
    }

    void GraphicsModuleImpl::SetSkybox(const std::string& path)
    {
        m_environment.SetSkybox(path);
    }

    void GraphicsModuleImpl::ClearEnvironment()
    {
        m_environment.Clear();
    }

    void GraphicsModuleImpl::Clear() noexcept
    {
        /** @note Don't clear the camera as it may be on a persistent entity. */
        /** @todo graphics::clear not marked noexcept */
        m_graphics.Clear();
        m_environment.Clear();
        m_pointLightSystem.Clear();
        m_particleEmitterSystem.Clear();
    }

    auto GraphicsModuleImpl::BuildWorkload() -> std::vector<Job>
    {
        return std::vector<Job>
        {
            Job{ [this] {Run(); }, "GraphicsModule", HookPoint::Render },
            Job{ [this] {m_particleEmitterSystem.Run(); }, "RunParticleEmitterSystem", HookPoint::Free },
            Job{ [this] {m_particleEmitterSystem.ProcessFrameEvents(); }, "ProcessParticleFrameEvents", HookPoint::PostFrameSync }
        };
    }

    void GraphicsModuleImpl::Run()
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
        for(auto& collider : View<Collider>{m_registry}) collider.SetEditorSelection(false);
        #endif

        m_graphics.FrameEnd();
    }
}
#include "GraphicsModuleImpl.h"
#include "ecs/view.h"
#include "PerFrameRenderState.h"
#include "window/WindowImpl.h"

#include "optick/optick.h"

namespace nc::graphics
{
    auto BuildGraphicsModule(bool enableModule, Registry* reg, window::WindowImpl* window) -> std::unique_ptr<GraphicsModule>
    {
        /** @todo allow stub */
        (void)enableModule;

        return std::make_unique<GraphicsModuleImpl>(reg, window);
    }

    GraphicsModuleImpl::GraphicsModuleImpl(Registry* registry, window::WindowImpl* window)
        : m_registry{registry},
          m_camera{},
          m_graphics{&m_camera,
                     window->GetHWND(),
                     window->GetHINSTANCE(),
                     window->GetDimensions()},
          m_ui{window->GetHWND()},
          m_environment{},
          m_pointLightSystem{registry}
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
    }

    auto GraphicsModuleImpl::BuildWorkload() -> std::vector<Job>
    {
        return std::vector<Job>
        {
            Job{ [this]{Run();}, "GraphicsModule", HookPoint::Render }
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
        auto state = PerFrameRenderState{m_registry, camera, areLightsDirty, &m_environment};
        MapPerFrameRenderState(state);
        m_graphics.Draw(state);

#ifdef NC_EDITOR_ENABLED
        for(auto& collider : view<Collider>{m_registry}) collider.SetEditorSelection(false);
#endif

        m_graphics.FrameEnd();
    }
}
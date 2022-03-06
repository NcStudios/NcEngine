#include "graphics_module_impl.h"
#include "ecs/view.h"
#include "PerFrameRenderState.h"
#include "window/WindowImpl.h"

namespace nc::graphics
{
    auto build_graphics_module(bool enableModule, Registry* reg, window::WindowImpl* window) -> std::unique_ptr<graphics_module>
    {
        /** @todo allow stub */
        (void)enableModule;

        return std::make_unique<graphics_module_impl>(reg, window);
    }

    graphics_module_impl::graphics_module_impl(Registry* reg, window::WindowImpl* window)
        : m_camera{},
          m_graphics{&m_camera,
                     window->GetHWND(),
                     window->GetHINSTANCE(),
                     window->GetDimensions()},
          m_ui{window->GetHWND()},
          m_environment{},
          m_pointLightSystem{reg}
    {
        m_graphics.InitializeUI();
        window->BindGraphicsOnResizeCallback(std::bind_front(&Graphics::OnResize, &m_graphics));
        window->BindGraphicsSetClearColorCallback(std::bind_front(&Graphics::SetClearColor, &m_graphics));
        window->BindUICallback(std::bind_front(&ui::UISystemImpl::WndProc, &m_ui));
    }

    void graphics_module_impl::set_camera(Camera* camera) noexcept
    {
        m_camera.Set(camera);
    }

    auto graphics_module_impl::get_camera() noexcept -> Camera*
    {
        return m_camera.Get();
    }

    void graphics_module_impl::set_ui(ui::IUI* ui) noexcept
    {
        m_ui.Set(ui);
    }

    bool graphics_module_impl::is_ui_hovered() const noexcept
    {
        return m_ui.IsHovered();
    }

    void graphics_module_impl::set_skybox(const std::string& path)
    {
        m_environment.SetSkybox(path);
    }

    void graphics_module_impl::clear_environment()
    {
        m_environment.Clear();
    }

    void graphics_module_impl::clear() noexcept
    {
        /** @note Don't clear the camera as it may be on a persistent entity. */
        /** @todo graphics::clear not marked noexcept */
        m_graphics.Clear();
        m_environment.Clear();
        m_pointLightSystem.Clear();
    }

    void graphics_module_impl::run(Registry* reg)
    {
        auto* camera = m_camera.Get();
        camera->UpdateViewMatrix();

        if(!m_graphics.FrameBegin())
            return;

        m_ui.FrameBegin();

#ifdef NC_EDITOR_ENABLED
        /** @todo I think the old editor can start to go away. I'm
         *  hacking dt factor in here. */
        float dtFactor = 1.0f;
        m_ui.Draw(&dtFactor, reg);
#else
        m_ui.Draw();
#endif

        auto areLightsDirty = m_pointLightSystem.CheckDirtyAndReset();
        auto state = PerFrameRenderState{reg, camera, areLightsDirty, &m_environment};
        MapPerFrameRenderState(state);
        m_graphics.Draw(state);

#ifdef NC_EDITOR_ENABLED
        for(auto& collider : view<Collider>{reg}) collider.SetEditorSelection(false);
#endif

        m_graphics.FrameEnd();
    }
}
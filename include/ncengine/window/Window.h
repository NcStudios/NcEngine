/**
 * @file Window.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/module/Module.h"
#include "ncengine/type/EngineId.h"
#include "ncengine/utility/Signal.h"
#include "ncengine/window/IOnResizeReceiver.h"

#include "ncmath/Vector.h"

struct GLFWwindow;

namespace nc
{
namespace config
{
struct ProjectSettings;
struct GraphicsSettings;
} // namespace config

namespace window
{
/** @brief Get the window dimensions */
auto GetDimensions() -> Vector2;

/** @brief Get the dimensions of the screen after aspect ratio transformations. */
auto GetScreenExtent() -> Vector2;

/** @brief Get the current DPI. */
auto GetContentScale() -> Vector2;

/** @brief Convert screen space coordinates to normalized device coordinates (in the range [-1, 1]). */
auto ToNormalizedDeviceCoordinates(const Vector2& screenCoordinates) -> Vector2;

/**
 * @brief Allow an object to receive window resize events. Receivers must be unregistered before they are destroyed.
 * @deprecated Prefer using NcWindow::OnResize().
 */
void RegisterOnResizeReceiver(IOnResizeReceiver* receiver);

/**
 * @brief Unregister an object from receiving window resize events.
 * @deprecated Prefer using NcWindow::OnResize().
 */
void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept;

/** @brief Window module interface.
 * 
 */
class NcWindow : public Module
{
    public:
        explicit NcWindow() noexcept
            : Module{NcWindowId} {}

        /** @brief Get the dimensions of the entire window. */
        auto GetDimensions() const noexcept -> const Vector2& { return m_dimensions; }

        /** @brief Get the dimensions of the renderable window region. */
        auto GetScreenExtent() const noexcept -> const Vector2& { return m_screenExtent; }

        /** @brief Get the ratio between the current DPI and the platform's default DPI. */
        auto GetContentScale() const noexcept -> const Vector2& { return m_contentScale; }

        /** @brief Get the GLFW window handle. */
        auto GetWindowHandle() const noexcept -> GLFWwindow* { return m_window; }

        /** @brief Get the Signal for window resize events. */
        auto OnResize() noexcept -> Signal<const Vector2&, bool>& { return m_onResize; }

        /**
         * @brief Process window and input events.
         * @note This is called by NcEngine each frame.
         */
        virtual void ProcessSystemMessages() = 0;

    protected:
        Vector2 m_dimensions;
        Vector2 m_screenExtent;
        Vector2 m_contentScale;
        GLFWwindow* m_window;
        Signal<const Vector2&, bool> m_onResize;
};

/** @brief Build an NcWindow module instance. */
auto BuildWindowModule(const config::ProjectSettings& projectSettings,
                       const config::GraphicsSettings& graphicsSettings,
                       Signal<>& quit) -> std::unique_ptr<NcWindow>;
} // namespace window
} // namespace nc

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

/** @brief Allow an object to receive window resize events. Receivers must be unregistered before they are destroyed. */
void RegisterOnResizeReceiver(IOnResizeReceiver* receiver);

/** @brief Unregister an object from receiving window resize events. */
void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept;

/** @brief */
class NcWindow : public Module
{
    public:
        explicit NcWindow() noexcept
            : Module{NcWindowId} {}

        virtual auto GetDimensions() const noexcept -> const Vector2& = 0;

        virtual auto GetScreenExtent() const noexcept -> const Vector2& = 0;

        virtual auto GetContentScale() const noexcept -> const Vector2& = 0;

        virtual auto GetWindowHandle() const noexcept -> GLFWwindow* = 0;

        virtual auto OnResize() noexcept -> Signal<float, float, bool>& = 0;
        // virtual void BindGraphicsOnResizeCallback(std::function<void(float,float,bool)> callback) noexcept = 0;


        virtual void ProcessSystemMessages() = 0;
};

/** @brief Build an NcWindow module instance. */
auto BuildWindowModule(const config::ProjectSettings& projectSettings,
                       const config::GraphicsSettings& graphicsSettings,
                       Signal<>& quit) -> std::unique_ptr<NcWindow>;
} // namespace window
} // namespace nc

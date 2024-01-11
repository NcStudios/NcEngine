/**
 * @file Window.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/window/IOnResizeReceiver.h"

#include <array>

namespace nc::window
{
    /**
     * @brief Get the window dimensions
     */
    [[nodiscard]] Vector2 GetDimensions();

    /**
     * @brief Get the dimensions of the screen after aspect ratio transformations.
     */
    [[nodiscard]] Vector2 GetScreenExtent();

    /**
     * @brief Allow an object to receive window resize events. Receivers must be unregistered before they are destroyed.
     */
    void RegisterOnResizeReceiver(IOnResizeReceiver* receiver);

    /**
     * @brief Unregister an object from receiving window resize events.
    */
    void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept;
} //namespace nc::window

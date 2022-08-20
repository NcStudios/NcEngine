#pragma once

#include "window/IOnResizeReceiver.h"

#include <array>

namespace nc::window
{
    /** Get current window dimensions */
    [[nodiscard]] Vector2 GetDimensions();

    /** Set the color the frame clears to */
    void SetClearColor(std::array<float, 4> color);

    /** Allow an object to receive window resize events. Receivers must
     *  be unregistered before they are destroyed. */
    void RegisterOnResizeReceiver(IOnResizeReceiver* receiver);
    void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept;
}
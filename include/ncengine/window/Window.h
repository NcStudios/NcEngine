#pragma once

#include "ncengine/window/IOnResizeReceiver.h"

#include <array>

namespace nc::window
{
    /** Get current window dimensions */
    [[nodiscard]] Vector2 GetDimensions();
    [[nodiscard]] Vector2 GetScreenDimensions();

    /** Allow an object to receive window resize events. Receivers must
     *  be unregistered before they are destroyed. */
    void RegisterOnResizeReceiver(IOnResizeReceiver* receiver);
    void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver) noexcept;
}
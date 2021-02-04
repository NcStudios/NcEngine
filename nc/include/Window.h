#pragma once

#include "window/IOnResizeReceiver.h"

namespace nc::window
{
    /** Get current window dimensions */
    [[nodiscard]] Vector2 GetDimensions();

    /** Allow an object to receive window resize events. Receivers must
     *  be unregistered before they are destroyed. */
    void RegisterOnResizeReceiver(IOnResizeReceiver* receiver);
    void UnregisterOnResizeReceiver(IOnResizeReceiver* receiver);
}
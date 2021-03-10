#pragma once

#include "physics/IClickable.h"

namespace nc::physics
{
    /** Raycast from the main camera to mouse position against IClickables in the
     *  layers specified by mask. Returns the first hit or nullptr on failure. */
    [[nodiscard]] IClickable* RaycastToClickables(LayerMask mask = LayerMaskAll);

    /** Include objects in RaycastToClickables checks. Objects must be unregistered
     *  before they are destroyed. */
    void RegisterClickable(IClickable* clickable);
    void UnregisterClickable(IClickable* clickable) noexcept;
}
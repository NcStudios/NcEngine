#pragma once

#include "physics/LayerMask.h"
#include "physics/IClickable.h"

namespace nc::physics
{
    void NcRegisterClickable(IClickable* clickable);
    void NcUnregisterClickable(IClickable* clickable);
    void NcRaycastToClickables(LayerMask mask = LAYER_MASK_ALL);
}
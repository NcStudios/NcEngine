#pragma once

#include "physics/LayerMask.h"
#include "physics/IClickable.h"

namespace nc::physics
{
    void NcRegisterClickable(IClickable* clickable);
    void NcUnregisterClickable(IClickable* clickable);
    void NcRaycastToIClickables(LayerMask mask = LAYER_MASK_ALL);
}
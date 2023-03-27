#pragma once

#include "DirectXMath.h"

namespace nc::graphics
{
/** @todo We probably don't want the viewProjection in this struct. We only need
 *  1 copy, not 1 per renderer. */
struct ObjectData
{
    DirectX::XMMATRIX model;
    DirectX::XMMATRIX modelView;
    DirectX::XMMATRIX viewProjection;

    uint32_t colorSlotAIndex; // Todo: Make this more generic for materials
    uint32_t colorSlotBIndex; // Todo: Make this more generic for materials
    uint32_t colorSlotCIndex; // Todo: Make this more generic for materials
    uint32_t colorSlotDIndex; // Todo: Make this more generic for materials
};
} // namespace nc::graphics

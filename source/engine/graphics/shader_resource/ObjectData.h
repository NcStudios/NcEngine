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

    uint32_t materialParameterA; // Todo: Make this more generic for materials
    uint32_t materialParameterB; // Todo: Make this more generic for materials
    uint32_t materialParameterC; // Todo: Make this more generic for materials
    uint32_t materialParameterD; // Todo: Make this more generic for materials

    // uint32_t skeletalAnimationIndex; /* @todo : Truncation point for this PR. */
};
} // namespace nc::graphics

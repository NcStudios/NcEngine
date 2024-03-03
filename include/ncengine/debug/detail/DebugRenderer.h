#pragma once

#ifndef NC_DEBUG_RENDERING_INCLUDE_TOKEN
#error __FILE__ should not be directly included. Use DebugRendering.h instead.
#endif

#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/WireframeRenderer.h"

#include <array>

namespace nc::debug
{
void DebugRendererInitialize(ecs::BasicEcs<graphics::WireframeRenderer> gameState);
void DebugRendererNewFrame();
void DebugRendererAddWireframe(WireframeType type, DirectX::FXMMATRIX matrix);
} // namespace nc::debug

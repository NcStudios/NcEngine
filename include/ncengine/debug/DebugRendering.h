#pragma once

#define NC_DEBUG_RENDERING_INCLUDE_TOKEN

namespace nc::debug
{
enum class WireframeType
{
    Cube,
    Plane,
    Sphere,
    Count
};
} // namespace nc::debug

#ifdef NC_DEBUG_RENDERING_ENABLED
#include "ncengine/debug/detail/DebugRenderer.h"
#define NC_DEBUG_DRAW_WIREFRAME(type, matrix) nc::debug::DebugRendererAddWireframe(type, matrix);
#else
#define NC_DEBUG_DRAW_WIREFRAME(type, matrix);
#endif

#undef NC_DEBUG_RENDERING_INCLUDE_TOKEN

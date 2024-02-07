/**
 * @file WireframeRenderer.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/asset/Assets.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/ecs/Entity.h"

namespace nc::graphics
{
/** @brief Specifies the source a WireframeRenderer should pull from. */
enum class WireframeSource
{
    Renderer,
    Collider
};

/**
 * @brief Component for rendering wireframe models.
 * 
 * When 'source' is set to WireframeSource::Renderer, the underlying mesh of the MeshRenderer or ToonRenderer attached
 * to 'target' will be used to render a wireframe. If 'target' does not contain either of these, the default cube mesh
 * will be used.
 * 
 * When 'source' is set to WireframeSource::Collider, the underlying geometry of the Collider attached to 'target' will
 * be used to render a wireframe. If 'target' does not contain a Collider, 'target' will be set to Entity::Null() during
 * the next pass of ExecutionPhase::Render.
 * 
 * The WireframeRenderer instance's parent Entity or another Entity may be specified for 'target'. If 'target' no longer
 * refers to a valid Entity, it will be set to Entity::Null() during the next pass of ExecutionPhase::Render.
 */
struct WireframeRenderer
{
    Entity target = Entity::Null();
    WireframeSource source = WireframeSource::Renderer;
};
} // namespace nc::graphics

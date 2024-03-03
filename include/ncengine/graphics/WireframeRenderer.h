/**
 * @file WireframeRenderer.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/asset/Assets.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/ecs/Entity.h"

#include <vector>

namespace nc::graphics
{
/** @brief Specifies the source a WireframeRenderer should pull from. */
enum class WireframeSource
{
    Internal,
    Renderer,
    Collider
};

/**
 * @brief Component for rendering wireframe models.
 * 
 * When 'source' is set to WireframeSource::Internal, the members 'mesh' and 'instances' will be used to render N
 * wireframes, where N is the size of 'instances'. The 'target' member is unused.
 * 
 * When 'source' is set to WireframeSource::Renderer, the underlying mesh of the MeshRenderer or ToonRenderer attached
 * to 'target' will be used to render a wireframe. If 'target' does not contain either of these, the default cube mesh
 * will be used. The rendered position is based on 'target'.
 * 
 * When 'source' is set to WireframeSource::Collider, the underlying geometry of the Collider attached to 'target' will
 * be used to render a wireframe. If 'target' does not contain a Collider, 'target' will be set to Entity::Null() during
 * the next pass of ExecutionPhase::Render. The rendered position is based on 'target'.
 * 
 * In either 'Renderer' or 'Collider' source modes, 'target' may be set to the WireframeRenderer's parent Entity or
 * another Entity. If 'target' no longer refers to a valid Entity, it will be set to Entity::Null() during the next pass
 * of ExecutionPhase::Render.
 */
struct WireframeRenderer
{
    explicit WireframeRenderer(WireframeSource source_, Entity target_) noexcept
        : source{source_}, target{target_}, mesh{}, instances{}
    {
    }

    explicit WireframeRenderer(MeshView mesh_, std::vector<DirectX::XMMATRIX> instances_) noexcept
        : source{WireframeSource::Internal}, target{}, mesh{mesh_}, instances{std::move(instances_)}
    {
    }

    WireframeSource source;
    Entity target;
    MeshView mesh;
    std::vector<DirectX::XMMATRIX> instances;
};
} // namespace nc::graphics

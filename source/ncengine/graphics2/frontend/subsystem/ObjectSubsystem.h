#pragma once

#include "ObjectRenderState.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/utility/Signal.h"

#include <memory>
#include <vector>

namespace nc::graphics
{
/** Alias over synchronized vector of XMMATRIX and vector of entities. 
 * Can't use a span here because we construct a temporary sorted vector of entities in the case of the static sort. */
using ModelMatricesAndEntities = std::pair<ObjectRenderState, std::vector<Entity>>; 

/*
Produces a vector of transform matrices for dynamic (movable) MeshRenderers and static MeshRenderers.
Static MeshRenderers are at the beginning because we won't need to rebuild that section each frame unless dirty.
Split into two CTORs to provide ability to benchmark if this is worth the CPU RAM hit for caching and CPU time for initial sort/maintenance.
*/
class ObjectSubsystem
{
    public:
        /** This overload does not sort static mesh renderers to the beginning of the vector. */
        explicit ObjectSubsystem();

         /** This overload sorts static mesh renderers to the beginning of the vector. */
        explicit ObjectSubsystem(nc::Signal<nc::graphics::ToonRenderer&>& onAddToonRenderer,
                                 nc::Signal<nc::Entity>& onRemoveToonRenderer);

        /** @todo: 776 Add MeshRenderer components in place of old ToonRenderer component. */
        auto BuildState(ecs::ExplicitEcs<ToonRenderer, Transform> ecs) -> ModelMatricesAndEntities;

    private:
        /** @todo 776 Replace with new MeshRenderer type once we implement 776 */
        void OnAddStaticRenderer(ToonRenderer& renderer) { m_isStaticRenderersDirty = renderer.ParentEntity().IsStatic(); }
        void OnRemoveStaticRenderer(Entity entity) { m_isStaticRenderersDirty = entity.IsStatic(); }

        std::unique_ptr<Connection> m_onAddStaticRenderer;
        std::unique_ptr<Connection> m_onRemoveStaticRenderer;
        ObjectRenderState m_staticRendererStateCache;
        std::vector<Entity> m_staticEntityCache;
        uint32_t m_dynamicRenderersStartIndex;
        bool m_isStaticRenderersDirty;
        bool m_sortByStatic;
};
} // namespace nc::graphics

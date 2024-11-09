#pragma once

#include "MeshRendererRenderState.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/utility/Signal.h"

#include <memory>
#include <vector>

namespace nc
{
struct SystemEvents;
namespace config
{
struct GraphicsSettings;
}
namespace graphics
{
/*
Produces a vector of transform matrices for dynamic (movable) MeshRenderers and static MeshRenderers.
Static MeshRenderers are at the beginning because we won't need to rebuild that section each frame unless dirty.
Split into two CTORs to provide ability to benchmark if this is worth the CPU RAM hit for caching and CPU time for initial sort/maintenance.
*/
class MeshRendererSubsystem
{
    public:
        /** This overload does not sort static mesh renderers to the beginning of the vector. */
        explicit MeshRendererSubsystem();

         /** This overload sorts static mesh renderers to the beginning of the vector. */
        explicit MeshRendererSubsystem(Signal<ToonRenderer&>& onAddToonRenderer,
                                       Signal<Entity>& onRemoveToonRenderer,
                                       SystemEvents& events);

        /** @todo: 776 Add MeshRenderer components in place of old ToonRenderer component. */
        auto BuildState(ecs::ExplicitEcs<ToonRenderer, Transform> ecs) -> MeshRendererRenderState;

    private:
        /** @todo 776 Replace with new MeshRenderer type once we implement 776 */
        void OnAddRenderer(ToonRenderer& renderer) { m_isStaticRenderersDirty = renderer.ParentEntity().IsStatic(); }
        void OnRemoveRenderer(Entity entity) { m_isStaticRenderersDirty = entity.IsStatic(); }
        void OnStaticEntitiesRebuilt() { m_isStaticRenderersDirty = true; }

        std::unique_ptr<Connection> m_onAddRenderer;
        std::unique_ptr<Connection> m_onRemoveRenderer;
        std::unique_ptr<Connection> m_onStaticEntitiesRebuilt;

        std::vector<DirectX::XMMATRIX> m_staticMatricesCache;
        std::vector<Entity> m_staticEntityCache;

        std::vector<DirectX::XMMATRIX> m_dynamicMatricesCache;
        std::vector<Entity> m_dynamicEntityCache;

        bool m_isStaticRenderersDirty;
        bool m_sortByStatic;
};
} // namespace graphics
} // namespace nc

#pragma once

#include "MaterialPassCache.h"
#include "MeshRendererCache.h"
#include "MeshRendererRenderState.h"
#include "graphics2/MeshRendererContext.h"

#include "ncengine/ecs/EcsFwd.h"

#include <vector>

namespace nc
{
class MeshRenderer2;
class Transform;

namespace graphics
{
/*
Produces a vector of transform matrices for MeshRenderers and their corresponding Entities.
*/
class MeshRendererSubsystem
{
    public:
        explicit MeshRendererSubsystem(std::span<const MaterialPass::type> passes);

        auto BuildState(ecs::ExplicitEcs<MeshRenderer2, Transform> ecs) -> MeshRendererRenderState;

        void AddInstance(Entity entity, const MaterialDesc& materialDesc);
        void RemoveInstance(Entity entity, MaterialInstanceHandle materialInstance);

        void SetInstanceMesh(Entity entity, MaterialPasses passes, const asset::MeshView& mesh);
        void SetInstancePasses(Entity entity, MaterialPasses oldPasses, MaterialPasses newPasses);
        // void SetInstanceMaterialProperties(Entity entity)
        // ...

    private:
        // std::vector<MeshRendererData> m_instanceData;
        InstanceCache m_instanceCache;
        MaterialPassCache m_passCache;
        MeshRendererContext m_ctx;
};

/** slick layout would be (vector might have to be a 'cache' type in some places):
 * 
 * // in sync
 * vector<Entity>           ids;
 * vector<MeshRendererData> instanceData;
 * vector<MaterialPasses>   passData;
 * 
 * vector<MeshView> meshCache;
 * 
 * 
 * 
 * 
 */

} // namespace graphics
} // namespace nc

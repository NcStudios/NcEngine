#pragma once

#include "MeshRendererRenderState.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/ToonRenderer.h"

#include <memory>
#include <vector>

namespace nc::graphics
{
/*
Produces a vector of transform matrices for MeshRenderers and their corresponding Entities.
*/
class MeshRendererSubsystem
{
    public:
        /** @todo: 776 Add MeshRenderer component(s) in place of old ToonRenderer component. */
        auto BuildState(ecs::ExplicitEcs<ToonRenderer, Transform> ecs) -> MeshRendererRenderState;

    private:
        std::vector<MeshRendererData> m_modelMatricesCache;

};
} // namespace nc::graphics

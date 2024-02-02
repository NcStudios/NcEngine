#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/physics/Collider.h"

#include <vector>

namespace nc::graphics
{
struct WidgetState
{
    std::vector<std::pair<DirectX::XMMATRIX, MeshView>> wireframeData;
};

class WidgetSystem
{
    public:
        auto Execute(ecs::ExplicitEcs<Transform,
                                      MeshRenderer,
                                      ToonRenderer,
                                      WireframeRenderer,
                                      physics::Collider> worldView) -> WidgetState;
};
} // namespace nc::graphics

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
struct WireframeRenderState
{
    DirectX::XMMATRIX matrix;
    MeshView mesh;
    Vector4 color;
};

struct WidgetState
{
    std::vector<WireframeRenderState> wireframeData;
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

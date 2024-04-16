#pragma once

#include "ncengine/ecs/EcsFwd.h"
#include "ncengine/asset/AssetViews.h"

#include "DirectXMath.h"

#include <vector>

namespace nc
{
class Transform;

namespace physics
{
class Collider;
} // namespace physics

namespace graphics
{
class MeshRenderer;
class ToonRenderer;
struct WireframeRenderer;

struct WireframeRenderState
{
    DirectX::XMMATRIX matrix;
    asset::MeshView mesh;
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
} // namespace graphics
} // namespace nc

#include "WidgetSystem.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/utility/MatrixUtilities.h"
#include "asset/AssetService.h"

#include "optick.h"

namespace
{
[[maybe_unused]]
auto GetMeshView(nc::Entity target, nc::ecs::ExplicitEcs<nc::graphics::MeshRenderer, nc::graphics::ToonRenderer> worldView) -> nc::asset::MeshView
{
    static const auto defaultMeshView = nc::asset::AssetService<nc::asset::MeshView>::Get()->Acquire(nc::asset::CubeMesh);

    if (worldView.Contains<nc::graphics::MeshRenderer>(target))
        return worldView.Get<nc::graphics::MeshRenderer>(target).GetMeshView();
    else if (worldView.Contains<nc::graphics::ToonRenderer>(target))
        return worldView.Get<nc::graphics::ToonRenderer>(target).GetMeshView();
    else
        return defaultMeshView;
}

[[maybe_unused]]
auto GetMeshView(nc::physics::ColliderType type) -> nc::asset::MeshView
{
    using namespace nc::asset;
    switch(type)
    {
        case nc::physics::ColliderType::Box:
        {
            static const auto view = AssetService<MeshView>::Get()->Acquire(CubeMesh);
            return view;
        }
        case nc::physics::ColliderType::Sphere:
        {
            static const auto view = AssetService<MeshView>::Get()->Acquire(SphereMesh);
            return view;
        }
        case nc::physics::ColliderType::Capsule:
        {
            static const auto view = AssetService<MeshView>::Get()->Acquire(CapsuleMesh);
            return view;
        }
        case nc::physics::ColliderType::Hull:
        {
            // @todo #567: Eventually not have sphere here.
            static const auto view = AssetService<MeshView>::Get()->Acquire(SphereMesh);
            return view;
        }
        default:
        {
            throw nc::NcError("Unknown ColliderType");
        }
    }
}

[[maybe_unused]]
auto GetMeshView(nc::physics::ShapeType shape) -> nc::asset::MeshView
{
    using namespace nc::asset;
    switch(shape)
    {
        case nc::physics::ShapeType::Box:
        {
            static const auto view = AssetService<MeshView>::Get()->Acquire(CubeMesh);
            return view;
        }
        case nc::physics::ShapeType::Sphere:
        {
            static const auto view = AssetService<MeshView>::Get()->Acquire(SphereMesh);
            return view;
        }
        case nc::physics::ShapeType::Capsule:
        {
            static const auto view = AssetService<MeshView>::Get()->Acquire(CapsuleMesh);
            return view;
        }
        default:
        {
            throw nc::NcError("Unknown Shape");
        }
    }
}

[[maybe_unused]]
auto CalculateWireframeMatrix(DirectX::FXMMATRIX worldSpace, const nc::physics::VolumeInfo& info) -> DirectX::XMMATRIX
{
    const auto scale = DirectX::XMLoadVector3(&info.scale);
    const auto offset = DirectX::XMLoadVector3(&info.offset);
    return DirectX::XMMatrixScalingFromVector(scale) * worldSpace * DirectX::XMMatrixTranslationFromVector(offset);
}

[[maybe_unused]]
auto CalculateWireframeMatrix(DirectX::FXMMATRIX worldSpace, const nc::physics::Shape& shape, bool scalesWithTransform) -> DirectX::XMMATRIX
{
    const auto localSpace = nc::ToScaleMatrix(shape.GetLocalScale()) * nc::ToTransMatrix(shape.GetLocalPosition());
    if (scalesWithTransform)
    {
        return localSpace * worldSpace;
    }

    const auto [_, worldRotation, worldPosition] = nc::DecomposeMatrix(worldSpace);
    return localSpace * DirectX::XMMatrixRotationQuaternion(worldRotation) * DirectX::XMMatrixTranslationFromVector(worldPosition);
}
} // anonymous namespace

namespace nc::graphics
{
auto WidgetSystem::Execute(ecs::ExplicitEcs<Transform,
                                            MeshRenderer,
                                            ToonRenderer,
                                            WireframeRenderer,
                                            physics::Collider,
                                            physics::RigidBody> worldView) -> WidgetState
{
    OPTICK_CATEGORY("WidgetSystem::Execute", Optick::Category::Rendering);
    auto state = WidgetState{};

#ifdef NC_EDITOR_ENABLED
    for (auto& renderer : worldView.GetAll<WireframeRenderer>())
    {
        if (renderer.source == WireframeSource::Internal)
        {
            for (const auto& matrix : renderer.instances)
            {
                state.wireframeData.emplace_back(matrix, renderer.mesh, renderer.color);
            }

            continue;
        }

        if (!renderer.target.Valid())
            continue;

        if (!worldView.Contains<Transform>(renderer.target))
        {
            renderer.target = Entity::Null();
            continue;
        }

        const auto& targetMatrix = worldView.Get<Transform>(renderer.target).TransformationMatrix();
        switch (renderer.source)
        {
            case WireframeSource::Renderer:
            {
                state.wireframeData.emplace_back(targetMatrix, GetMeshView(renderer.target, worldView), renderer.color);
                break;
            }
            case WireframeSource::Collider:
            {
                if (!worldView.Contains<physics::RigidBody>(renderer.target))
                {
                    renderer.target = Entity::Null();
                    continue;
                }

                const auto& body = worldView.Get<physics::RigidBody>(renderer.target);
                const auto& shape = body.GetShape();
                state.wireframeData.emplace_back(CalculateWireframeMatrix(targetMatrix, shape, body.ScalesWithTransform()), GetMeshView(shape.GetType()), renderer.color);
                break;
            }
            case WireframeSource::Internal: std::unreachable();
        }
    }
#else
    (void)worldView;
#endif

    return state;
}
} // namespace nc::graphics

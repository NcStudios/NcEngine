#include "WireframeRendererSubsystem.h"

#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/Mesh.h"
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/physics/RigidBody.h"
#include "asset/AssetService.h"

#include "ncmath/MatrixUtilities.h"
#include "optick.h"

namespace
{
auto GetMeshView(nc::Entity target, nc::ecs::ExplicitEcs<nc::StaticMesh> worldView) -> nc::asset::MeshView
{
    static const auto meshService = nc::asset::AssetService<nc::asset::MeshView>::Get();
    static const auto defaultMeshView = meshService->Acquire(nc::asset::CubeMesh);

    if (worldView.Contains<nc::StaticMesh>(target))
    {
        const auto id = worldView.Get<nc::StaticMesh>(target).GetMeshId();
        const auto path = std::string{meshService->GetPath(id)};
        return meshService->Acquire(path);
    }
    else
    {
        return defaultMeshView;
    }
}

auto GetMeshView(nc::ShapeType shape) -> nc::asset::MeshView
{
    using namespace nc::asset;
    switch(shape)
    {
        case nc::ShapeType::Box:
        {
            static const auto view = AssetService<MeshView>::Get()->Acquire(CubeMesh);
            return view;
        }
        case nc::ShapeType::Sphere:
        {
            static const auto view = AssetService<MeshView>::Get()->Acquire(SphereMesh);
            return view;
        }
        case nc::ShapeType::Capsule:
        {
            static const auto view = AssetService<MeshView>::Get()->Acquire(CapsuleMesh);
            return view;
        }
        case nc::ShapeType::ConvexHull:
        {
            /** @todo 567 currently have no renderable geometry for these */
            static const auto view = AssetService<MeshView>::Get()->Acquire(CubeMesh);
            return view;
        }
        default:
        {
            throw nc::NcError("Unknown Shape");
        }
    }
}

auto CalculateWireframeMatrix(DirectX::FXMMATRIX worldSpace,
                              const nc::Shape& shape,
                              bool scalesWithTransform) -> DirectX::XMMATRIX
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
auto WireframeRendererSubsystem::BuildState(ecs::ExplicitEcs<Transform,
                                                             WireframeRenderer,
                                                             StaticMesh,
                                                             RigidBody> worldView) -> WireframeRendererRenderState
{
    OPTICK_CATEGORY("WireframeRendererSubsystem::Execute", Optick::Category::Rendering);
    auto state = WireframeRendererRenderState{};

    for (auto& renderer : worldView.GetAll<WireframeRenderer>())
    {
        if (renderer.source == WireframeSource::Internal)
        {
            for (const auto& matrix : renderer.instances)
            {
                state.wireframeData.emplace_back(
                    WireframeData{matrix, renderer.color},
                    renderer.mesh
                );
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
                state.wireframeData.emplace_back(
                    WireframeData{targetMatrix, renderer.color},
                    GetMeshView(renderer.target, worldView)
                );

                break;
            }
            case WireframeSource::Collider:
            {
                if (!worldView.Contains<RigidBody>(renderer.target))
                {
                    renderer.target = Entity::Null();
                    continue;
                }

                const auto& body = worldView.Get<RigidBody>(renderer.target);
                const auto& shape = body.GetShape();
                state.wireframeData.emplace_back(
                    WireframeData{
                        CalculateWireframeMatrix(
                            targetMatrix,
                            shape,
                            body.ScalesWithTransform()
                        ),
                        renderer.color
                    },
                    GetMeshView(shape.GetType())
                );

                break;
            }
            case WireframeSource::Internal: std::unreachable();
        }
    }

    return state;
}
} // namespace nc::graphics

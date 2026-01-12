#include "WireframeRendererSubsystem.h"

#include "ncasset/DefaultAssets.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/graphics/Light.h"
#include "ncengine/graphics/Mesh.h"
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/physics/RigidBody.h"
#include "asset/AssetService.h"

#include "ncmath/MatrixUtilities.h"
#include "optick.h"

#include <array>
#include <cmath>
#include <numbers>

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
        case nc::ShapeType::Mesh:
        case nc::ShapeType::Compound:
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
    const auto localSpace = nc::ToScaleMatrix(shape.GetLocalScale());
    if (scalesWithTransform)
    {
        return localSpace * worldSpace;
    }

    const auto [_, worldRotation, worldPosition] = nc::DecomposeMatrix(worldSpace);
    return localSpace * DirectX::XMMatrixRotationQuaternion(worldRotation) * DirectX::XMMatrixTranslationFromVector(worldPosition);
}

// Create a transformation matrix for a line segment between two points using a thin cube
auto CreateLineMatrix(const nc::Vector3& start, const nc::Vector3& end, float thickness = 0.02f) -> DirectX::XMMATRIX
{
    using namespace DirectX;

    const auto startVec = XMLoadVector3(&start);
    const auto endVec = XMLoadVector3(&end);

    // Midpoint for translation
    const auto midpoint = XMVectorScale(XMVectorAdd(startVec, endVec), 0.5f);

    // Direction and length
    const auto direction = XMVectorSubtract(endVec, startVec);
    const float length = XMVectorGetX(XMVector3Length(direction));

    // Scale: thin in X and Y, stretched along Z (cube's default is unit cube)
    const auto scale = XMMatrixScaling(thickness, thickness, length);

    // Rotation to align Z-axis with direction
    const auto normalizedDir = XMVector3Normalize(direction);
    const auto defaultDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    XMMATRIX rotation = XMMatrixIdentity();
    const float dot = XMVectorGetX(XMVector3Dot(defaultDir, normalizedDir));

    if (dot < -0.9999f)
    {
        // Opposite direction - rotate 180 degrees around any perpendicular axis
        rotation = XMMatrixRotationY(XM_PI);
    }
    else if (dot < 0.9999f)
    {
        // General case - use axis-angle rotation
        const auto axis = XMVector3Normalize(XMVector3Cross(defaultDir, normalizedDir));
        const float angle = std::acos(std::clamp(dot, -1.0f, 1.0f));
        rotation = XMMatrixRotationAxis(axis, angle);
    }

    const auto translation = XMMatrixTranslationFromVector(midpoint);

    return scale * rotation * translation;
}

// Generate 12 edge matrices for a frustum
auto GenerateFrustumEdgeMatrices(const nc::FrustumCorners& corners, float thickness = 0.02f) -> std::array<DirectX::XMMATRIX, 12>
{
    std::array<DirectX::XMMATRIX, 12> edges;

    // Near plane edges (4)
    edges[0] = CreateLineMatrix(corners.nearTopLeft, corners.nearTopRight, thickness);
    edges[1] = CreateLineMatrix(corners.nearTopRight, corners.nearBottomRight, thickness);
    edges[2] = CreateLineMatrix(corners.nearBottomRight, corners.nearBottomLeft, thickness);
    edges[3] = CreateLineMatrix(corners.nearBottomLeft, corners.nearTopLeft, thickness);

    // Far plane edges (4)
    edges[4] = CreateLineMatrix(corners.farTopLeft, corners.farTopRight, thickness);
    edges[5] = CreateLineMatrix(corners.farTopRight, corners.farBottomRight, thickness);
    edges[6] = CreateLineMatrix(corners.farBottomRight, corners.farBottomLeft, thickness);
    edges[7] = CreateLineMatrix(corners.farBottomLeft, corners.farTopLeft, thickness);

    // Connecting edges (4)
    edges[8] = CreateLineMatrix(corners.nearTopLeft, corners.farTopLeft, thickness);
    edges[9] = CreateLineMatrix(corners.nearTopRight, corners.farTopRight, thickness);
    edges[10] = CreateLineMatrix(corners.nearBottomLeft, corners.farBottomLeft, thickness);
    edges[11] = CreateLineMatrix(corners.nearBottomRight, corners.farBottomRight, thickness);

    return edges;
}

// Generate spotlight cone frustum (8 edges: 4 from apex to far corners + 4 far plane edges)
auto GenerateSpotLightEdgeMatrices(const nc::Transform& transform, float outerAngle, float radius, float thickness = 0.02f) -> std::array<DirectX::XMMATRIX, 8>
{
    using namespace DirectX;

    const auto position = transform.Position();
    const auto forward = transform.Forward();
    const auto right = transform.Right();
    const auto up = transform.Up();

    // Calculate far plane dimensions based on outer angle and radius
    const float farHalfWidth = radius * std::tan(outerAngle);
    const float farHalfHeight = farHalfWidth;

    // Far plane center
    const auto farCenter = nc::Vector3(
        position.x + forward.x * radius,
        position.y + forward.y * radius,
        position.z + forward.z * radius
    );

    // Far plane corners
    const auto farTopLeft = nc::Vector3(
        farCenter.x - right.x * farHalfWidth + up.x * farHalfHeight,
        farCenter.y - right.y * farHalfWidth + up.y * farHalfHeight,
        farCenter.z - right.z * farHalfWidth + up.z * farHalfHeight
    );
    const auto farTopRight = nc::Vector3(
        farCenter.x + right.x * farHalfWidth + up.x * farHalfHeight,
        farCenter.y + right.y * farHalfWidth + up.y * farHalfHeight,
        farCenter.z + right.z * farHalfWidth + up.z * farHalfHeight
    );
    const auto farBottomLeft = nc::Vector3(
        farCenter.x - right.x * farHalfWidth - up.x * farHalfHeight,
        farCenter.y - right.y * farHalfWidth - up.y * farHalfHeight,
        farCenter.z - right.z * farHalfWidth - up.z * farHalfHeight
    );
    const auto farBottomRight = nc::Vector3(
        farCenter.x + right.x * farHalfWidth - up.x * farHalfHeight,
        farCenter.y + right.y * farHalfWidth - up.y * farHalfHeight,
        farCenter.z + right.z * farHalfWidth - up.z * farHalfHeight
    );

    std::array<DirectX::XMMATRIX, 8> edges;

    // Lines from apex to far corners (4)
    edges[0] = CreateLineMatrix(position, farTopLeft, thickness);
    edges[1] = CreateLineMatrix(position, farTopRight, thickness);
    edges[2] = CreateLineMatrix(position, farBottomLeft, thickness);
    edges[3] = CreateLineMatrix(position, farBottomRight, thickness);

    // Far plane edges (4)
    edges[4] = CreateLineMatrix(farTopLeft, farTopRight, thickness);
    edges[5] = CreateLineMatrix(farTopRight, farBottomRight, thickness);
    edges[6] = CreateLineMatrix(farBottomRight, farBottomLeft, thickness);
    edges[7] = CreateLineMatrix(farBottomLeft, farTopLeft, thickness);

    return edges;
}

// Generate point light sphere wireframe (3 circles in XY, XZ, YZ planes)
constexpr uint32_t g_circleSegments = 24;
auto GeneratePointLightEdgeMatrices(const nc::Vector3& position, float radius, float thickness = 0.02f) -> std::array<DirectX::XMMATRIX, g_circleSegments * 3>
{
    std::array<DirectX::XMMATRIX, g_circleSegments * 3> edges;
    constexpr float angleStep = 2.0f * std::numbers::pi_v<float> / g_circleSegments;

    // XY plane circle
    for (uint32_t i = 0; i < g_circleSegments; ++i)
    {
        const float angle1 = i * angleStep;
        const float angle2 = (i + 1) * angleStep;
        const auto p1 = nc::Vector3(position.x + radius * std::cos(angle1), position.y + radius * std::sin(angle1), position.z);
        const auto p2 = nc::Vector3(position.x + radius * std::cos(angle2), position.y + radius * std::sin(angle2), position.z);
        edges[i] = CreateLineMatrix(p1, p2, thickness);
    }

    // XZ plane circle
    for (uint32_t i = 0; i < g_circleSegments; ++i)
    {
        const float angle1 = i * angleStep;
        const float angle2 = (i + 1) * angleStep;
        const auto p1 = nc::Vector3(position.x + radius * std::cos(angle1), position.y, position.z + radius * std::sin(angle1));
        const auto p2 = nc::Vector3(position.x + radius * std::cos(angle2), position.y, position.z + radius * std::sin(angle2));
        edges[g_circleSegments + i] = CreateLineMatrix(p1, p2, thickness);
    }

    // YZ plane circle
    for (uint32_t i = 0; i < g_circleSegments; ++i)
    {
        const float angle1 = i * angleStep;
        const float angle2 = (i + 1) * angleStep;
        const auto p1 = nc::Vector3(position.x, position.y + radius * std::cos(angle1), position.z + radius * std::sin(angle1));
        const auto p2 = nc::Vector3(position.x, position.y + radius * std::cos(angle2), position.z + radius * std::sin(angle2));
        edges[g_circleSegments * 2 + i] = CreateLineMatrix(p1, p2, thickness);
    }

    return edges;
}

// Generate directional light frustum (simple box representing shadow map bounds)
auto GenerateDirectionalLightEdgeMatrices(const nc::Transform& transform, float extent, float thickness = 0.02f) -> std::array<DirectX::XMMATRIX, 12>
{
    const auto position = transform.Position();
    const auto forward = transform.Forward();
    const auto right = transform.Right();
    const auto up = transform.Up();

    const float halfExtent = extent * 0.5f;

    // Near and far centers along light direction
    const auto nearCenter = nc::Vector3(
        position.x - forward.x * halfExtent,
        position.y - forward.y * halfExtent,
        position.z - forward.z * halfExtent
    );
    const auto farCenter = nc::Vector3(
        position.x + forward.x * halfExtent,
        position.y + forward.y * halfExtent,
        position.z + forward.z * halfExtent
    );

    auto computeCorner = [&](const nc::Vector3& center, float rx, float uy) -> nc::Vector3 {
        return nc::Vector3(
            center.x + right.x * rx * halfExtent + up.x * uy * halfExtent,
            center.y + right.y * rx * halfExtent + up.y * uy * halfExtent,
            center.z + right.z * rx * halfExtent + up.z * uy * halfExtent
        );
    };

    nc::FrustumCorners corners;
    corners.nearTopLeft     = computeCorner(nearCenter, -1.0f,  1.0f);
    corners.nearTopRight    = computeCorner(nearCenter,  1.0f,  1.0f);
    corners.nearBottomLeft  = computeCorner(nearCenter, -1.0f, -1.0f);
    corners.nearBottomRight = computeCorner(nearCenter,  1.0f, -1.0f);
    corners.farTopLeft      = computeCorner(farCenter,  -1.0f,  1.0f);
    corners.farTopRight     = computeCorner(farCenter,   1.0f,  1.0f);
    corners.farBottomLeft   = computeCorner(farCenter,  -1.0f, -1.0f);
    corners.farBottomRight  = computeCorner(farCenter,   1.0f, -1.0f);

    return GenerateFrustumEdgeMatrices(corners, thickness);
}
} // anonymous namespace

namespace nc::graphics
{
auto WireframeRendererSubsystem::BuildState(ecs::ExplicitEcs<Transform,
                                                             WireframeRenderer,
                                                             StaticMesh,
                                                             RigidBody,
                                                             Camera,
                                                             DirectionalLight,
                                                             PointLight,
                                                             SpotLight> worldView) -> WireframeRendererRenderState
{
    OPTICK_CATEGORY("WireframeRendererSubsystem::Execute", Optick::Category::Rendering);
    auto state = WireframeRendererRenderState{};

    // Cache cube mesh for frustum edges
    static const auto cubeMeshView = asset::AssetService<asset::MeshView>::Get()->Acquire(asset::CubeMesh);

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

        const auto& transform = worldView.Get<Transform>(renderer.target);
        const auto& targetMatrix = transform.TransformationMatrix();
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
            case WireframeSource::Camera:
            {
                if (!worldView.Contains<Camera>(renderer.target))
                {
                    renderer.target = Entity::Null();
                    continue;
                }

                const auto& camera = worldView.Get<Camera>(renderer.target);
                const auto frustumCorners = camera.CalculateFrustumCorners();
                const auto edgeMatrices = GenerateFrustumEdgeMatrices(frustumCorners);

                for (const auto& matrix : edgeMatrices)
                {
                    state.wireframeData.emplace_back(
                        WireframeData{matrix, renderer.color},
                        cubeMeshView
                    );
                }

                break;
            }
            case WireframeSource::SpotLight:
            {
                if (!worldView.Contains<SpotLight>(renderer.target))
                {
                    renderer.target = Entity::Null();
                    continue;
                }

                const auto& light = worldView.Get<SpotLight>(renderer.target);
                // Compute outer angle to match the actual lit cone
                // This matches the calculation in LightSubsystem.cpp
                const float radiusFactor = std::max(1.0f - light.radius * 0.01f, 0.1f);
                const float outerAngleCos = std::cos(std::max(light.outerAngle, 0.0001f)) * radiusFactor;
                const float effectiveOuterAngle = std::acos(std::clamp(outerAngleCos, -1.0f, 1.0f));
                const auto edgeMatrices = GenerateSpotLightEdgeMatrices(transform, effectiveOuterAngle, light.radius);

                for (const auto& matrix : edgeMatrices)
                {
                    state.wireframeData.emplace_back(
                        WireframeData{matrix, renderer.color},
                        cubeMeshView
                    );
                }

                break;
            }
            case WireframeSource::PointLight:
            {
                if (!worldView.Contains<PointLight>(renderer.target))
                {
                    renderer.target = Entity::Null();
                    continue;
                }

                const auto& light = worldView.Get<PointLight>(renderer.target);
                const auto edgeMatrices = GeneratePointLightEdgeMatrices(transform.Position(), light.radius);

                for (const auto& matrix : edgeMatrices)
                {
                    state.wireframeData.emplace_back(
                        WireframeData{matrix, renderer.color},
                        cubeMeshView
                    );
                }

                break;
            }
            case WireframeSource::DirectionalLight:
            {
                if (!worldView.Contains<DirectionalLight>(renderer.target))
                {
                    renderer.target = Entity::Null();
                    continue;
                }

                // Use a default extent for directional light visualization
                constexpr float defaultExtent = 20.0f;
                const auto edgeMatrices = GenerateDirectionalLightEdgeMatrices(transform, defaultExtent);

                for (const auto& matrix : edgeMatrices)
                {
                    state.wireframeData.emplace_back(
                        WireframeData{matrix, renderer.color},
                        cubeMeshView
                    );
                }

                break;
            }
            case WireframeSource::Internal: std::unreachable();
        }
    }

    return state;
}
} // namespace nc::graphics

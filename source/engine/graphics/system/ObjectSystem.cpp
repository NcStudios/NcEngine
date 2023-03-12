#include "ObjectSystem.h"
#include "CameraSystem.h"
#include "EnvironmentSystem.h"
#include "graphics/Camera.h"
#include "graphics/shader_resource/ObjectData.h"
#include "physics/collision/IntersectionQueries.h"

namespace
{
bool IsViewedByFrustum(const nc::Frustum& frustum, const nc::graphics::MeshRenderer& renderer, DirectX::FXMMATRIX transform)
{
    const auto maxScaleExtent = nc::GetMaxScaleExtent(transform);
    const auto maxMeshExtent = renderer.GetMesh().maxExtent;
    auto sphere = nc::Sphere{ .center = nc::Vector3::Zero(), .radius = maxScaleExtent * maxMeshExtent };
    DirectX::XMStoreVector3(&sphere.center, transform.r[3]);
    return nc::physics::Intersect(frustum, sphere);
}
} // anonymous namespace

namespace nc::graphics
{
auto ObjectSystem::Execute(MultiView<MeshRenderer, Transform> gameState,
                           const CameraFrontendState& cameraState,
                           EnvironmentFrontendState& environmentState) -> ObjectFrontendState
{
    const auto viewProjection = cameraState.view * cameraState.projection;
    auto objectData = std::vector<ObjectData>{};
    objectData.reserve(gameState.size_upper_bound());
    auto frontendState = ObjectFrontendState{};
    frontendState.meshes.reserve(gameState.size_upper_bound());

    for (const auto& [renderer, transform] : gameState)
    {
        const auto& modelMatrix = transform->TransformationMatrix();
        if (!IsViewedByFrustum(cameraState.frustum, *renderer, modelMatrix))
        {
            continue;
        }

        const auto [base, normal, roughness, metallic] = renderer->GetTextureIndices();
        objectData.emplace_back(modelMatrix, modelMatrix * cameraState.view, viewProjection, base.index, normal.index, roughness.index, metallic.index);
        frontendState.meshes.push_back(renderer->GetMesh());
    }

    if (environmentState.useSkybox)
    {
        auto skyboxMatrix = DirectX::XMMatrixScaling(200.0f, 200.0f, 200.0f);
        skyboxMatrix.r[3] = DirectX::XMVectorAdd(DirectX::XMLoadVector3(&cameraState.position), DirectX::g_XMIdentityR3);
        objectData.emplace_back(skyboxMatrix, skyboxMatrix * cameraState.view, viewProjection, 0, 0, 0, 0);
        frontendState.skyboxInstanceIndex = static_cast<uint32_t>(objectData.size() - 1);
    }

    m_backendPort.Emit(objectData);
    return frontendState;
}
} // namespace nc::graphics

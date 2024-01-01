#include "ObjectSystem.h"
#include "CameraSystem.h"
#include "EnvironmentSystem.h"
#include "SkeletalAnimationSystem.h"
#include "graphics/Camera.h"
#include "graphics/shader_resource/ObjectData.h"
#include "physics/collision/IntersectionQueries.h"

#include "optick.h"

namespace
{
bool IsViewedByFrustum(const nc::Frustum& frustum, float maxMeshExtent, DirectX::FXMMATRIX transform)
{
    const auto maxScaleExtent = nc::GetMaxScaleExtent(transform);
    auto sphere = nc::Sphere{ .center = nc::Vector3::Zero(), .radius = maxScaleExtent * maxMeshExtent };
    DirectX::XMStoreVector3(&sphere.center, transform.r[3]);
    return nc::physics::Intersect(frustum, sphere);
}
} // anonymous namespace

namespace nc::graphics
{
auto ObjectSystem::Execute(MultiView<MeshRenderer, Transform> pbrRenderers,
                           MultiView<ToonRenderer, Transform> toonRenderers,
                           const CameraState& cameraState,
                           const EnvironmentState& environmentState,
                           const SkeletalAnimationSystemState&) -> ObjectState
{
    OPTICK_CATEGORY("ObjectSystem::Execute", Optick::Category::Rendering);
    const auto viewProjection = cameraState.view * cameraState.projection;
    auto objectData = std::vector<ObjectData>{};
    objectData.reserve(pbrRenderers.size_upper_bound() + toonRenderers.size_upper_bound());
    auto frontendState = ObjectState{};
    frontendState.pbrMeshes.reserve(pbrRenderers.size_upper_bound());
    frontendState.toonMeshes.reserve(toonRenderers.size_upper_bound());

    for (const auto& [renderer, transform] : pbrRenderers)
    {
        const auto& modelMatrix = transform->TransformationMatrix();
        if (!IsViewedByFrustum(cameraState.frustum, renderer->GetMeshView().maxExtent, modelMatrix))
        {
            continue;
        }

        const auto& [base, normal, roughness, metallic] = renderer->GetMaterialView();
        objectData.emplace_back(modelMatrix, modelMatrix * cameraState.view, viewProjection, base.index, normal.index, roughness.index, metallic.index);
        frontendState.pbrMeshes.push_back(renderer->GetMeshView());
    }

    frontendState.pbrMeshStartingIndex = 0u;

    for (const auto& [renderer, transform] : toonRenderers)
    {
        const auto& modelMatrix = transform->TransformationMatrix();
        if (!IsViewedByFrustum(cameraState.frustum, renderer->GetMeshView().maxExtent, modelMatrix))
        {
            continue;
        }

        const auto& [baseColor, overlay, hatching, hatchingTiling] = renderer->GetMaterialView();
        objectData.emplace_back(modelMatrix, modelMatrix * cameraState.view, viewProjection, baseColor.index, overlay.index, hatching.index, hatchingTiling);
        frontendState.toonMeshes.push_back(renderer->GetMeshView());
    }
    frontendState.toonMeshStartingIndex = static_cast<uint32_t>(frontendState.pbrMeshes.size());

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

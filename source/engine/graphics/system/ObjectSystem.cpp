#include "ObjectSystem.h"
#include "CameraSystem.h"
#include "EnvironmentSystem.h"
#include "SkeletalAnimationSystem.h"
#include "graphics/Camera.h"
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

template<typename T>
concept AnimatableComponent = std::same_as<T, nc::graphics::MeshRenderer> || std::same_as<T, nc::graphics::ToonRenderer>;

template <AnimatableComponent T>
uint32_t GetSkeletalAnimationIndex(const T* renderer, const nc::graphics::SkeletalAnimationSystemState& state)
{
    auto iter = state.animationIndices.find(renderer->ParentEntity().Index());
    return iter != state.animationIndices.end() ? iter->second : UINT32_MAX;
}
} // anonymous namespace

namespace nc::graphics
{
auto ObjectSystem::Execute(uint32_t frameIndex,
                           MultiView<MeshRenderer, Transform> pbrRenderers,
                           MultiView<ToonRenderer, Transform> toonRenderers,
                           const CameraState& cameraState,
                           const EnvironmentState& environmentState,
                           const SkeletalAnimationSystemState& skeletalAnimationState) -> ObjectState
{
    OPTICK_CATEGORY("ObjectSystem::Execute", Optick::Category::Rendering);
    auto frontendState = ObjectState{};
    const auto maxPbrRenderers = pbrRenderers.size_upper_bound();
    const auto maxToonRenderers = toonRenderers.size_upper_bound();
    frontendState.pbrMeshes.reserve(maxPbrRenderers);
    frontendState.toonMeshes.reserve(maxToonRenderers);
    m_objectData.clear();
    m_objectData.reserve(maxPbrRenderers + maxToonRenderers);

    for (const auto& [renderer, transform] : pbrRenderers)
    {
        const auto& modelMatrix = transform->TransformationMatrix();
        if (!IsViewedByFrustum(cameraState.frustum, renderer->GetMeshView().maxExtent, modelMatrix))
        {
            continue;
        }

        const auto skeletalAnimationIndex = GetSkeletalAnimationIndex(renderer, skeletalAnimationState);
        const auto& [base, normal, roughness, metallic] = renderer->GetMaterialView();
        m_objectData.emplace_back(modelMatrix, base.index, normal.index, roughness.index, metallic.index, skeletalAnimationIndex);
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

        const auto skeletalAnimationIndex = GetSkeletalAnimationIndex(renderer, skeletalAnimationState);
        const auto& [baseColor, overlay, hatching, hatchingTiling] = renderer->GetMaterialView();
        m_objectData.emplace_back(modelMatrix, baseColor.index, overlay.index, hatching.index, hatchingTiling, skeletalAnimationIndex);
        frontendState.toonMeshes.push_back(renderer->GetMeshView());
    }
    frontendState.toonMeshStartingIndex = static_cast<uint32_t>(frontendState.pbrMeshes.size());

    if (environmentState.useSkybox)
    {
        auto skyboxMatrix = DirectX::XMMatrixScaling(200.0f, 200.0f, 200.0f);
        skyboxMatrix.r[3] = DirectX::XMVectorAdd(DirectX::XMLoadVector3(&cameraState.position), DirectX::g_XMIdentityR3);
        m_objectData.emplace_back(skyboxMatrix, 0, 0, 0, 0);
        frontendState.skyboxInstanceIndex = static_cast<uint32_t>(m_objectData.size() - 1);
    }

    m_objectDataBuffer.Bind(m_objectData, frameIndex);
    return frontendState;
}
} // namespace nc::graphics

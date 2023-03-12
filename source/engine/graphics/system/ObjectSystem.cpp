#include "ObjectFrontend.h"
// #include "asset/Assets.h"
#include "ecs/Registry.h"
#include "ecs/View.h"
#include "graphics/Camera.h"
#include "graphics/MeshRenderer.h"
#include "graphics/shader_resource/ObjectData.h"
#include "physics/collision/IntersectionQueries.h"
#include "CameraState.h"
#include "Environment.h"

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
auto ObjectFrontend::Execute(Registry* registry,
                             const CameraFrontendState& cameraState,
                             EnvironmentFrontendState& environmentState) -> ObjectFrontendState
{
    // const auto viewMatrix = camera->GetViewMatrix();
    // const auto projectionMatrix = camera->GetProjectionMatrix();
    const auto viewProjection = cameraState.view * cameraState.projection;
    // const auto frustum = camera->GetFrustum();
    // const auto cameraPosition = registry->Get<Transform>(camera->ParentEntity())->Position();

    const auto renderers = View<MeshRenderer>{registry};

    auto objectData = std::vector<ObjectData>{};
    objectData.reserve(renderers.size());

    auto frontendState = ObjectFrontendState{};
    frontendState.meshes.reserve(renderers.size());

    for (const auto& renderer : renderers)
    {
        const auto& modelMatrix = registry->Get<Transform>(renderer.ParentEntity())->TransformationMatrix();

        if (!IsViewedByFrustum(cameraState.frustum, renderer, modelMatrix))
            continue;

        const auto [base, normal, roughness, metallic] = renderer.GetTextureIndices();
        objectData.emplace_back(modelMatrix, modelMatrix * cameraState.view, viewProjection, base.index, normal.index, roughness.index, metallic.index);
        frontendState.meshes.push_back(renderer.GetMesh());
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

void ObjectFrontend::Clear() noexcept
{

}
} // namespace nc::graphics

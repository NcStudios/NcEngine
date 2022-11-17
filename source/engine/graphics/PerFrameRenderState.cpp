#include "PerFrameRenderState.h"
#include "ecs/Registry.h"
#include "ecs/Transform.h"
#include "ecs/View.h"
#include "graphics/Camera.h"
#include "graphics/MeshRenderer.h"
#include "graphics/shaders/Environment.h"
#include "optick/optick.h"
#include "physics/collision/IntersectionQueries.h"
#include "shaders/ShaderResourceService.h"

namespace
{
    bool IsViewedByFrustum(const nc::physics::Frustum& frustum, const nc::graphics::MeshRenderer& renderer, DirectX::FXMMATRIX transform)
    {
        const auto maxScaleExtent = nc::GetMaxScaleExtent(transform);
        const auto maxMeshExtent = renderer.GetMesh().maxExtent;
        auto sphere = nc::physics::Sphere{ .center = nc::Vector3::Zero(), .radius = maxScaleExtent * maxMeshExtent };
        DirectX::XMStoreVector3(&sphere.center, transform.r[3]);
        return nc::physics::Intersect(frustum, sphere);
    }
}

namespace nc::graphics
{
    PerFrameRenderState::PerFrameRenderState(Registry* registry, graphics::Camera* camera, bool isPointLightSystemDirty, Environment* environment, std::span<const nc::particle::EmitterState> particleEmitters)
        : camViewMatrix{ camera->ViewMatrix() },
          projectionMatrix{ camera->ProjectionMatrix() },
          cameraPosition{ registry->Get<Transform>(camera->ParentEntity())->Position() },
          objectData{},
          pointLightInfos{},
          #ifdef NC_EDITOR_ENABLED
          colliderDebugWidget{ std::nullopt },
          #endif
          pointLightVPs{},
          isPointLightBindRequired{ isPointLightSystemDirty },
          environment{ environment },
          useSkybox{ environment->UseSkybox() },
          emitterStates{ particleEmitters }
    {
        OPTICK_CATEGORY("PerFrameRenderState", Optick::Category::Rendering);
        const auto frustum = camera->CalculateFrustum();
        const auto viewProjection = camViewMatrix * projectionMatrix;
        const auto renderers = View<MeshRenderer>{ registry };
        objectData.reserve(renderers.size());
        meshes.reserve(renderers.size());

        for (const auto& renderer : renderers)
        {
            const auto& modelMatrix = registry->Get<Transform>(renderer.ParentEntity())->TransformationMatrix();

            if (!IsViewedByFrustum(frustum, renderer, modelMatrix))
                continue;

            const auto [base, normal, roughness, metallic] = renderer.GetTextureIndices();
            objectData.emplace_back(modelMatrix, modelMatrix * camViewMatrix, viewProjection, base.index, normal.index, roughness.index, metallic.index);
            meshes.push_back(renderer.GetMesh());
        }

        environment->SetCameraPosition(cameraPosition);
        if (useSkybox)
        {
            auto skyboxMatrix = DirectX::XMMatrixScaling(200.0f, 200.0f, 200.0f);
            skyboxMatrix.r[3] = DirectX::XMVectorAdd(DirectX::XMLoadVector3(&cameraPosition), DirectX::g_XMIdentityR3);
            objectData.emplace_back(skyboxMatrix, skyboxMatrix * camViewMatrix, viewProjection, 0, 0, 0, 0);
        }

        #ifdef NC_EDITOR_ENABLED
        auto colliderIsSelected = false;
        for (auto& collider : View<physics::Collider>{ registry })
        {
            if (collider.GetEditorSelection())
            {
                colliderDebugWidget = collider.GetDebugWidget();
                colliderIsSelected = true;
            }
        }

        if (!colliderIsSelected) colliderDebugWidget = std::nullopt;
        #endif

        auto pointLights = View<PointLight>{ registry };
        pointLightVPs.reserve(pointLights.size());

        for (auto& pointLight : pointLights)
        {
            auto* transform = registry->Get<Transform>(pointLight.ParentEntity());

            pointLightVPs.push_back(pointLight.CalculateLightViewProjectionMatrix(transform->TransformationMatrix()));

            if (pointLight.Update(transform->Position(), pointLightVPs.back()))
                isPointLightBindRequired = true;
        }

        if (isPointLightBindRequired)
        {
            pointLightInfos.reserve(pointLights.size());

            for (const auto& pointLight : pointLights)
            {
                pointLightInfos.push_back(pointLight.GetInfo());
            }
        }
    }

    void MapPerFrameRenderState(const PerFrameRenderState& state)
    {
        OPTICK_CATEGORY("MapPerFrameRenderState", Optick::Category::Rendering);
        ShaderResourceService<ObjectData>::Get()->Update(state.objectData);

        if (state.isPointLightBindRequired)
        {
            ShaderResourceService<PointLightInfo>::Get()->Update(state.pointLightInfos);
        }

        auto dataVector = std::vector<EnvironmentData>{};
        dataVector.push_back(state.environment->Get());
        ShaderResourceService<EnvironmentData>::Get()->Update(dataVector);
    }
}
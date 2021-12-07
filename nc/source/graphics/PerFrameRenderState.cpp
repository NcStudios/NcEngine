#include "PerFrameRenderState.h"
#include "ecs/Registry.h"
#include "ecs/component/Camera.h"
#include "ecs/component/MeshRenderer.h"
#include "ecs/component/Transform.h"
#include "graphics/Environment.h"
#include "physics/collision/IntersectionQueries.h"
#ifdef NC_EDITOR_ENABLED
#include "physics/PhysicsSystemImpl.h"
#endif
#include "resources/ShaderResourceService.h"

namespace
{
    using namespace nc;

    bool IsViewedByFrustum(const Frustum& frustum, const MeshRenderer& renderer, DirectX::FXMMATRIX transform)
    {
        const auto maxScaleExtent = GetMaxScaleExtent(transform);
        const auto maxMeshExtent = renderer.GetMesh().maxExtent;
        Sphere sphere{.center = Vector3::Zero(), .radius = maxScaleExtent * maxMeshExtent};
        DirectX::XMStoreVector3(&sphere.center, transform.r[3]);
        return physics::Intersect(frustum, sphere);
    }
}

namespace nc::graphics
{
    PerFrameRenderState::PerFrameRenderState(Registry* registry, Camera* camera, bool isPointLightSystemDirty, Environment* environment)
    : camViewMatrix{camera->GetViewMatrix()},
      projectionMatrix{camera->GetProjectionMatrix()},
      cameraPosition{registry->Get<Transform>(camera->GetParentEntity())->GetPosition()},
      objectData{},
      pointLightInfos{},
      #ifdef NC_EDITOR_ENABLED
      colliderDebugWidget{std::nullopt},
      #endif
      pointLightVPs{},
      isPointLightBindRequired{isPointLightSystemDirty},
      environment{environment},
      useSkybox{environment->UseSkybox()}
    {
        const auto frustum = camera->CalculateFrustum();
        const auto viewProjection = camViewMatrix * projectionMatrix;
        const auto renderers = registry->ViewAll<MeshRenderer>();
        objectData.reserve(renderers.size());
        meshes.reserve(renderers.size());
        Sphere sphere;

        for(const auto& renderer : renderers)
        {
            const auto& modelMatrix = registry->Get<Transform>(renderer.GetParentEntity())->GetTransformationMatrix();
            
            if(!IsViewedByFrustum(frustum, renderer, modelMatrix))
                continue;

            const auto [base, normal, roughness, metallic] = renderer.GetTextureIndices();
            objectData.emplace_back(modelMatrix, modelMatrix * camViewMatrix, viewProjection, base.index, normal.index, roughness.index, metallic.index);
            meshes.push_back(renderer.GetMesh());
        }

        environment->Set(cameraPosition);
        useSkybox = environment->UseSkybox();
        if (useSkybox)
        {
            const auto& cameraPosition = registry->Get<Transform>(camera->GetParentEntity())->GetTransformationMatrix().r[3];
            auto skyboxMatrix = DirectX::XMMatrixScaling(100.0f, 100.0f, 100.0f);
            skyboxMatrix.r[3] = cameraPosition;

            objectData.emplace_back(skyboxMatrix, skyboxMatrix * camViewMatrix, viewProjection, 0, 0, 0, 0);
        }

        #ifdef NC_EDITOR_ENABLED
        auto colliderIsSelected = false;
        for(auto& collider : registry->ViewAll<Collider>())
        {
            if (collider.GetEditorSelection())
            {
                colliderDebugWidget = collider.GetDebugWidget();
                colliderIsSelected = true;
            }
        }

        if (!colliderIsSelected) colliderDebugWidget = std::nullopt;
        #endif

        auto pointLights = registry->ViewAll<PointLight>();
        pointLightVPs.reserve(pointLights.size());

        for(auto& pointLight : pointLights)
        {
            auto* transform = registry->Get<Transform>(pointLight.GetParentEntity());

            pointLightVPs.push_back(pointLight.CalculateLightViewProjectionMatrix(transform->GetTransformationMatrix()));

            if(pointLight.Update(transform->GetPosition(), pointLightVPs.back()))
                isPointLightBindRequired = true;
        }

        if(isPointLightBindRequired)
        {
            pointLightInfos.reserve(pointLights.size());

            std::transform(pointLights.begin(), pointLights.end(), std::back_inserter(pointLightInfos), [](auto&& component)
            {
                return component.GetInfo();
            });
        }
    }

    void MapPerFrameRenderState(const PerFrameRenderState& state)
    {
        ShaderResourceService<ObjectData>::Get()->Update(state.objectData);

        if(state.isPointLightBindRequired)
        {
            ShaderResourceService<PointLightInfo>::Get()->Update(state.pointLightInfos);
        }

        auto dataVector = std::vector<EnvironmentData>{};
        dataVector.push_back(state.environment->Get());
        ShaderResourceService<EnvironmentData>::Get()->Update(dataVector);
    }
}
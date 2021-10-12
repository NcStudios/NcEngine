#include "PerFrameRenderState.h"
#include "MainCamera.h"
#include "camera/MainCameraInternal.h"
#include "physics/collision/IntersectionQueries.h"
#include "resources/ResourceManager.h"

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
    PerFrameRenderState::PerFrameRenderState(registry_type* registry, bool isPointLightSystemDirty)
        : camViewMatrix{camera::GetViewMatrix()},
          projectionMatrix{camera::GetProjectionMatrix()},
          cameraPosition{camera::GetMainCameraTransform()->GetPosition()},
          objectData{},
          pointLightInfos{},
          pointLightVPs{},
          isPointLightBindRequired{isPointLightSystemDirty}
    {
        const auto frustum = camera::CalculateFrustum();
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

            const auto [baseIndex, normalIndex, roughnessIndex] = renderer.GetTextureIndices();
            objectData.emplace_back(modelMatrix, modelMatrix * camViewMatrix, viewProjection, baseIndex, normalIndex, roughnessIndex, 1);
            meshes.push_back(renderer.GetMesh());
        }

        auto pointLights = registry->ViewAll<PointLight>();
        pointLightVPs.reserve(pointLights.size());

        for(auto& pointLight : pointLights)
        {
            auto* transform = registry->Get<Transform>(pointLight.GetParentEntity());

            pointLightVPs.push_back(pointLight.CalculateLightViewProjectionMatrix());

            if(pointLight.Update(transform->GetPosition(), camViewMatrix, pointLightVPs.back()))
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
        ResourceManager::UpdateObjects(state.objectData);
        
        if(state.isPointLightBindRequired)
        {
            ResourceManager::UpdatePointLights(state.pointLightInfos);
        }
    }
}
#include "PerFrameRenderState.h"
#include "MainCamera.h"
#include "camera/MainCameraInternal.h"
#include "physics/collision/IntersectionQueries.h"
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
    PerFrameRenderState::PerFrameRenderState(registry_type* registry, bool isPointLightSystemDirty)
        : viewMatrix{camera::GetViewMatrix()},
          projectionMatrix{camera::GetProjectionMatrix()},
          cameraPosition{camera::GetMainCameraTransform()->GetPosition()},
          objectData{},
          pointLightInfos{},
          isPointLightBindRequired{isPointLightSystemDirty}
    {
        const auto frustum = camera::CalculateFrustum();
        const auto viewProjection = viewMatrix * projectionMatrix;
        const auto renderers = registry->ViewAll<MeshRenderer>();
        objectData.reserve(renderers.size());
        meshes.reserve(renderers.size());
        Sphere sphere;

        for(const auto& renderer : renderers)
        {
            const auto& modelMatrix = registry->Get<Transform>(renderer.GetParentEntity())->GetTransformationMatrix();
            
            if(!IsViewedByFrustum(frustum, renderer, modelMatrix))
                continue;

            const auto [base, normal, roughness] = renderer.GetTextureIndices();
            objectData.emplace_back(modelMatrix, modelMatrix * viewMatrix, viewProjection, base.index, normal.index, roughness.index, 1);
            meshes.push_back(renderer.GetMesh());
        }

        auto pointLights = registry->ViewAll<PointLight>();

        for(auto& pointLight : pointLights)
        {
            auto* transform = registry->Get<Transform>(pointLight.GetParentEntity());
            if(pointLight.Update(transform->GetPosition(), viewMatrix))
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
    }
}
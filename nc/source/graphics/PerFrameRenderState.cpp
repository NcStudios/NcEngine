#include "PerFrameRenderState.h"
#include "MainCamera.h"
#include "camera/MainCameraInternal.h"
#include "resources/ResourceManager.h"


#include "physics/collision/IntersectionQueries.h"
#include <iostream>

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

        auto viewProjection = viewMatrix * projectionMatrix;
        auto renderers = registry->ViewAll<MeshRenderer>();
        objectData.reserve(renderers.size());
        meshes.reserve(renderers.size());

        size_t culled = 0u;

        for(const auto& renderer : renderers)
        {
            const auto& modelMatrix = registry->Get<Transform>(renderer.GetParentEntity())->GetTransformationMatrix();
            const auto maxScaleExtent = GetMaxScaleExtent(modelMatrix);
            const auto maxMeshExtent = renderer.GetMesh().maxExtent;

            const auto pos_v = modelMatrix.r[3];
            Vector3 center;
            DirectX::XMStoreVector3(&center, pos_v);
            Sphere s{.center = center, .radius = maxScaleExtent * maxMeshExtent};


            if(!physics::Intersect(s, frustum))
            {
                ++culled;
                continue;
            }


            auto [baseIndex, normalIndex, roughnessIndex] = renderer.GetTextureIndices();
            objectData.emplace_back(modelMatrix, modelMatrix * viewMatrix, viewProjection, baseIndex, normalIndex, roughnessIndex, 1);
            meshes.push_back(renderer.GetMesh());

        }

        std::cout << "culled objects: " << culled << '\n';


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
        ResourceManager::UpdateObjects(state.objectData);
        
        if(state.isPointLightBindRequired)
        {
            ResourceManager::UpdatePointLights(state.pointLightInfos);
        }
    }
}
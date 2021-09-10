#pragma once

#include "MainCamera.h"
#include "camera/MainCameraInternal.h"
#include "component/PointLight.h"
#include "component/MeshRenderer.h"
#include "component/Transform.h"
#include "resources/ObjectData.h"

#include <vector>

namespace nc::graphics
{
    struct PerFrameRenderState
    {
        PerFrameRenderState(registry_type* registry, bool isPointLightSystemDirty);

        DirectX::XMMATRIX viewMatrix;
        DirectX::XMMATRIX projectionMatrix;
        Vector3 cameraPosition;
        std::vector<ObjectData> objectData;
        std::vector<Mesh> meshes;
        std::vector<PointLightInfo> pointLightInfos;
        bool isPointLightBindRequired;
    };

    inline PerFrameRenderState::PerFrameRenderState(registry_type* registry, bool isPointLightSystemDirty)
        : viewMatrix{camera::CalculateViewMatrix()},
          projectionMatrix{camera::GetProjectionMatrix()},
          cameraPosition{camera::GetMainCameraTransform()->GetPosition()},
          objectData{},
          pointLightInfos{},
          isPointLightBindRequired{isPointLightSystemDirty}
    {
        auto viewProjection = viewMatrix * projectionMatrix;
        auto renderers = registry->ViewAll<MeshRenderer>();
        objectData.reserve(renderers.size());
        meshes.reserve(renderers.size());

        for(const auto& renderer : renderers)
        {
            const auto& modelMatrix = registry->Get<Transform>(renderer.GetParentEntity())->GetTransformationMatrix();
            auto [baseIndex, normalIndex, roughnessIndex] = renderer.GetTextureIndices();
            objectData.emplace_back(modelMatrix, modelMatrix * viewMatrix, viewProjection, baseIndex, normalIndex, roughnessIndex, 1);
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
}
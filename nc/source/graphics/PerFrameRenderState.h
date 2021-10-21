#pragma once

#include "Ecs.h"
#include "component/Camera.h"
#include "resources/ObjectDataManager.h"

namespace nc::graphics
{
    struct PerFrameRenderState
    {
        PerFrameRenderState(registry_type* registry, Camera* camera, bool isPointLightSystemDirty);

        DirectX::XMMATRIX viewMatrix;
        DirectX::XMMATRIX projectionMatrix;
        Vector3 cameraPosition;
        std::vector<ObjectData> objectData;
        std::vector<MeshView> meshes;
        std::vector<PointLightInfo> pointLightInfos;
        bool isPointLightBindRequired;
    };

    void MapPerFrameRenderState(const PerFrameRenderState& state);
}
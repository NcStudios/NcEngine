#pragma once

#include "Assets.h"
#include "ecs/component/PointLight.h"
#include "resources/ObjectDataManager.h"

namespace nc
{
    class Camera;
    class Registry;
}

namespace nc::graphics
{
    struct PerFrameRenderState
    {
        PerFrameRenderState(Registry* registry, Camera* camera, bool isPointLightSystemDirty);

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
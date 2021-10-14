#pragma once

#include "Ecs.h"
#include "resources/ObjectDataManager.h"

namespace nc::graphics
{
    struct PerFrameRenderState
    {
        PerFrameRenderState(registry_type* registry, bool isPointLightSystemDirty);

        DirectX::XMMATRIX camViewMatrix;
        DirectX::XMMATRIX projectionMatrix;
        Vector3 cameraPosition;
        std::vector<ObjectData> objectData;
        std::vector<MeshView> meshes;
        std::vector<PointLightInfo> pointLightInfos;
        std::vector<DirectX::XMMATRIX> pointLightVPs;
        bool isPointLightBindRequired;
    };

    void MapPerFrameRenderState(const PerFrameRenderState& state);
}
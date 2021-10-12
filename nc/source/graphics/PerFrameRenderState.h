#pragma once

#include "Ecs.h"
#include "resources/ObjectData.h"

namespace nc::graphics
{
    struct PerFrameRenderState
    {
        PerFrameRenderState(registry_type* registry, bool isPointLightSystemDirty);

        DirectX::XMMATRIX camViewMatrix;
        DirectX::XMMATRIX projectionMatrix;
        Vector3 cameraPosition;
        std::vector<ObjectData> objectData;
        std::vector<Mesh> meshes;
        std::vector<PointLightInfo> pointLightInfos;
        std::vector<DirectX::XMMATRIX> pointLightVPs;
        bool isPointLightBindRequired;
    };

    void MapPerFrameRenderState(const PerFrameRenderState& state);
}
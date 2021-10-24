#pragma once

#include "Ecs.h"
#include "resources/ObjectDataManager.h"
#include "component/DebugWidget.h"

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
        #ifdef NC_EDITOR_ENABLED
        std::optional<nc::DebugWidget> colliderDebugWidget;
        #endif
        std::vector<DirectX::XMMATRIX> pointLightVPs;
        bool isPointLightBindRequired;
    };

    void MapPerFrameRenderState(const PerFrameRenderState& state);
}
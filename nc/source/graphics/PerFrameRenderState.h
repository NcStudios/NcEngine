#pragma once

#include "Assets.h"
#include "ecs/component/PointLight.h"
#include "resources/ObjectDataManager.h"
#include "component/DebugWidget.h"

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
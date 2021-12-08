#pragma once

#include "Assets.h"
#include "ecs/component/PointLight.h"
#include "ecs/Registry.h"
#ifdef NC_EDITOR_ENABLED
#include "ecs/component/DebugWidget.h"
#endif
#include "resources/ObjectDataManager.h"

namespace nc
{
    class Camera;
    class EnvironmentImpl;
    class Registry;
    #ifdef NC_DEBUG_RENDERING
    namespace physics { class PhysicsSystemImpl; }
    #endif
}

namespace nc::graphics
{
    struct PerFrameRenderState
    {
        PerFrameRenderState(Registry* registry, Camera* camera, bool isPointLightSystemDirty, EnvironmentImpl* environment);

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
        EnvironmentImpl* environment;
        bool useSkybox;
    };

    void MapPerFrameRenderState(const PerFrameRenderState& state);
}
#pragma once
#include "config/Config.h"
#include "component/Transform.h"
#include "scene/Scene.h"

#include <memory>

namespace nc::engine::internal
{
    struct EngineData
    {
        EngineData() = default;

        EngineData(config::Config config)
            : mainCameraTransform{ nullptr },
              configData{ std::move(config) },
              isRunning{ true },
              frameDeltaTimeFactor{ 1.0f },
              isSceneSwapScheduled{ false },
              activeScene{ nullptr },
              swapScene{ nullptr }
        {
        }
        
        Transform * mainCameraTransform; //non-owning - set by NcSetMainCamera, returned from NcGetMainCamera
        config::Config configData;
        bool isRunning;
        float frameDeltaTimeFactor; //for slowing/pausing in debug editor

        bool isSceneSwapScheduled; // if true, activeScene will be cleared and swapScene will be loaded at end of current frame
        std::unique_ptr<scene::Scene> activeScene;
        std::unique_ptr<scene::Scene> swapScene;
    };
}
#pragma once
#include "system/TransformSystem.h"
#include "system/RenderingSystem.h"
#include "system/LightSystem.h"
#include "system/CollisionSystem.h"
#include "system/EntitySystem.h"
#include "win32/Window.h"
#include "time/NcTime.h"
#include "ui/UI.h"

namespace nc::engine::internal
{
    struct EngineSystems
    {
        EngineSystems() = default;

        EngineSystems(int screenWidth, int screenHeight, HWND hwnd)
        {
            rendering = std::make_unique<system::RenderingSystem>(screenWidth, screenHeight, hwnd);
            entity = std::make_unique<system::EntitySystem>();
            transform = std::make_unique<system::ComponentSystem<Transform>>();
            light = std::make_unique<system::LightSystem>();
            collision = std::make_unique<system::CollisionSystem>();
            ui = std::make_unique<ui::UI>(hwnd, rendering->GetGraphics());
            #ifdef NC_EDITOR_ENABLED
            frameLogicTimer = std::make_unique<nc::time::Timer>();
            #endif
        }

        std::unique_ptr<system::EntitySystem> entity;
        std::unique_ptr<system::ComponentSystem<Transform>> transform;
        std::unique_ptr<system::RenderingSystem> rendering;
        std::unique_ptr<system::LightSystem> light;
        std::unique_ptr<system::CollisionSystem> collision;
        std::unique_ptr<ui::UI> ui;
        #ifdef NC_EDITOR_ENABLED
        std::unique_ptr<nc::time::Timer> frameLogicTimer;
        #endif
    };
}


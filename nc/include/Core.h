#pragma once

#include "platform/win32/HInstanceForwardDecl.h"
#include "config/Config.h"
#include "Scene.h"

namespace nc::core
{
    /** Create the engine and all required systems. */
    void Initialize(HINSTANCE hInstance, const std::string& configPath);

    /** Load initialScene and enters the game loop. */
    void Start(std::unique_ptr<scene::Scene> initialScene);
    
    /** Exit the game loop after completing the current frame. If control cannot be returned to the
     *  loop(e.g. an exception), setting forceImmediate to true will force clearing state data. */
    void Quit(bool forceImmediate = false) noexcept;
    
    /** Destroy the engine instance. */
    void Shutdown() noexcept;
}
#include "NCWin32.h"
#include "Window.h"
#include "config/Config.h"
#include "engine/Engine.h"
#include "debug/NcException.h"
#include "graphics/Graphics.h"

#ifdef NC_EDITOR_ENABLED
#include "utils/editor/EditorManager.h"
#endif

#include <iostream>
#include <memory>

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
    (void)prevInstance;
    (void)commandLine;
    (void)showCommand;

    const auto configPaths = nc::config::detail::ConfigPaths
    {
        "project/config/project.ini",
        "project/config/graphics.ini",
        "project/config/physics.ini"
    };

    auto config = nc::config::detail::Read(configPaths);
    
    nc::Window window(instance, config);
    
    auto engine = std::make_unique<nc::engine::Engine>(std::move(config));

    #ifdef NC_EDITOR_ENABLED
    window.BindEditorManager(engine->GetEditorManager());
    #endif

    try
    {
        engine->MainLoop();
    }
    catch(const nc::NcException& e)
    {
        e.what();
        engine->Exit();
    }
    catch(...)
    {
        std::cerr << "WinMain.cpp - unkown exception caught\n";
    }

	return 0;
}
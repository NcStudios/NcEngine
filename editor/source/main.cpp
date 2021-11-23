#include "platform/win32/NcWin32.h"
#include "NcEngine.h"
#include "debug/Utils.h"
#include "EditorScene.h"
#include "framework/EditorFramework.h"

#include <iostream>

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    std::unique_ptr<nc::NcEngine> engine = nullptr;

    try
    {
        engine = nc::InitializeNcEngine(instance, "editor/config/engine_config.ini", true);
        nc::editor::EditorFramework framework{engine.get()};
        engine->Start(std::make_unique<nc::editor::EditorScene>(framework.GetProjectManager()));
        framework.SaveProjectData();
    }
    catch(std::exception& e)
    {
        nc::debug::LogException(e);
    }
    catch(...)
    {
        std::cerr << "WinMain - unkown exception caught\n";
        nc::debug::LogToDiagnostics("WinMain - unkown exception");
    }

    if(engine) engine->Shutdown();
    engine = nullptr;
	return 0;
}
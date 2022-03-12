#include "NcEngine.h"
#include "debug/Utils.h"
#include "EditorScene.h"
#include "framework/EditorFramework.h"

#include <iostream>

int main()
{
    std::unique_ptr<nc::NcEngine> engine = nullptr;

    try
    {
        engine = nc::InitializeNcEngine("editor/config/engine_config.ini", nc::EngineInitFlags::NoPhysics);
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
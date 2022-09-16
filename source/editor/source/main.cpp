#include "EditorScene.h"
#include "framework/EditorFramework.h"

#include "NcEngine.h"
#include "utility/Log.h"

#include <iostream>

int main()
{
    std::unique_ptr<nc::NcEngine> engine = nullptr;

    try
    {
        const auto config = nc::config::Load("engine_config.ini");
        engine = nc::InitializeNcEngine(config, nc::EngineInitFlags::NoPhysics);
        nc::editor::EditorFramework framework{engine.get()};
        engine->Start(std::make_unique<nc::editor::EditorScene>(framework.GetProjectManager()));
        framework.SaveProjectData();
    }
    catch(std::exception& e)
    {
        NC_LOG_EXCEPTION(e);
    }
    catch(...)
    {
        NC_LOG_ERROR("WinMain - unkown exception");
        std::cerr << "WinMain - unkown exception caught\n";
    }

    if(engine) engine->Shutdown();
    engine = nullptr;
	return 0;
}
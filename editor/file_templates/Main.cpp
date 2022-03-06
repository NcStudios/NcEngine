#include "platform/win32/NcWin32.h"
#include "nc_engine.h"
#include "debug/Utils.h"
#include "@INCLUDE_INITIAL_SCENE@"

#include <iostream>

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    std::unique_ptr<nc::NcEngine> engine;

    try
    {
        engine = nc::InitializeNcEngine(instance, "config/config.ini");
        engine->Start(std::make_unique<@INITIAL_SCENE_TYPE@>());
    }
    catch(std::exception& e)
    {
        nc::debug::LogException(e);
    }
    catch(...)
    {
        std::cerr << "WinMain.cpp - unkown exception caught\n";
        nc::debug::LogToDiagnostics("WinMain.cpp - unkown exception");
    }

    if(engine)
    {
        engine->Shutdown();
        engine = nullptr;
    }

    return 0;
}
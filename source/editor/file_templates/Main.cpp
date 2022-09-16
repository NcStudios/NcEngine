#include "platform/win32/NcWin32.h"
#include "NcEngine.h"
#include "utility/Log.h"
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
        LOG_EXCEPTION(e);
    }
    catch(...)
    {
        std::cerr << "WinMain.cpp - unkown exception caught\n";
        LOG_ERROR("WinMain.cpp - unkown exception");
    }

    return 0;
}
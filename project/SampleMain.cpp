#include "platform/win32/NcWin32.h"
#include "Core.h"
#include "debug/Utils.h"
#include "worms/Worms.h"

#include <iostream>

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    std::unique_ptr<nc::NcEngine> engine;

    try
    {
        engine = std::make_unique<nc::NcEngine>(instance, "project/config.ini");
        engine->Start(std::make_unique<nc::sample::Worms>());
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
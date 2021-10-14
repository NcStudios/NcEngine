#include "platform/win32/NcWin32.h"
#include "Core.h"
#include "debug/Utils.h"
#include "worms/Worms.h"

#include <iostream>

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    try
    {
        nc::core::Initialize(instance, "project/config.ini");
        nc::core::Start(std::make_unique<nc::sample::Worms>());
    }
    catch(std::exception& e)
    {
        nc::debug::LogException(e);
        nc::core::Quit(true);
    }
    catch(...)
    {
        std::cerr << "WinMain.cpp - unkown exception caught\n";
        nc::debug::LogToDiagnostics("WinMain.cpp - unkown exception");
        nc::core::Quit(true);
    }

    nc::core::Shutdown();
	return 0;
}
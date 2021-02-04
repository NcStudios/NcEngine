#include "platform/win32/NcWin32.h"
#include "Core.h"
#include "debug/Utils.h"
#include "scenes/MenuScene.h"

#include <iostream>

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    try
    {
        nc::core::Initialize(instance);
        nc::core::Start(std::make_unique<project::MenuScene>());
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
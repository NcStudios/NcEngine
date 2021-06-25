#include "platform/win32/NcWin32.h"
#include "Core.h"
#include "debug/Utils.h"
#ifdef USE_VULKAN
    #include "solar_system/SolarSystem.h"
#else
    #include "worms/Worms.h"
#endif

#include <iostream>

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    try
    {
        nc::core::Initialize(instance);
#ifdef USE_VULKAN
        nc::core::Start(std::make_unique<nc::sample::SolarSystem>());
#else
        nc::core::Start(std::make_unique<nc::sample::Worms>());
#endif
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
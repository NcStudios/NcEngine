#include "win32/NcWin32.h"
#include "config/Config.h"
#include "NcDebug.h"
#include "NcEngine.h"

#include <iostream>

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

    nc::engine::NcInitializeEngine(instance, std::move(configPaths));

    try
    {
        nc::engine::NcStartEngine();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "Fatal error:\n" << e.what();
        nc::engine::NcShutdownEngine();
    }
    catch(std::exception& e)
    {
        std::cerr << "Exception: \n";
        std::cerr << e.what();
        nc::engine::NcShutdownEngine();
    }
    catch(...)
    {
        std::cerr << "WinMain.cpp - unkown exception caught\n";
        nc::engine::NcShutdownEngine();
    }

	return 0;
}
#include "win32/NcWin32.h"
#include "config/Config.h"
#include "NcDebug.h"
#include "NcEngine.h"
#include "NcLog.h"
#include "NcUI.h"
#include "project/source/ui/UI.h"
#include "project/source/log/GameLog.h"

#include <iostream>

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
    (void)prevInstance;
    (void)commandLine;
    (void)showCommand;

    try
    {
        nc::engine::NcInitializeEngine(instance);
        project::log::GameLog gameLog;
        nc::log::NcRegisterGameLog(&gameLog);
        project::ui::UI projectUI(&gameLog);
        nc::ui::NcRegisterUI(&projectUI);
        nc::engine::NcStartEngine();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "Fatal error:\n" << e.what();
        nc::log::NcLogToDiagnostics(e.what());
        nc::engine::NcShutdownEngine(true);
    }
    catch(std::exception& e)
    {
        std::cerr << "Exception: \n" << e.what();
        nc::log::NcLogToDiagnostics(e.what());
        nc::engine::NcShutdownEngine(true);
    }
    catch(...)
    {
        std::cerr << "WinMain.cpp - unkown exception caught\n";
        nc::log::NcLogToDiagnostics("WinMain.cpp - unkown exception");
        nc::engine::NcShutdownEngine(true);
    }

    std::cout << "WinMain - Exiting\n";
	return 0;
}
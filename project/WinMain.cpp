#include "win32/NcWin32.h"
#include "config/Config.h"
#include "NcDebug.h"
#include "NcLog.h"
#include "NcUI.h"
#include "project/source/ui/UI.h"
#include "project/source/log/GameLog.h"
#include "engine/Engine.h"

#include <iostream>

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
    (void)prevInstance;
    (void)commandLine;
    (void)showCommand;

    std::unique_ptr<nc::engine::Engine> engine = nullptr;

    try
    {
        engine = std::make_unique<nc::engine::Engine>(instance);
        project::log::GameLog gameLog;
        project::ui::UI projectUI(&gameLog);
        nc::ui::NcRegisterUI(&projectUI);
        engine->Start();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "Fatal error:\n" << e.what();
        nc::log::NcLogToDiagnostics(e.what());
        if(engine)
            engine->Shutdown(true);
    }
    catch(std::exception& e)
    {
        std::cerr << "Exception: \n" << e.what();
        nc::log::NcLogToDiagnostics(e.what());
        if(engine)
            engine->Shutdown(true);

    }
    catch(...)
    {
        std::cerr << "WinMain.cpp - unkown exception caught\n";
        nc::log::NcLogToDiagnostics("WinMain.cpp - unkown exception");
        if(engine)
            engine->Shutdown(true);
    }

    std::cout << "WinMain - Exiting\n";
	return 0;
}
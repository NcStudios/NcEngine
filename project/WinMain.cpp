#include "win32/NcWin32.h"
#include "DebugUtils.h"
#include "Engine.h"
#include "project/scenes/MenuScene.h"

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
        engine->Start(std::make_unique<MenuScene>());
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "Fatal error:\n" << e.what();
        nc::debug::Log::LogToDiagnostics(e.what());
        if(engine)
            engine->Shutdown(true);
    }
    catch(std::exception& e)
    {
        std::cerr << "Exception: \n" << e.what();
        nc::debug::Log::LogToDiagnostics(e.what());
        if(engine)
            engine->Shutdown(true);

    }
    catch(...)
    {
        std::cerr << "WinMain.cpp - unkown exception caught\n";
        nc::debug::Log::LogToDiagnostics("WinMain.cpp - unkown exception");
        if(engine)
            engine->Shutdown(true);
    }

    std::cout << "WinMain - Exiting\n";
	return 0;
}
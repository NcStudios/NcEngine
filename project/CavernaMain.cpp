#include "nc/source/win32/NcWin32.h"
#include "DebugUtils.h"
#include "Engine.h"
#include "scenes/MenuScene.h"

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
        engine->Start(std::make_unique<project::MenuScene>());
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "std::runtime_error:\n" << e.what() << '\n';
        nc::debug::Log::LogToDiagnostics("std::runtime_error:");
        nc::debug::Log::LogToDiagnostics(e.what());
        if(engine)
            engine->Shutdown(true);
    }
    catch(std::exception& e)
    {
        std::cerr << "std::exception: \n" << e.what() << '\n';
        nc::debug::Log::LogToDiagnostics("std::exception:");
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
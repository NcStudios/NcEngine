#include "nc/source/win32/NcWin32.h"
#include "DebugUtils.h"
#include "Engine.h"
#include "scenes/MenuScene.h"

#include <iostream>

void LogException(const std::exception& e)
{
    std::cerr << "Exception: " << e.what() << '\n';
    nc::debug::Log::LogToDiagnostics("**Exception**");
    nc::debug::Log::LogToDiagnostics(e.what());
    try
    {
        std::rethrow_if_nested(e);
    }
    catch(const std::exception& e)
    {
        LogException(e);
    }
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    std::unique_ptr<nc::engine::Engine> engine = nullptr;

    try
    {
        engine = std::make_unique<nc::engine::Engine>(instance);
        engine->Start(std::make_unique<project::MenuScene>());
    }
    catch(std::exception& e)
    {
        LogException(e);
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
#include "NcEngine.h"
#include "debug/Utils.h"
#include "scenes/Worms.h"
#include "shared/SampleUI.h"

#include <iostream>

int main()
{
    std::unique_ptr<nc::NcEngine> engine;

    try
    {
        engine = nc::InitializeNcEngine("config.ini");
        auto ui = nc::sample::InitializeSampleUI(engine.get());
        engine->Start(std::make_unique<nc::sample::Worms>(ui.get()));
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

    return 0;
}
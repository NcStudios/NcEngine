#include "nc_engine.h"
#include "debug/Utils.h"
#include "scenes/Worms.h"

#include <iostream>

int main()
{
    std::unique_ptr<nc::nc_engine> engine;

    try
    {
        engine = nc::initialize_nc_engine("project/config.ini");
        engine->start(std::make_unique<nc::sample::Worms>());
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

    if(engine)
    {
        engine->shutdown();
        engine = nullptr;
    }

    return 0;
}
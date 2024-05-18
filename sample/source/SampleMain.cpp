#include "scenes/PhysicsTest.h"
#include "scenes/SmokeTest.h"
#include "shared/SampleUI.h"
#include "shared/Prefabs.h"

#include "ncengine/NcEngine.h"
#include "ncengine/utility/Log.h"

#include <iostream>


#include <windows.h>
#include <sstream>

LONG handler(_EXCEPTION_POINTERS* ExceptionInfo)
{
    std::cerr << "handler -- Fatal: Unhandled exception 0x" << std::hex << ExceptionInfo->ExceptionRecord->ExceptionCode << '\n';

    return EXCEPTION_CONTINUE_SEARCH;
}

LONG vectored_handler(_EXCEPTION_POINTERS* ExceptionInfo)
{
    std::cerr << "vectored_handler -- Fatal: Unhandled exception 0x" << std::hex << ExceptionInfo->ExceptionRecord->ExceptionCode << '\n';
    return EXCEPTION_CONTINUE_SEARCH;
}

int main(int argc, char** argv)
{
    SetUnhandledExceptionFilter(handler);
    AddVectoredExceptionHandler(1, vectored_handler);

    std::cerr << "starting main\n";

    const auto runSmokeTest = [argc, argv]()
    {
        return argc > 1 && argv[1] == std::string_view{"--run-test"};
    }();

    std::unique_ptr<nc::NcEngine> engine;

    try
    {
        std::cerr << "initializing engine\n";

        const auto config = nc::config::Load("config.ini");
        engine = nc::InitializeNcEngine(config);
        nc::sample::InitializeResources();

        if (runSmokeTest)
        {
            std::cerr << "loading SmokeTest\n";

            engine->Start(std::make_unique<nc::sample::SmokeTest>(
                [instance = engine.get()](){ instance->Stop(); }
            ));
        }
        else
        {
            auto ui = nc::sample::InitializeSampleUI(engine.get());
            engine->Start(std::make_unique<nc::sample::PhysicsTest>(ui.get()));
        }
    }
    catch(std::exception& e)
    {
        NC_LOG_EXCEPTION(e);
        return -1;
    }
    catch(...)
    {
        NC_LOG_ERROR("SampleMain.cpp - unknown exception");
        std::cerr << "SampleMain.cpp - unknown exception\n";
        return -1;
    }

    std::cerr << "exiting main\n";

    return 0;
}

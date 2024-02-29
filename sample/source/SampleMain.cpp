#include "scenes/PhysicsTest.h"
#include "scenes/SmokeTest.h"
#include "shared/SampleUI.h"
#include "shared/Prefabs.h"

#include "ncengine/NcEngine.h"
#include "ncengine/utility/Log.h"

#include <iostream>

int main(int argc, char** argv)
{
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
    }
    catch(...)
    {
        NC_LOG_ERROR("SampleMain.cpp - unknown exception");
        std::cerr << "SampleMain.cpp - unknown exception\n";
    }

    std::cerr << "exiting main\n";

    return 0;
}

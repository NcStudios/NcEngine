#include "scenes/PhysicsTest.h"
#include "shared/SampleUI.h"
#include "shared/Prefabs.h"

#include "ncengine/NcEngine.h"
#include "ncengine/utility/Log.h"

#include <iostream>

int main()
{
    std::unique_ptr<nc::NcEngine> engine;

    try
    {
        const auto config = nc::config::Load("config.ini");
        engine = nc::InitializeNcEngine(config);
        nc::sample::InitializeResources();
        auto ui = nc::sample::InitializeSampleUI(engine.get());
        engine->Start(std::make_unique<nc::sample::PhysicsTest>(ui.get()));
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

    return 0;
}

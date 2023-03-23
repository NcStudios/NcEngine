#include "scenes/Worms.h"
#include "shared/SampleUI.h"

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
        auto ui = nc::sample::InitializeSampleUI(engine.get());
        engine->Start(std::make_unique<nc::sample::Worms>(ui.get()));

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
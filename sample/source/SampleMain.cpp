#include "scenes/PhysicsTest.h"
#include "scenes/SmokeTest.h"
#include "shared/SampleUI.h"
#include "shared/Prefabs.h"

#include "ncengine/NcEngine.h"
#include "ncengine/utility/Log.h"

#include <iostream>

struct Args
{
    bool runSmokeTest = false;
    std::string configPath = "config.ini";
};

auto ParseArgs(int argc, char** argv) -> Args
{
    auto args = Args{};
    for (auto i = 1; i < argc; ++i)
    {
        const auto arg = std::string_view{argv[i]};
        if (arg == "--run-test")
        {
            args.runSmokeTest = true;
        }
        else if (arg == "--config-path" && i + 1 < argc)
        {
            args.configPath = argv[++i];
        }
        else
        {
            std::cerr << "Unknown argument: " << arg << '\n';
            throw nc::NcError("Bad argument");
        }
    }

    return args;
}

int main(int argc, char** argv)
{
    std::unique_ptr<nc::NcEngine> engine;

    try
    {
        const auto args = ParseArgs(argc, argv);
        const auto config = nc::config::Load(args.configPath);
        engine = nc::InitializeNcEngine(config);
        nc::sample::InitializeResources();

        if (args.runSmokeTest)
        {
            engine->Start(std::make_unique<nc::sample::SmokeTest>(
                [&quit = engine->GetSystemEvents().quit](){ quit.Emit(); }
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

    return 0;
}

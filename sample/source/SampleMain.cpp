#include "scenes/PhysicsTest.h"
#include "scenes/SmokeTest.h"
#include "shared/SampleUI.h"
#include "shared/Prefabs.h"

#include "ncengine/NcEngine.h"
#include "ncengine/utility/FileLogger.h"

#include <iostream>
#include <ranges>

struct Args
{
    bool runSmokeTest = false;             // if true, runs an automated test workflow
    std::string logPath = "";              // optional file to log to; uses stdout if empty
    std::string configPath = "config.ini"; // file to read config from; overriding is intended for automated testing
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
        else if (arg == "--log-path" && i + 1 < argc)
        {
            args.logPath = argv[++i];
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
    const auto args = ParseArgs(argc, argv);
    const auto logger = args.logPath.empty()
        ? nullptr
        : std::make_unique<nc::FileLogger>(args.logPath);

    try
    {
        auto config = nc::config::Load(args.configPath);
        auto ui = std::unique_ptr<nc::sample::SampleUI>{};
        auto engine = nc::InitializeNcEngine(config);
        nc::sample::InitializeResources();

        if (args.runSmokeTest)
        {
            engine->Start(std::make_unique<nc::sample::SmokeTest>(
                [&quit = engine->GetSystemEvents().quit](){ quit.Emit(); }
            ));
        }
        else
        {
            ui = nc::sample::InitializeSampleUI(engine.get());
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
        return -1;
    }

    return 0;
}

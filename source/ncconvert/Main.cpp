#include "Config.h"
#include "ReturnCodes.h"
#include "Usage.h"
#include "builder/BuildOrchestrator.h"
#include "utility/EnumExtensions.h"

#include "ncasset/NcaHeader.h"
#include "ncutility/NcError.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <string>

auto ParseArgs(int argc, char** argv, nc::convert::Config* config, const char** usage) -> bool;

int main(int argc, char** argv)
{
    auto config = nc::convert::Config{};
    const char* usage = nc::convert::usageMessage;
    if (!ParseArgs(argc, argv, &config, &usage))
    {
        std::cout << "nc-convert v" << nc::asset::currentVersion << '\n' << usage;
        return ResultCode::ArgumentError;
    }

    try
    {
        auto builder = nc::convert::BuildOrchestrator{config};
        builder.RunBuild();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return ResultCode::RuntimeError;
    }
    catch(...)
    {
        std::cerr << "Unknown exception\n";
        return ResultCode::RuntimeError;
    }

    return ResultCode::Success;
}

auto ParseArgs(int argc, char** argv, nc::convert::Config* out, const char** usage) -> bool
{
    if(argc == 1)
    {
        return false;
    }

    auto option = std::string{};
    auto current = 1;
    while (current < argc)
    {
        option = argv[current];
        std::transform(option.begin(), option.end(), option.begin(), [](auto&& c)
        {
            return std::tolower(c);
        });

        if (option == "-h")
        {
            if (current + 1 < argc)
            {
                const auto subCommand = std::string_view{argv[current + 1]};
                if (subCommand == "manifest")
                {
                    *usage = nc::convert::manifestUsageMessage;
                }
                else if (subCommand == "generate")
                {
                    *usage = nc::convert::generateUsageMessage;
                }
            }

            return false;
        }
        else if (++current >= argc)
        {
            return false;
        }
        else if (option == "-s")
        {
            out->mode = nc::convert::OperationMode::SingleTarget;
            out->targetPath = std::filesystem::path(argv[current++]);
            out->targetPath.value().make_preferred();
        }
        else if (option == "-n")
        {
            out->assetName = std::string{argv[current++]};
        }
        else if (option == "-t")
        {
            out->targetType = nc::convert::ToAssetType(std::string{argv[current++]});
        }
        else if (option == "-m")
        {
            out->mode = nc::convert::OperationMode::Manifest;
            out->manifestPath = std::filesystem::path(argv[current++]);
            out->manifestPath.value().make_preferred();
        }
        else if (option == "-g")
        {
            out->mode = nc::convert::OperationMode::GenerateSource;
            out->manifestPath = std::filesystem::path(argv[current++]);
            out->manifestPath.value().make_preferred();
        }
        else if (option == "-o")
        {
            out->outputDirectory = std::filesystem::path(argv[current++]);
            out->outputDirectory.make_preferred();
        }
        else if (option == "-i")
        {
            out->mode = nc::convert::OperationMode::Inspect;
            out->targetPath = std::filesystem::path(argv[current++]);
            out->targetPath.value().make_preferred();
        }
        else if (option == "-r")
        {
            out->rootNamespace = std::string{argv[current++]};
        }
        else if (option == "-f")
        {
            out->textureFormat = nc::convert::ToTextureFormat(argv[current++]);
        }
        else if (option == "-p")
        {
            const auto processingOption = std::string_view{argv[current++]};
            if (processingOption == "optimize")
            {
                out->optimizeMesh = true;
            }
            else if (processingOption == "generateMips")
            {
                out->generateMips = true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    switch (out->mode)
    {
        case nc::convert::OperationMode::Unspecified:
            return false;
        case nc::convert::OperationMode::SingleTarget:
            return out->targetPath.has_value() && out->targetType.has_value() && out->assetName.has_value();
        case nc::convert::OperationMode::Manifest:
            return out->manifestPath.has_value();
        case nc::convert::OperationMode::GenerateSource:
            return out->manifestPath.has_value();
        case nc::convert::OperationMode::Inspect:
            return out->targetPath.has_value();
    }

    return false;
}

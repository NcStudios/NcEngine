#include "Config.h"
#include "ReturnCodes.h"
#include "builder/BuildOrchestrator.h"
#include "utility/EnumExtensions.h"

#include "ncasset/NcaHeader.h"
#include "ncutility/NcError.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <string>

constexpr auto usageMessage = 
R"(Usage: nc-convert [options]
Options
  -h                      Display this information.
  -t <asset type>         Specify asset type for a single target.
  -s <source>             Parse a single asset from <source>.
  -n <name>               Specify the asset name for a single target.
  -o <dir>                Output assets to <dir>.
  -m <manifest>           Perform conversions specified in <manifest>.
  -i <assetPath>          Print details about an existing asset file.

Asset types               Supported file types      Can produce multiple assets
  mesh                    fbx, obj                  true
  hull-collider           fbx, obj                  false
  concave-collider        fbx, obj                  false
  texture                 jpg, png, bmp             false
  cube-map                jpg, png, bmp             false
  audio-clip              wav                       false

Asset names
  The provided asset name is used to construct the output file path. It may
  optionally contain the ".nca" file extension and can be prefixed with
  subdirectories ("meshes/level1/myMesh.nca"). Every asset should always have
  a unique name.

Json Manifest
  A provided manifest should be a json file containing an array of conversion
  specifications for each required asset type, and an optional 'globalOptions'
  object defining global settings. Relative paths within `globalOptions` will
  be interpreted relative to the manifest. Example:
  {
      "globalOptions": {
          "outputDirectory": "./", // default: "./"
          "workingDirectory": "./" // default: "./"
      },
      "mesh": [
          {
              "sourcePath": "path/to/mesh1.fbx",
              "assetNames": [
                  {
                      "subResourceName" : "mesh1head",
                      "assetName" : "head"
                  },
                  {
                      "subResourceName" : "mesh1shoulders",
                      "assetName" : "shoulders"
                  }
              ]
          },
          {
              "sourcePath": "path/to/mesh2.fbx",
              "assetName": "mesh2"
          }
      ],
      "texture": [
          {
              "sourcePath": "path/to/texture.png",
              "assetName": "myTexture"
          }
      ]
  }

Return Values
  Success: 0
  RuntimeError: 1
  ArgumentError: 2
)";

bool ParseArgs(int argc, char** argv, nc::convert::Config* config);

int main(int argc, char** argv)
{
    auto config = nc::convert::Config{};
    if(!ParseArgs(argc, argv, &config))
    {
        std::cout << "nc-convert v" << nc::asset::currentVersion << '\n'
                  << usageMessage;
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

bool ParseArgs(int argc, char** argv, nc::convert::Config* out)
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
        else
        {
            return false;
        }
    }

    switch (out->mode)
    {
        case nc::convert::OperationMode::Unspecified:
        {
            return false;
        }
        case nc::convert::OperationMode::SingleTarget:
        {
            return out->targetPath.has_value() && out->targetType.has_value() && out->assetName.has_value();
        }
        case nc::convert::OperationMode::Manifest:
        {
            return out->manifestPath.has_value();
        }
        case nc::convert::OperationMode::Inspect:
        {
            return out->targetPath.has_value();
        }
    }

    return false;
}

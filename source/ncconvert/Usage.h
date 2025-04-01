#pragma once

namespace nc::convert
{
constexpr auto usageMessage = 
R"(Usage: nc-convert [options]
Options
    -h                         Display this information.
    -h manifest | generate     Dispaly help for specific modes.
    -t <asset type>            Specify asset type for a single target.
    -s <source>                Parse a single asset from <source>.
    -n <name>                  Specify the asset name for a single target.
    -o <dir>                   Output assets or source files to <dir>.
    -m <manifest>              Perform conversions specified in <manifest>.
    -g <manifest>              Generate source code based on <manifest>.
    -i <assetPath>             Print details about an existing asset file.
    -r <namespace>             Specify root namespace to use for generated source code.
    -f <texture format>        For textures built in single target mode, specify texture
                               format (default RGB8_UNORM_SRGB).
    -p optimize | generateMips In single target mode, enable additional processing options.
                               For meshes, 'optimize' enables a mesh optimization pass. For
                               textures, 'generateMips' creates a mipmap chain instead of
                               a single texture.

Asset types               Supported file types      Can produce multiple assets
    audio-clip              wav                       false
    convex-hull             fbx, obj                  false
    cube-map                jpg, png, bmp             false
    mesh                    fbx, obj                  true
    mesh-collider           fbx, obj                  false
    texture                 jpg, png, bmp             false

Texture Formats
    RGBA8_UNORM_SRGB
    RGBA8_UNORM
    BC1_UNORM_SRGB
    BC1_UNORM
    BC3_UNORM_SRGB
    BC3_UNORM

Asset names
    The provided asset name is used to construct the output file path. It may
    optionally contain the ".nca" file extension and can be prefixed with
    subdirectories ("meshes/level1/myMesh.nca"). Every asset should always have
    a unique name.

Return Values
    Success: 0
    RuntimeError: 1
    ArgumentError: 2
)";

constexpr auto manifestUsageMessage =
R"(Usage: nc-convert -m <manifest> [options]
Build assets specified in <manifest>. This allows batch processessing as well as
skipping conversions for up-to-date assets.
Options
    -o <dir>                Output assets to <dir>. This can also be specified
                            directly in the manifest.

Json Manifest
    A provided manifest should be a json file containing an array of conversion
    specifications for each required asset type, and an optional 'globalOptions'
    object defining global settings. Default target 'options' can be defined for
    relevant asset types in the 'globalOptions' object. These can overriden on
    a per-asset-basis. Relative paths within `globalOptions` will be interpreted
    relative to the manifest.

    Other items in the manifest are arrays keyed off of the asset types listed
    above, with the exception of textures. Textures should be listed in one of
    four arrays, "diffuse", "normal", "particle", or "effect", under a "texture"
    object.

    globalOptions:
        outputDirectory:               string  (default "./")
        workingDirectory:              string  (default "./")
        defaultMeshOptions:            options
        defaultDiffuseTextureOptions:  options
        defaultNormalTextureOptions:   options
        defaultParticleTextureOptions: options
        defaultEffectTextureOptions:   options

    options:
        optimizeMesh:                  bool   (default false, mesh only)
        generateMips:                  bool   (default false, texture only)
        textureFormat:                 string (default "RGBA8_UNORM_SRGB", texture only)

    Example:
    {
        "globalOptions": {
            "outputDirectory": "./", // default: "./"
            "workingDirectory": "./", // default: "./"
            "defaultMeshOptions": {
                "optimizeMesh": true
            }
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
        "texture": {
            "diffuse" : [
            {
                "sourcePath": "path/to/texture.png",
                "assetName": "myTexture",
                "options": {
                    "textureFormat": "BC3_UNORM_SRGB"
                }
            }
            ],
            "normal" : [
            ]
        }
    }
)";

constexpr auto generateUsageMessage =
R"(Usage: nc-convert -g <manifest> [options]
Generate source code for enumerating and loading assets in <manifest>.
Options:
    -o <dir>                Output source files to <dir>. The output directory in the
                            manifest is ignored for this mode as it points to an asset
                            directory, not a source directory.
    -r <namespace>          Specify root namespace to use for generated source code.

This outputs the files `GeneratedAssets.h` and `GeneratedAssets.cpp` which are populated
with asset paths and ids or views, depending on the asset type. Each type is nested in its
own namespace.

Asset namespaces will have a `Load()` function to load every asset in that namespace. If the
asset type enumerates asset views, like meshes or textures, there will also be an `Acquire()`
function to re-fetch those views after loading.

The simplest usage is to include the generated source in your build and wrap the header in
your own assets file:

// MyAssets.h
#include "GeneratedAssets.h"

// additional asset data (material properties, post process properties, etc.)

void LoadAll()
{
    // Load Assets
    convex_hull::Load();
    meshes::Load();
    textures::Load();
    // ...

    // Fetch AssetViews
    meshes::Acquire();
    textures::Acquire();
    // ...
}
)";
} // namespace nc::convert

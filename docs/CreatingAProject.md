# Creating a Project

NcEngine projects can be developed purely through source code or with the help of NcEditor. This document will walk through creating and building a simple project using the former method. The project will consist of a single scene with a camera, point light, and a cube that can be moved with the WASD keys.

## Setting up the project directory
-----------------------------------
Create a directory called 'example' in the repository directory with a few files:
* ExampleScene.h
* Main.cpp
* Config.ini
* CMakeLists.txt

## Creating a Scene
--------------------
Next we're going to create a scene. A scene's primary responsibility is to set up the initial state of a level/environment through the Load() member function. We'll load the assets for the box, add a few entities and components, and register a camera:

```cpp
/** ExampleScene.h */
#include "asset/Assets.h"
#include "Input.h"
#include "NcEngine.h"

/** Default assets from the nc/resources directory. */
const auto CubeMeshPath = std::string{"cube.nca"};
const auto DefaultMaterial = nc::Material
{
    .baseColor = "DefaultBaseColor.png",
    .normal    = "DefaultNormal.png",
    .roughness = "DefaultMetallic.png",
    .metallic  = "DefaultMetallic.png",
};

class ExampleScene : public nc::Scene
{
    public:
        void Load(nc::NcEngine* engine) override
        {
            /** Load box assets. */
            nc::LoadMeshAsset(CubeMeshPath);
            nc::LoadTextureAssets({DefaultMaterial.baseColor,
                                   DefaultMaterial.normal,
                                   DefaultMaterial.roughness,
                                   DefaultMaterial.metallic});

            auto registry = engine->Registry();

            /** Initial properties for the Camera. */
            auto cameraInit = nc::EntityInfo
            {
                .position = nc::Vector3{0.0f, 3.0f, -10.0f},
                .rotation = nc::Quaternion::FromEulerAngles(0.0f, 0.0f, 0.785f),
                .tag = "Camera"
            };

            /** Create and register the Camera. */
            auto cameraHandle = registry->Add<nc::Entity>(cameraInit);
            auto camera = registry->Add<nc::Camera>(cameraHandle);
            engine->Graphics()->SetCamera(camera);

            /** Initial properties for the PointLight. */
            auto pointLightInit = nc::EntityInfo
            {
                .position = nc::Vector3::Up(),
                .tag = "Point Light"
            };

            /** Create the PointLight. */
            auto pointLightHandle = registry->Add<nc::Entity>(pointLightInit);
            registry->Add<nc::PointLight>(pointLightHandle, nc::PointLightInfo{});

            /** Create the cube using the previously loaded assets. */
            auto cubeInit = nc::EntityInfo{.tag = "Cube"};
            auto cubeHandle = registry->Add<nc::Entity>(cubeInit);
            registry->Add<nc::MeshRenderer>(cubeHandle, CubeMeshPath, DefaultMaterial, nc::TechniqueType::PhongAndUi);

            /** Add logic to the box for detecting input and handling movement. */
            registry->Add<FrameLogic>(cubeHandle, [](Entity self, Registry* registry, float dt)
            {
                constexpr float Speed = 5.0f;

                /** Get the state of the WASD keys as floats in the range [-1, 1] and scale them. */
                auto [xAxis, yAxis] = nc::input::GetAxis() * Speed * dt;
                
                /** Get the Transform of the Entity we're attached to. */
                auto transform = registry->Get<nc::Transform>(self);

                /** Move the Transform. */
                transform->Translate(nc::Vector3{xAxis, 0.0f, yAxis});
            });
        }

        /** Nothing to be done on Unload. */
        void Unload() override {}
};
```

## Entry Point
----------------------
The main file will be pretty simple:
```cpp
/** Main.cpp */
#include "NcEngine.h"
#include "ExampleScene.h"

int main()
{
    /** Create the engine instance. */
    auto engine = nc::InitializeNcEngine("example/Config.ini");

    /** Start the game loop. */
    engine->Start(std::make_unique<ExampleScene>());

    /** Destroy the engine instance. */
    engine->Shutdown();

    return 0;
}
```

NcEngine is fully initialized after construction until Shutdown is called, and any functions may be called even if the game loop isn't running. For instance, the assets for the box could have been loaded before Start() instead of on scene load.

One thing that isn't obvious in this example is how control is given back to WinMain. NcEngine::Stop() needs to be called to exit the game loop, however, NcEngine will internally call Quit() upon receiving a WM_CLOSE message.

## Setting up the config file
-------------------------
For the config file, copy the defaults from [sdk/engine/source/config/default_config.ini](../sdk/engine/source/config/default_config.ini). The default asset paths are specified relative to the repository directory. If you have made your project directory somewhere else, these will need to be updated.

## Building
------------
The CMakeLists.txt needs to be written before we can build. Set the find_package search path according to your install location.
```cmake
cmake_minimum_required(VERSION 3.10)
project("Example" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
find_package(NcEngine REQUIRED PATHS "my-install-prefix/NcSdk-x.y.z/NcEngine")
add_executable(Example ${PROJECT_SOURCE_DIR}/Main.cpp)
target_link_libraries(Example PRIVATE Nc::NcEngine-dev)
```

Build and install the sdk, if you haven't already:
```
cmake -S ./ -B build -DCMAKE_INSTALL_PREFIX=my-install-prefix
cmake --build build --target install --config Release
```

Then build the example:
```
>mkdir build/example_project
>cmake -B build/example_project -S example
>cmake --build build/example_project --config Release
```

That's it! You should be able to run Example.exe and move the cube around. Pressing ` will toggle the editor, where you can tinker with the camera or point light properties. The editor has a pretty limited set of features, but there is an improved standalone version coming soon.

For ideas on where to go from here, check out the [sample project](../project/source). It has examples showing how to use audio, colliders, physics, joints, transform hierarchies, and more.
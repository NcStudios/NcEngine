# Creating a Project

This document will walk through creating and building a simple project using NcEngine. The project will consist of a single scene with a camera, point light, and a box that can be moved with the WASD keys. If anything is unclear, the [overview](Overview.md) or [engine components](EngineComponents.md) pages might offer clarification.

## Setting up the project directory
-----------------------------------
Create a directory called 'example' in the repository directory with a few files:
* Controller.h
* ExampleScene.h
* Main.cpp
* Config.ini
* CMakeLists.txt

## Creating a Component
-----------------------
Controller.h will define a component that handles movement of the box:
```cpp
/** Controller.h */
#include "ecs/Registry.h"
#include "Input.h"

/** Deriving from AutoComponent allows us to run logic each frame with FrameUpdate. */
class Controller : public nc::AutoComponent
{
    constexpr static auto Speed = 5.0f;
    nc::Registry* m_registry;

    public:
        Controller(nc::Entity entity, nc::Registry* registry)
            : nc::AutoComponent{entity},
              m_registry{registry}
        {
        }

        void FrameUpdate(float dt) override
        {
            /** Get the state of the WASD keys as floats in the range [-1, 1] and scale them. */
            auto [xAxis, yAxis] = nc::input::GetAxis() * Speed * dt;
            
            /** Get the Transform of the Entity we're attached to. */
            auto transform = m_registry->Get<nc::Transform>(ParentEntity());

            /** Move the Transform. */
            transform->Translate(nc::Vector3{xAxis, 0.0f, yAxis});
        }
};
```

## Creating a Scene
--------------------
Next we're going to create a scene. A scene's primary responsibility is to set up the initial state of a level/environment through the Load() member function. We'll load the assets for the box, add a few entities and components, and register a camera:

```cpp
/** ExampleScene.h */
#include "Assets.h"
#include "NcEngine.h"
#include "Controller.h"

/** Default assets from the nc/resources directory. */
const auto CubeMeshPath = std::string{"cube.nca"};
const auto DefaultMaterial = nc::Material
{
    .baseColor = "DefaultBaseColor.png",
    .normal = "DefaultNormal.png",
    .roughness = "DefaultMetallic.png",
    .metallic = "DefaultMetallic.png",
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

            /** Create and register a camera. */
            auto cameraInit = nc::EntityInfo
            {
                .position = nc::Vector3{0.0f, 3.0f, -10.0f},
                .rotation = nc::Quaternion::FromEulerAngles(0.0f, 0.0f, 45.0f),
                .tag = "Camera"
            };

            auto cameraHandle = registry->Add<nc::Entity>(cameraInit);
            auto camera = registry->Add<nc::Camera>(cameraHandle);
            engine->MainCamera()->Set(camera);

            /** Add a PointLight. */
            auto pointLightInit = nc::EntityInfo
            {
                .position = nc::Vector3::Up(),
                .tag = "Point Light"
            };

            auto pointLightHandle = registry->Add<nc::Entity>(pointLightInit);
            registry->Add<nc::PointLight>(pointLightHandle, nc::PointLightInfo{});

            /** Add the box. */
            auto cubeHandle = registry->Add<nc::Entity>(nc::EntityInfo{.tag = "Box"});
            registry->Add<nc::MeshRenderer>(cubeHandle, CubeMeshPath, DefaultMaterial, nc::TechniqueType::PhongAndUi);

            /** Add the movement controller to the cube. */
            registry->Add<Controller>(cubeHandle, registry);
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
#include "platform/win32/NcWin32.h"
#include "ExampleScene.h"

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    /** Create the engine instance. */
    auto engine = nc::InitializeEngine(instance, "example/Config.ini");

    /** Start the game loop. */
    engine->Start(std::make_unique<ExampleScene>());

    /** Destroy the engine instance. */
    engine->Shutdown();

    return 0;
}
```

NcEngine is fully initialized after construction until Shutdown is called, and any functions may be called even if the game loop isn't running. For instance, the assets for the box could have been loaded before Start() instead of on scene load.

One thing that isn't obvious in this example is how control is given back to WinMain. The Quit() function (Core.h) needs to be called to exit the game loop, however, NcEngine will internally call Quit() upon receiving a WM_CLOSE message.

## Setting up the config file
-------------------------
For the config file, start by copying the defaults from [nc/source/config/default_config.ini](../nc/source/config/default_config.ini). Most of these values will be sufficient, but writing shaders is outside the scope of this guide, so we'll just point to the ones in the sample project:

```
shaders_path=project/shaders/Compiled/
```

## Building
------------
The CMakeLists.txt needs to be written before we can build:
```cmake
cmake_minimum_required(VERSION 3.10)
project("Example" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(ExampleGame "Example")
set(NCENGINE_REPOSITORY_DIRECTORY ${PROJECT_SOURCE_DIR}/../)
set(RTAUDIO_LINK_FLAGS "-lole32 -lwinmm -lksuser -lmfplat -lmfuuid -lwmcodecdspuuid")

find_package(Vulkan REQUIRED)

add_executable(${ExampleGame} ${PROJECT_SOURCE_DIR}/Main.cpp)

add_definitions(-DNC_EDITOR_ENABLED)

set_target_properties(${ExampleGame}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${NCENGINE_REPOSITORY_DIRECTORY}
)

target_include_directories(${ExampleGame}
    PRIVATE
        ${NCENGINE_REPOSITORY_DIRECTORY}/nc/include
        ${NCENGINE_REPOSITORY_DIRECTORY}/nc/external/include
)

target_link_libraries(${ExampleGame}
    PRIVATE
        ${NCENGINE_REPOSITORY_DIRECTORY}/nc/lib/libNcEngine-ReleaseWithEditor.a
        ${NCENGINE_REPOSITORY_DIRECTORY}/nc/lib/libimgui-Release.a
        ${RTAUDIO_LINK_FLAGS}
        Vulkan::Vulkan
)
```

Because we're defining NC_EDITOR_ENABLED in the CMake, we'll need to build the engine with the editor enabled:
```
>tools/cmake Engine Release-WithEditor
>ninja -C build/Engine/Release-WithEditor
```

Then build the example:
```
>mkdir build/example_project
>cmake -G Ninja -B build/example_project -S example
>ninja -C build/example_project
```

That's it! You should be able to run Example.exe and move the cube around. Pressing ` will toggle the editor, where you can tinker with the camera or point light properties. The editor has a pretty limited set of features, but there is an improved standalone version coming soon.

For ideas on where to go from here, check out the [sample project](../project/source). It has examples showing how to use audio, colliders, physics, joints, transform hierarchies, and more.
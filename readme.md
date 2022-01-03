# NcEngine

<p align="center">
  <img src="docs/Logo.png" />
</p>

NcEngine is a 3D game engine written in C++20 targeting Windows. It is actively being developed but is still in very early stages. Features are in varying stages of completeness.

-----------
Getting started:
* [Requirements](#requirements)
* [Cloning](#cloning)
* [Targets](#targets)
* [Building](#building)
    * [Command Line](#command-line)
    * [Visual Studio](#visual-studio)
* [Configuration and Presets](#configuration)
* [Definitions](#definitions)

[1]: <#command line>

More information:
* [Overview](docs/Overview.md)
* [Creating a Project](docs/CreatingAProject.md)
* [NcEngine Components](docs/EngineComponents.md)


### Requirements:
----------------
* Windows
* Vulkan SDK
* CMake
* Compiler with c++20 support:
    * Visual Studio 17 2022
    * MinGW-w64 9.0.0

### Cloning:
------------
When cloning, make sure submodules are cloned properly:

```
git clone --recurse-submodules https://github.com/McCallisterRomer/NCEngine
```

If any subdirectories of nc/external are empty, the repository was cloned non-recursively. You can get the dependencies with:
```
git submodule update --init
```

### Targets
----------
There are three primary targets in the repository:
* NcEngine: Core Library
    * Type: Static Library
    * Directory: nc
* NcEditor: Application for project creation and management.
    * Type: Executable
    * Directory: editor
    * Dependencies: NcEngine [with -DNC_EDITOR_ENABLED=ON]
* Sample: Application containing demo, test, and benchmark scenes.
    * Type: Executable
    * Directory: project
    * Dependencies: NcEngine

NcEngine is the library games link against. NcEditor and the sample application are optional and depend on NcEngine being build *and* installed.

### Building
------------
### Command Line
Each target can be configured and built with:
```
>cmake -S <target-directory> -B build/<Preset-Name> --preset <Preset-Name>
>cmake --build build/<Preset-Name>
```

For NcEngine, install as well:
```
>cmake --install build/<Preset-Name>
```

Example building everything using Ninja and the 'Release-WithEditor' preset:
```
>cmake -S nc -B build/NcEngine-Ninja-Release-WithEditor --preset NcEngine-Ninja-Release-WithEditor
>cmake --build build/NcEngine-Ninja-Release-WithEditor
>cmake --install build/NcEngine-Ninja-Release-WithEditor

>cmake -S editor -B build/NcEditor-Ninja-Release-WithEditor --preset NcEditor-Ninja-Release-WithEditor
>cmake --build build/Editor-Ninja-Release-WithEditor

>cmake -S project -B build/Sample-Ninja-Release-WithEditor --preset Sample-Ninja-Release-WithEditor
>cmake --build build/Sample-Ninja-Release-WithEditor
```

### Visual Studio
Once cloned, open the repository in a Visual Studio instance. Visual Studio should detect the CMakePresets.json files and display dropdowns for Target System/Configuration/Build Preset:

<p align="center">
  <img src="docs/visual_studio_control.png" />
</p>

CMake presets may need to be enabled: Tools > Options > CMake > Use CMakePresets.json to drive CMake configure, build and test.

Select the desired NcEngine-MSVC option from the 'Configuration' dropdown. You may need to first select 'Manage Configurations... (nc/CMakeLists.txt). The 'Build Preset' dropdown should update to have the same name as the selected configuration. Upon completion of the configure step, proceed to build and install.

Once NcEngien is installed, NcEditor or the sample application may be built in the same way.

### Configuration and Presets
----------------------------
Each target's root directory contains a CMakePresets.json file with common configuration and build presets for both Visual Studio and Ninja + MinGW. The available presets can be viewed with `cmake -S <target-source-dir> --list-presets`.

Each of these presets follows the following naming scheme `<target>-<generator>-<configuration>-[modifier]`, where modifier describes extra features to include:
* WithEditor: Include editor-specific code.
* WithProfiling: Include Optick profiling code.
* WithValidation: Enable Vulkan validation layers.

Each preset provides a different set of [definitions](#definitions). When adding custom presets to CMakeUserPresets.json, both NC_BUILD_TYPE and NC_INSTALL_DIR must be defined.

More on CMake Presets:
* [Documentation](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)
* [CMake Presets in Visual Studio](https://docs.microsoft.com/en-us/cpp/build/cmake-presets-vs?view=msvc-170)

### Definitions
--------------
#### NC_BUILD_TYPE
    Default = N/A
    A unique name identifying the selected build preset. Used for differentiating builds/directories.

#### NC_INSTALL_DIR
    Default = NcEngine/install
    The directory NcEngine will be installed to.

#### NC_TESTS_ENABLED
    Default = OFF
    Flag used by cmake to enable building test executables.

#### NC_EDITOR_ENABLED
    Default = OFF
    Flag used by both CMake and NcEngine specifying whether to include the debug editor in the final executable. Some blocks of coded required only by the editor are wrapped in #ifdefs. If this value is set to ON, it must be passed when building both the engine library and your project.

#### NC_USE_VALIDATION
    Default = OFF
    Enables Vulkan validation layers.

#### NC_DEBUG_RENDERING
    Default = OFF
    Allows rendering wireframe primitives for debugging purposes.

#### NC_PROFILING_ENABLED
    Default = OFF
    Enabled profiling with Optick. This also requires the Optick application (https://www.optickprofiler.com) and the Optick
    shared library (nc/lib/libOptick.dll).

#### VERBOSE_LOGGING_ENABLED
    Default = OFF
    Flag used to enable extra logging of internal engine operations to the diagnostics file specified in config.ini.


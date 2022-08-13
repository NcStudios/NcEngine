# NcEngine
[![Tests](https://github.com/NcStudios/NCEngine/actions/workflows/build.yml/badge.svg)](https://github.com/NcStudios/NCEngine/actions?query=workflow%3ATests)
[![License](https://img.shields.io/github/license/McCallisterRomer/NCEngine.svg)](https://github.com/McCallisterRomer/NCEngine/blob/vnext/LICENSE)

<p align="center">
  <img src="docs/Logo.png" />
</p>

NcEngine is a 3D game engine written in C++20 targeting Windows. It is actively being developed but is still in very early stages. Features are in varying stages of completeness.

-------------------
* Getting Started
    * [Requirements](#requirements)
    * [Building](#building)
    * [Definitions](#definitions)
* More Information
    * [Documentation](https://ncstudios.github.io/NcEngine)
    * [Creating a Project](docs/CreatingAProject.md)
    * [NcEngine Components](docs/EngineComponents.md)

### Requirements
----------------
* Windows
* Vulkan SDK
* CMake 3.20
* Compiler with c++20 support:
    * Visual Studio 17 2022
    * MinGW-w64 9.0.0

## Building
---
Run CMake on the root CMakeLists with your required options. It is easiest to provide an install path and build the install target:
```
cmake -S ./ -B build -DCMAKE_INSTALL_PREFIX=NcSdk
cmake --build build --target install --config Release
```

There are also files with build tasks for Visual Studio and Visual Studio Code located in `sdk/tools/`.

### Installation Items
* NcEngine: Engine libraries and headers.
  * To include in a CMake project use: `find_package(NcEngine PATHS install-path)` and link against `Nc::NcEngine-dev`.
* NcEditor: Application for project creation and management.
  * Application and utilities are installed to `install-path/bin`.
* Sample: Application containing demo, test, and benchmark scenes.
  * Installed to `install-path/sample`.

### Building a Production Library
While the default `Nc::Engine-dev` target can be built with a Release configuration, it still includes extra code for profiling and inspection by NcEditor that is otherwise not needed by the project itself. This can be excluded by defining `NC_PROD_BUILD=ON` during the CMake configure step. The target will instead be exported as `Nc::Engine`, and additional items, like NcEditor and the sample, will not be built. Unique build and install directories should be used when enabling and disabling this option.

### Definitions
---------------
#### NC_BUILD_TESTS
    Default: OFF
    Include tests when building the NcSdk project.

#### NC_PROD_BUILD
    Default: OFF
    Build binaries with debug, utility, and profiling code stripped. Only NcEngine is built, and its target name is changed to 'Nc::NcEngine' (dropping the '-dev' suffix). NcEditor uses this for project release builds.

#### NC_EDITOR_ENABLED
    Default: ON (OFF when NC_PROD_BUILD=ON)
    Flag used by both CMake and NcEngine specifying whether to include the debug editor in the final executable. Some blocks of coded required only by the editor are wrapped in #ifdefs. If this value is set to ON, it must be passed when building both the engine library and your project.

#### NC_ASSERT_ENABLED
    Default: ON (OFF when NC_PROD_BUILD=ON)
    Enables extra checks through the NC_ASSERT macro. These are enabled by default, even in release builds, as they are unlikely to have a serious effect on performance.

#### NC_DEBUG_RENDERING_ENABLED
    Default: ON (OFF when NC_PROD_BUILD=ON)
    Allows rendering wireframe primitives for debugging purposes.

#### NC_PROFILING_ENABLED
    Default: ON (OFF when NC_PROD_BUILD=ON)
    Enabled profiling with Optick. This also requires the Optick application (https://www.optickprofiler.com) and the Optick
    shared library (nc/lib/libOptick.dll).

#### NC_VERBOSE_LOGGING_ENABLED
    Default: ON (OFF when NC_PROD_BUILD=ON)
    Flag used to enable extra logging of internal engine operations to the diagnostics file specified in config.ini.

## More Information
-------------------
To learn more:
* [Documentation](https://ncstudios.github.io/NcEngine)
* [Tutorial: Creating a Project](docs/CreatingAProject.md)
* [NcEngine Components](docs/EngineComponents.md)
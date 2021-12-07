# NcEngine

<p align="center">
  <img src="docs/Logo.png" />
</p>

NcEngine is a 3D game engine written in C++20 targeting Windows. It is actively being developed but is still in very early stages. Features are in varying stages of completeness.

## Links
-----------
* [Building](#building)
    * [Configurations](#configurations)
    * [Definitions](#definitions)
* [Overview](docs/Overview.md)
* [Creating a Project](docs/CreatingAProject.md)
* [NcEngine Components](docs/EngineComponents.md)


## Building
-----------
Current limitations:
* NcEngine depends on Windows. There will likely be cross-platform support in the future, but it is not actively being pursued.
* There are a few places where gcc specific code hasn't been fixed. This unfortunately means that mingw is the only currently supported compiler. This is planned to be fixed in the near future.

There are three CMake projects in the repository
* The engine, located in the 'nc' directory, is built as a static library and is output to 'nc/lib'.
* An editor, located in the 'editor' directory, is output to the repository directory as 'NcEditor.exe'.
* A sample project, located in the 'project' directory, is output to the repository directory as 'Sample.exe'.

The easiest way to build any of these is by running 'build.bat' in the 'tools' directory. It requires
* Target: Engine, Editor, or Project
* Config: Common configurations [here](#configurations)
* Install Directory: Where to install NcEngine headers and binaries
* Generator: CMake Generator

For example, building everything in Release-WithEditor mode from the repository root directory using Ninja looks like:

```
>tools/build.bat Engine Release-WithEditor C:/your/install/path "Ninja"
>tools/build.bat Editor Release-WithEditor C:/your/install/path "Ninja"
>tools/build.bat Project Release-WithEditor C:/your/install/path "Ninja"
```

For Vs Code users, tasks for common cases can be found here [tools/tasks.json](tools/tasks.json).

### Configurations
-----------------
When building with the tools/build.bat file, one of these configurations are usually sufficient:
* Release - standard release build
* Release-WithEditor - Release with editor-specific code included. Link the editor against this.
* Debug - add debug symbols, editor-specific code, extra logging, build tests

There are other available options. Note that these options just set the CMAKE_BUILD_TYPE and various definitions for convenience. CMake can always be run manually to add whatever definitions are needed. Note that NcEngine is installed to \<your-instalation-directory>/NcEngine/NC_BUILD_TYPE, so custom build types should set NC_BUILD_TYPE to a unique name.

### Definitions
--------------
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
    Default = true
    Flag used to enable extra logging of internal engine operations to the diagnostics file specified in config.ini.


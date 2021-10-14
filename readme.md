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

There are two CMake projects in the repository
* The engine, located in the 'nc' directory, is built as a static library and is output to 'nc/lib'.
* A sample project, located in the 'project' directory, is output to the repository directory as 'Sample.exe'.

The easiest way to build the either of these is by running 'cmake.bat' in the 'tools' directory. It requires a target and configuration, where the target is either 'Engine' or 'Sample', and the configuration is one of the options listed [below](#configurations). This will output build files into the directory 'build/target/configuration' that can then be provided to the build system.

For example, building the engine and sample in release mode from the repository root directory using Ninja looks like:

Generate engine build files:
```
>tools/cmake.bat Engine Release
```

Build NcEngine library
```
>ninja -C build/Engine/Release
```

Generate sample build files:
```
>tools/cmake.bat Project Release
```

Build Sample.exe
```
>ninja -C build/Project/Release
```

By default the cmake.bat file specifies Ninja as the generator. To use a different generator just change "Ninja" in the last line of cmake.bat to the generator of your choice.

### Configurations
-----------------
When building with the tools/cmake.bat file, one of these configurations are usually sufficient:
* Release - standard release build
* Release-WithEditor - Release with editor-specific code included
* Debug - add debug symbols, editor-specific code, extra logging, build tests

There are other available options. Note that these options just set the CMAKE_BUILD_TYPE and various definitions for convenience. CMake can always be run manually to add whatever definitions are needed.

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

#### VERBOSE_LOGGING_ENABLED
    Default = true
    Flag used to enable extra logging of internal engine operations to the diagnostics file specified in config.ini.
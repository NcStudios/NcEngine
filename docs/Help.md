NcEngine Help
=============

## Contents
* [Building](#building)
* [Macros](#macros)
* [Directory Structure](#directory-structure)
* [Using Config Files](#using-config-files)

## Building
---------------------------------
NcEngine uses CMake to generate build files. User created source files will automatically be included by the build system, assuming they are located in the expected project search directories. Some may find it easier to instead manually add source files. If this is desired, see the CMakeLists.txt in the project directory.

There are scripts located in the 'tools' directory for basic building. Assuming a freshly cloned repo, the steps to build are as follows:
1. Generate build files
    * \>tools/cmake.bat
2. Build executable
    * \>tools/make.bat

Considerations:
* The cmake.bat script will build libraries for external dependencies if they do not exist(e.g. the first time it is run). It will **not** rebuild them if changes are made to their source files. If rebuilding is necessary, either manually run tools/build_external_libs.bat or simply delete the libs from external/lib before running cmake.

## Macros
---------
#### USE_VULKAN
    Default = false
    Flag used by both CMake and NcEngine specifying whether to include and run Graphics2, which is the Vulkan implementation of the graphics API. If false, DirectX is used via Graphics.

#### NC_TESTS_ENABLED
    Default = false
    Flag used by cmake to enable building test executables.

#### NC_EDITOR_ENABLED
    Default = true
    Flag used by both CMake and NcEngine specifying whether to include the debug editor in the final executable. Setting this to false at the time CMake is run will exclude the imgui lib and any source files related to the debug editor. Anything relying on those files should therefore be wrapped in #ifdef checks on this value.

#### VERBOSE_LOGGING_ENABLED
    Default = true
    Flag used to enable extra logging of internal engine operations to the diagnostics file specified in config.ini.

## Directory Structure
----------------------
#### /build
    CMake cache directory. If this directory does not exist, tools/cmake.bat will create it before generating build files.

#### /external
    Source directory for external dependencies. Once built, libraries for these can be found in external/lib.

#### /nc
    NcEngine source directory.

#### /project
    Parent directory for all user created files relating to a project. NcEngine searches for user components in project/components, models in project/models, and so on. Configuration of these search directories is not currently supported but is planned for the future.

#### /test
    Test source directory.

#### /tools
    Directory containing various utilities for working with and building NcEngine.

## Using Config Files
---------------------
Various settings can be controlled through the .ini files located in project/config. Modifications should conform to these rules:
* One key-value pair per line separated with '='.
* Non-data content appears either:
    * As a line comment after a ';' or '#'
    * As a human-readable section tag enclosed in square brackets
* Extraneous whitespace and blank lines should be avoided.

#### config.ini defaults
    [project]
    project_name=MyProjectName
    log_file_path=Diagnostics.log
    [user]
    user_name=Player
    [physics]
    fixed_update_interval=0.2
    [graphics]
    use_native_resolution=0
    launch_fullscreen=0
    screen_width=1200
    screen_height=1200
    target_fps=60
    near_clip=0.5
    far_clip=400.0
    shaders_path=project\shaders\compiled\
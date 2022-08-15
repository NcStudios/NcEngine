# Build script for the NcEngine SDK. Visual studio is used by default, but MinGW with the
# Ninja generator is also supported.
#
# Build artifacts will be placed under a subdirectory named after the generator. A
# directory suffix may be used to separate build variants (dev vs prod).

param
(
    [Parameter()]
    [ValidateSet("msvc", "mingw")]
    [Alias("G")]
    $Generator = "msvc",

    [Parameter()]
    [Alias("S")]
    $SourceDir = "./",

    [Parameter()]
    [Alias("B")]
    $BuildDirectory = "build",

    [Parameter()]
    $BuildDirectorySuffix = "dev",

    [Parameter()]
    [Alias("I")]
    $InstallDirectory = "install",

    [Parameter()]
    [Alias("C")]
    $BuildType = "Release"
)

$FullBuildDirectory = "$BuildDirectory/$Generator-$BuildDirectorySuffix"

if ($Generator -eq "msvc")
{
    cmake -G "Visual Studio 17 2022" -S "$SourceDir" -B "$FullBuildDirectory" -DCMAKE_INSTALL_PREFIX="$InstallDirectory"
    cmake --build "$FullBuildDirectory" --target install --parallel 8 --config $BuildType
}
elseif ($Generator -eq "mingw")
{
    cmake -G "Ninja" -S "$SourceDir" -B "$FullBuildDirectory" -DCMAKE_INSTALL_PREFIX="$InstallDirectory" -DCMAKE_BUILD_TYPE="$BuildType"
    cmake --build "$FullBuildDirectory" --target install --parallel 8
}

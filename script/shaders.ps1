param
(
[Parameter()]
[string]$InputDir = "..\project\shaders",

[Parameter()]
[string]$OutputDir = "..\project\shaders\Compiled"
)


$GlslcExe = $Env:VULKAN_SDK + "\Bin\glslc.exe"

Get-ChildItem -Path $InputDir -Filter *.frag |
Foreach-Object {
    $fileName = $_.BaseName + "Fragment.spv"
    .$GlslcExe $InputDir\$_ -o $OutputDir\$fileName
}

Get-ChildItem -Path $InputDir -Filter *.vert |
Foreach-Object {
    $fileName = $_.BaseName + "Vertex.spv"
    .$GlslcExe $InputDir\$_ -o $OutputDir\$fileName
}

cmake --build "D:\Source\NcEngine\out\build\x64-Debug" --target install --parallel 8 --config Release
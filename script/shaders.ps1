param
(
[Parameter(Mandatory=$false)]
[string]$InputDir = "" + $(get-location).Path + "\resources\shaders",

[Parameter(Mandatory=$false)]
[string]$OutputDir = "" + $(get-location).Path + "\resources\shaders\compiled"
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
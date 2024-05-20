param
(
[Parameter(Mandatory=$false)]
[string]$InputDir = "" + $(get-location).Path + "\resources\shaders",

[Parameter(Mandatory=$false)]
[string]$OutputDir = "" + $(get-location).Path + "\resources\shaders\compiled",

[Parameter(Mandatory=$false)]
[boolean]$DebugShaders = $false
)

$GlslcExe = $Env:VULKAN_SDK + "\Bin\glslc.exe"
$debugArg = ""
if ($DebugShaders)
{
    $debugArg = "-g"
}

Get-ChildItem -Path $InputDir -Filter *.frag |
Foreach-Object {
    $fileName = $_.BaseName + "Fragment.spv"
    .$GlslcExe $InputDir\$_ -o $OutputDir\$fileName $debugArg
}

Get-ChildItem -Path $InputDir -Filter *.vert |
Foreach-Object {
    $fileName = $_.BaseName + "Vertex.spv"
    .$GlslcExe $InputDir\$_ -o $OutputDir\$fileName $debugArg
}
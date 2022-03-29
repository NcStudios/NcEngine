param([string]$TargetName, [string]$Generator, [string]$PresetModifier="")

function Run-Commands {
    param (
        [string]$SourceDirectory,
        [string]$PresetName
    )

    $configType = ""
    if ($PresetName.Contains("Debug")) {
       $configType = "Debug"
    }
    else {
       $configType = "Release"
    }

    cmake -S "$SourceDirectory/" -B "build/$PresetName" --preset "$PresetName"
    cmake --build "build/$PresetName" --config $configType
    if ($PresetName.Contains("NcEngine"))
    {
        Write-Host "Installing build/$PresetName"
        cmake --install "build/$PresetName"
    }
}

$sourceDirectory = ""
$presetName = ""
$requiresEngine = $false

$presetName = "$TargetName-$Generator-$PresetModifier"

if ($TargetName.Equals("NcEngine")) {
    $sourceDirectory = "engine/"
    $requiresEngine = $false
}
elseif ($TargetName.Equals("Sample")) {
    $sourceDirectory = "sample/"
    $requiresEngine = $true
}
elseif ($TargetName.Equals("NcEditor")) {
    $sourceDirectory = "editor/"
    $requiresEngine = $true
}
elseif ($TargetName.Equals("Tests")) {
    $sourceDirectory = "test/"
    $requiresEngine = $false
    $presetName = "$TargetName-$Generator"
}


if ($requiresEngine) {
    Run-Commands -SourceDirectory "engine/" -PresetName "NcEngine-$Generator-$PresetModifier"
}

Write-Host "PresetName: $presetName"
Write-Host "SourceDirectory: $sourceDirectory"
Run-Commands -SourceDirectory $sourceDirectory -PresetName $presetName 

Write-Host "Complete"
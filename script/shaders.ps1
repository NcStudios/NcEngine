# When compiling with DebugMode, shaders are output to a 'debug' subdirectory. 'shaders_path' in the config also needs
# to be updated to point to that subdirectory.

param
(
[Parameter(Mandatory=$false)]
[string]$InputDir = "" + $(get-location).Path + "\resources\shaders\source",

[Parameter(Mandatory=$false)]
[string]$OutputDir = "" + $(get-location).Path + "\resources\shaders\compiled",

[Parameter(Mandatory=$false)]
[bool]$DebugMode = $false
)

$Compiler = $Env:VULKAN_SDK + "\Bin\dxc.exe"
$Optimizer = $Env:VULKAN_SDK + "\Bin\spirv-opt.exe"

if ($DebugMode) {
    $CompilerOutputDir = $OutputDir
}
else {
    $CompilerOutputDir = Join-Path $OutputDir "temp"
}

$FailureCount = 0

if (-not (Test-Path -Path $CompilerOutputDir)) {
    New-Item -Path $CompilerOutputDir -ItemType Directory | Out-Null
}

Write-Host "--Compiling Shaders--"
Get-ChildItem -Path $InputDir | Where-Object { $_.Extension -eq '.psh' -or $_.Extension -eq '.vsh' } |
ForEach-Object {
    if ($_.Extension -eq '.psh') {
        $Profile = 'ps_6_6'
        $FileName = "$($_.BaseName)Pixel.spv"
    } elseif ($_.Extension -eq '.vsh') {
        $Profile = 'vs_6_6'
        $FileName = "$($_.BaseName)Vertex.spv"
    } else {
        return
    }

    Write-Output "Compiling $_"
    $InputFile = Join-Path $InputDir $_.Name
    $OutputFile = Join-Path $CompilerOutputDir $FileName
    $Args = @(
        '-spirv',                                         # generate SPIR-V for Vulkan
        '-fspv-target-env=vulkan1.2',                     # target Vulkan 1.2
        '-fspv-extension=SPV_GOOGLE_hlsl_functionality1', # SPIR-V HLSL semantics
        '-fspv-extension=SPV_KHR_non_semantic_info',      # SPIR-V HLSL semantics
        '-fspv-extension=SPV_EXT_descriptor_indexing',    # Allow for descriptor indexing
        '-Zpr',                                           # pack matrices in row-major order
        '-E', 'main',                                     # entry point
        '-T', $Profile,                                   # shader profile (vs_6_0, ps_6_0)
        '-Fo', $OutputFile,             # output file
        $InputFile
    )

    if ($DebugMode) {
        $Args = @('-Zi', '-fspv-debug=line') + $Args
    }

    & $Compiler @Args

    if ($LASTEXITCODE -eq 0) {
        Write-Host "COMPILATION SUCCEEDED: $_ -> $OutputFile" -ForegroundColor Green
    }
    else {
        Write-Host "COMPILATION FAILED: $_" -ForegroundColor Red
        $FailureCount++
    }
}

if (-not $DebugMode) {
    Write-Host "--Optimizing Shaders--"
    Get-ChildItem -Path $CompilerOutputDir -Filter *.spv |
    ForEach-Object {
        Write-Host "Optimizing $_"
        $InputFile = Join-Path $CompilerOutputDir $_
        $OutputFile = Join-Path $OutputDir $_
        $Args = @(
            '--legalize-hlsl', # optimizations to generate legal vulkan spir-v from hlsl input
            '-O',              # default optimizations
            $InputFile,        # input spirv
            '-o', $OutputFile  # output spirv
        )

        & $Optimizer $Args

        if ($LASTEXITCODE -eq 0) {
            Write-Host "OPTIMIZE SUCCEEDED: $_ -> $OutputFile" -ForegroundColor Green
        }
        else {
            Write-Host "OPTIMIZE FAILED: $_" -ForegroundColor Red
            $FailureCount++
        }
    }

    Remove-Item -Path $CompilerOutputDir -Recurse
}

if ($FailureCount -eq 0) {
    Write-Host "`nAll Succeeded" -ForegroundColor Green
}
else {
    Write-Host "`n$FailureCount Failures" -ForegroundColor Red
}

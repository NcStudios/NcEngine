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

$Compiler = $Env:VULKAN_SDK + "\Bin\glslangValidator.exe"
$Optimizer = $Env:VULKAN_SDK + "\Bin\spirv-opt.exe"
$Preamble = "#pragma pack_matrix(row_major)"

# if ($DebugMode) {
#     $CompilerOutputDir = Join-Path $OutputDir "debug"
#     if (-not (Test-Path -Path $OutputDir)) {
#         New-Item -Path $OutputDir -ItemType Directory | Out-Null
#     }
# }
# else {
    $CompilerOutputDir = $OutputDir
# }

$FailureCount = 0

if (-not (Test-Path -Path $CompilerOutputDir)) {
    New-Item -Path $CompilerOutputDir -ItemType Directory | Out-Null
}

Write-Host "--Compiling Shaders--"
Get-ChildItem -Path $InputDir | Where-Object { $_.Extension -eq '.psh' -or $_.Extension -eq '.vsh' } |
ForEach-Object {
    if ($_.Extension -eq '.psh') {
        $Stage = 'frag'
        $FileName = "$($_.BaseName)Pixel.spv"
    } elseif ($_.Extension -eq '.vsh') {
        $Stage = 'vert'
        $FileName = "$($_.BaseName)Vertex.spv"
    } else {
        return
    }

    Write-Output "Compiling $_"
    $InputFile = Join-Path $InputDir $_.Name
    $OutputFile = Join-Path $CompilerOutputDir $FileName
    $Args = @(
        '-V',                    # create SPIRV binary w/ Vulkan semantics
        '-D',                    # input is HLSL
        "-P$Preamble",           # prepended our definitions to each source file
        '-e', 'main',            # entry point
        '-fhlsl_functionality1', # enable functionality1 extension
        "-S", $Stage,            # explicitly specify shader stage
        "-o", $OutputFile,
        $InputFile
    )

    if ($DebugMode) {
        $Args = @('-g') + $Args
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

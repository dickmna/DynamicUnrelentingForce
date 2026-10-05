param(
    [ValidateSet('Release', 'RelWithDebInfo', 'Debug')][string]$Configuration = 'Release',
    [string]$CMakePrefixPath = '',
    [switch]$Addon
)
$ErrorActionPreference = 'Stop'
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$sourcePath = if ($Addon) { Join-Path $repoRoot 'addons/ShoutProgressionPushSubmod' } else { $repoRoot }
$buildPath = Join-Path $repoRoot $(if ($Addon) { 'build/addon' } else { 'build/vs2022-release' })
$prefixPath = Join-Path $repoRoot 'build/commonlib-installed'
if ($CMakePrefixPath) { $prefixPath += ';' + $CMakePrefixPath }
cmake -S $sourcePath -B $buildPath -G 'Visual Studio 17 2022' -A x64 "-DCMAKE_PREFIX_PATH=$prefixPath"
if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed.' }
cmake --build $buildPath --config $Configuration
if ($LASTEXITCODE -ne 0) { throw 'CMake build failed.' }

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.IO.Compression.FileSystem
function Copy-ReleaseFile {
    param([string]$Source, [string]$Destination)
    if (-not (Test-Path -LiteralPath $Source -PathType Leaf)) { throw "Required file is missing: $Source" }
    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $Destination) | Out-Null
    Copy-Item -LiteralPath $Source -Destination $Destination
}
function Copy-LicenseNotices {
    param([string]$RepoRoot, [string]$DocumentationPath)
    foreach ($fileName in @('LICENSE', 'CommonLib-COPYING.txt', 'CommonLib-EXCEPTIONS.md', 'THIRD_PARTY.md')) {
        Copy-ReleaseFile (Join-Path $RepoRoot $fileName) (Join-Path $DocumentationPath $fileName)
    }
    $dependencyNotices = @{
        'fmt-12.1.0/LICENSE' = 'fmt-LICENSE.txt'
        'spdlog-1.16.0/LICENSE' = 'spdlog-LICENSE.txt'
        'rapidcsv-8.90/LICENSE' = 'rapidcsv-LICENSE.txt'
        'DirectXMath-2025-04-03/LICENSE' = 'DirectXMath-LICENSE.txt'
        'DirectXTK-2025-10-27/LICENSE' = 'DirectXTK-LICENSE.txt'
        'MinHook-hde64-1.3.4/LICENSE.txt' = 'MinHook-LICENSE.txt'
    }
    foreach ($entry in $dependencyNotices.GetEnumerator()) {
        Copy-ReleaseFile (Join-Path $RepoRoot ('dependencies/' + $entry.Key)) (Join-Path $DocumentationPath $entry.Value)
    }
}
function Write-SourceLink {
    param([string]$RepoRoot, [string]$RepositoryName, [string]$SourceRef, [string]$DocumentationPath)
    if (-not $SourceRef) {
        $SourceRef = (git -C $RepoRoot rev-parse HEAD).Trim()
        if ($LASTEXITCODE -ne 0) { throw 'Could not identify the corresponding source commit.' }
    }
    if ($SourceRef -notmatch '^[a-fA-F0-9]{40}$') { throw 'SourceRef must be a full Git commit SHA.' }
    $sourceLink = "Corresponding source and build instructions: https://github.com/dickmna/$RepositoryName/tree/$SourceRef"
    Set-Content -LiteralPath (Join-Path $DocumentationPath 'SOURCE.txt') -Value $sourceLink -Encoding UTF8
}
function Complete-PlayerArchive {
    param([string]$StagePath, [string]$ArchivePath, [bool]$Fomod)
    $files = @(Get-ChildItem -LiteralPath $StagePath -Recurse -File)
    $forbidden = @($files | Where-Object {
        $_.Extension -match '(?i)^\.(cpp|c|h|hpp|psc|pdb|obj|lib|exp|exe|zip|7z|rar)$' -or
        $_.FullName -match '(?i)[\\/](Source|ThirdPartySource|dependencies|CorrespondingSource)[\\/]'
    })
    if ($forbidden.Count) { throw 'The player archive contains source, debug, build, or nested archive files.' }
    $moduleConfigs = @($files | Where-Object {
        $_.Name -like '*ModuleConfig.xml*' -and (Split-Path -Leaf $_.DirectoryName) -like '*fomod*'
    })
    if ($Fomod -and ($moduleConfigs.Count -ne 1 -or $moduleConfigs[0].FullName -ne (Join-Path $StagePath 'fomod/ModuleConfig.xml'))) {
        throw 'A player FOMOD must contain exactly one root installer configuration.'
    }
    if (-not $Fomod -and $moduleConfigs.Count) { throw 'This player archive should not be detected as a FOMOD.' }
    if (Test-Path -LiteralPath $ArchivePath) { throw "Output already exists: $ArchivePath" }
    [IO.Compression.ZipFile]::CreateFromDirectory($StagePath, $ArchivePath)
    $checksum = (Get-FileHash -Algorithm SHA256 -LiteralPath $ArchivePath).Hash.ToLowerInvariant()
    Set-Content -LiteralPath ($ArchivePath + '.sha256') -Value ($checksum + '  ' + [IO.Path]::GetFileName($ArchivePath)) -Encoding ASCII
    Write-Output $ArchivePath
}
function Remove-PlayerStage {
    param([string]$StagePath, [string]$OutputDirectory)
    $stageAbsolute = [IO.Path]::GetFullPath($StagePath)
    $outputAbsolute = [IO.Path]::GetFullPath($OutputDirectory).TrimEnd([IO.Path]::DirectorySeparatorChar) + [IO.Path]::DirectorySeparatorChar
    if (-not $stageAbsolute.StartsWith($outputAbsolute, [StringComparison]::OrdinalIgnoreCase) -or [IO.Path]::GetFileName($stageAbsolute) -notlike '.runtime-stage-*') {
        throw 'Refusing to clean a staging directory outside the output directory.'
    }
    Remove-Item -LiteralPath $stageAbsolute -Recurse -Force
}

param(
    [string]$StandaloneDataDirectory = '',
    [string]$AddonDataDirectory = '',
    [string]$OutputDirectory = '',
    [string]$SourceRef = ''
)
. (Join-Path $PSScriptRoot 'runtime-packaging.ps1')
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
if (-not $StandaloneDataDirectory) { $StandaloneDataDirectory = Join-Path $repoRoot 'build/vs2022-release/Data' }
if (-not $AddonDataDirectory) { $AddonDataDirectory = Join-Path $repoRoot 'build/addon/Data' }
if (-not $OutputDirectory) { $OutputDirectory = Join-Path $repoRoot 'dist' }
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null
$stagePath = Join-Path $OutputDirectory ('.runtime-stage-' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $stagePath | Out-Null
try {
    foreach ($fileName in @('ModuleConfig.xml', 'info.xml')) {
        $templatePath = Join-Path $repoRoot ('installer/fomod/' + $fileName)
        if ((Get-Content -LiteralPath $templatePath -Raw -Encoding UTF8) -notmatch '^<\?xml version="1.0" encoding="UTF-8"\?>') {
            throw 'FOMOD XML declarations must use double quotes and UTF-8 for the native installer.'
        }
        Copy-ReleaseFile (Join-Path $repoRoot ('installer/fomod/' + $fileName)) (Join-Path $stagePath ('fomod/' + $fileName))
    }
    $variants = @(
        @{ Name = 'Standalone'; Plugin = 'DynamicUnrelentingForce'; Data = $StandaloneDataDirectory; Package = (Join-Path $repoRoot 'package') },
        @{ Name = 'ShoutProgressionAddon'; Plugin = 'ShoutProgressionPushSubmod'; Data = $AddonDataDirectory; Package = (Join-Path $repoRoot 'addons/ShoutProgressionPushSubmod/package') }
    )
    foreach ($variant in $variants) {
        $destination = Join-Path $stagePath ('options/' + $variant.Name)
        Copy-ReleaseFile (Join-Path $variant.Data ('SKSE/Plugins/' + $variant.Plugin + '.dll')) (Join-Path $destination ('SKSE/Plugins/' + $variant.Plugin + '.dll'))
        Copy-ReleaseFile (Join-Path $variant.Package ('SKSE/Plugins/' + $variant.Plugin + '.ini')) (Join-Path $destination ('SKSE/Plugins/' + $variant.Plugin + '.ini'))
        foreach ($scriptName in @($variant.Plugin, 'VoicePushEffectScript')) {
            Copy-ReleaseFile (Join-Path $variant.Data ('Scripts/' + $scriptName + '.pex')) (Join-Path $destination ('Scripts/' + $scriptName + '.pex'))
        }
    }
    $documentationPath = Join-Path $stagePath 'Docs/DynamicUnrelentingForce'
    Copy-LicenseNotices $repoRoot $documentationPath
    Write-SourceLink $repoRoot 'DynamicUnrelentingForce' $SourceRef $documentationPath
    Set-Content -LiteralPath (Join-Path $documentationPath 'README.txt') -Value 'For Skyrim Steam 1.7.104 / SKSE64 2.3.1 only. Skyrim 1.6.1170 users should keep 1.1.0. Choose the standalone plugin or the Shout Progression addon; install only one.' -Encoding UTF8
    Complete-PlayerArchive $stagePath (Join-Path $OutputDirectory 'DynamicUnrelentingForce-1.1.1-FOMOD.zip') $true
}
finally {
    Remove-PlayerStage $stagePath $OutputDirectory
}

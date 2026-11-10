# Run on the Windows PC used for the actual Arduino/OpenMV build.
# Does not guess installed dependency versions or claim a successful compile.
param([string]$FQBN = 'arduino:esp32:nano_nora')
$ErrorActionPreference = 'Continue'
$root = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$out = Join-Path $root 'docs\validation\evidence\local-build'
New-Item -ItemType Directory -Force -Path $out | Out-Null
Get-Date -Format o | Out-File (Join-Path $out 'capture-date.txt')
try { git rev-parse HEAD | Out-File (Join-Path $out 'git-commit.txt') } catch { 'Git commit unavailable' | Out-File (Join-Path $out 'git-commit.txt') }
try { arduino-cli version | Out-File (Join-Path $out 'arduino-cli-version.txt') } catch { 'Arduino CLI not installed' | Out-File (Join-Path $out 'arduino-cli-version.txt') }
try { arduino-cli core list | Out-File (Join-Path $out 'board-platforms.txt') } catch { 'Run Boards Manager and transcribe the installed board platform' | Out-File (Join-Path $out 'board-platforms.txt') }
try { arduino-cli lib list | Out-File (Join-Path $out 'installed-libraries.txt') } catch { 'Run Library Manager and transcribe author/version' | Out-File (Join-Path $out 'installed-libraries.txt') }
try {
  arduino-cli compile --fqbn $FQBN (Join-Path $root 'src\HLR_FE_Separated') --warnings all *> (Join-Path $out 'arduino-build.log')
  'Exit code: ' + $LASTEXITCODE | Out-File (Join-Path $out 'arduino-build-result.txt')
} catch { $_.Exception.Message | Out-File (Join-Path $out 'arduino-build-result.txt') }
Get-ChildItem (Join-Path $root 'src\HLR_FE_Separated') -File | Get-FileHash -Algorithm SHA256 | Select-Object Hash,Path | Export-Csv (Join-Path $out 'firmware-checksums.csv') -NoTypeInformation
Write-Output "Build evidence folder: $out"
Write-Output 'Also record Arduino IDE version from Help > About and OpenMV firmware from os.uname() on the actual RT1062.'

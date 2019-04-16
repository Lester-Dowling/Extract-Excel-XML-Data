## PowerShell Script to conveniently build and run the CMake unit tests project in
## the current directory.
##
## @file        build-and-run-unit-tests.ps1
## @author      Lester J. Dowling
## @date        Started 2019-04-10-10-16-08
Param
(
    [parameter(mandatory = $false)]
    [Switch]
    [alias("c")]
    $CLEAN,

    [parameter(mandatory = $false)]
    [Switch]
    [alias("q")]
    $QUIET
)

###
### VLC location
###
New-Variable -Name VLC  -Scope Script  -Force
[string] $VLC = Resolve-Path "${ENV:ProgramFiles}/VideoLAN/VLC/vlc.exe" -ea:ignore    |    Select -ExpandProperty Path
[string] $VOLUME = "--directx-volume=0.1"
###
### Sounds Directory
###
New-Variable -Name SOUNDS_DIR  -Scope Script -Force
[string] $SOUNDS_DIR = Resolve-Path "${ENV:USERPROFILE}/sounds" -ea:ignore    |    Select -ExpandProperty Path


Function play_start_sound() {   # Commencement sound.
    if ($QUIET) {return}
    $START_SOUND = Resolve-Path "${SOUNDS_DIR}/Computer Data 01.wav" -ea:ignore    |    Select -ExpandProperty Path
    if (Test-Path $START_SOUND) {
        &${VLC}  $VOLUME  -I dummy  --quiet  --one-instance  --play-and-exit   "$START_SOUND"
    }
}


Function play_finished_sound() { # Successful completion.
    if ($QUIET) {return}
    $FINISH_SOUND = Resolve-Path "${SOUNDS_DIR}/Computer Data 02.wav" -ea:ignore    |    Select -ExpandProperty Path
    if (Test-Path $FINISH_SOUND) {
        &${VLC}  $VOLUME  -I dummy  --quiet  --one-instance  --play-and-exit   "$FINISH_SOUND"
    }
}


Function fatal_error_exit($ERROR_MESSAGE) {   # Fatal error; cannot continue.
    if (-Not($QUIET)) {
        $ERROR_SOUND = Resolve-Path "${SOUNDS_DIR}/Computer-Data-Error-Sound.wav" -ea:ignore    |    Select -ExpandProperty Path
        if (Test-Path $ERROR_SOUND) {
            &${VLC}  $VOLUME  -I dummy  --quiet  --one-instance  --play-and-exit   "$ERROR_SOUND"
        }
    }
    Write-Output "!!! [ERROR] $ERROR_MESSAGE"
    Write-VolumeCache C
    exit 1
}

#
# Invoke the given batch file through cmd.exe and retain its environment variables.
#
Function Invoke-Environment()
{
    # First, merely show banner and logo:
    $command = "CALL $Args"
    cmd.exe /c "$command"
    # Now, run the script for real:
    $command = "CALL $Args > nul 2>&1 && set"
    ## Write-Debug "Invoke-Environment command == $command"
    cmd.exe /c "$command" |
      . {process {
             if ($_ -match '^([^=]+)=(.*)') {
                 $EnvVar = $matches[1]
                 $EnvVal = $matches[2]
                 if (-Not([System.Environment]::GetEnvironmentVariable($EnvVar)) -or $EnvVar -eq "PATH") {
                     [System.Environment]::SetEnvironmentVariable($EnvVar, $EnvVal)
                     # Verbose output of newly set environment variables:
                     if ($EnvVal.Contains(";")) {
                         foreach ($v in ($EnvVal -split ';')) {
                             Write-Output ("{0} += {1}" -f $EnvVar,$v);
                         }
                     } else {
                         Write-Output ("{0} = {1}" -f $EnvVar,$EnvVal);
                     }
                 } else {
                     $ExistingVal = [System.Environment]::GetEnvironmentVariable($EnvVar);
                     if ($EnvVal -ne $ExistingVal) {
                         Write-Output ("!!! {0} = {1} vs existing {2}" -f $EnvVar,$EnvVal,$ExistingVal);
                     }
                 }
             }
         }}
    if ($LASTEXITCODE) {
        fatal_error_exit "Batch command '${args[0]}' exited with code: $LASTEXITCODE"
    }
}

#
# MAIN
#
play_start_sound

#
# Choose the version of Visual Studio
#
$VSYEAR = "2017"
$VSARCH = "Win64"
$VCTAG  = "vc141"
$VC = Resolve-Path "${ENV:ProgramFiles(x86)}\Microsoft Visual Studio\$VSYEAR\Community\VC" -ea:stop    |    Select -ExpandProperty Path
if (-Not(Test-Path $VC)) { fatal_error_exit "No VC path" }
$VCVARSALL = Resolve-Path "${VC}\Auxiliary\Build\vcvarsall.bat" -ea:stop    |    Select -ExpandProperty Path
if (-Not(Test-Path $VCVARSALL)) { fatal_error_exit "No VCVARSALL path" }

#
# Setup Compiler Variables
#
New-Variable VCVARSALL_SETUP_ONLY_ONCE
$VCVARSALL_SETUP_ONLY_ONCE = [System.Environment]::GetEnvironmentVariable("VCVARSALL_SETUP_ONLY_ONCE")
if (-Not($VCVARSALL_SETUP_ONLY_ONCE)) {
    Write-Host "Calling compiler setup script..."
    $DQ = '"'
    Invoke-Environment   "${DQ}${VCVARSALL}${DQ}"   "x64"
    [System.Environment]::SetEnvironmentVariable('VCVARSALL_SETUP_ONLY_ONCE', 1)
    cl
}

#
# Build Directory
#
$WORKSPACEHASH = "cbefd5ec-437e-6535-8692-ef753bfc67e5"
$BUILD="C:/Users/ljdowling/CMakeBuilds/${WORKSPACEHASH}/build/x64-Release"
$BUILD = Resolve-Path $BUILD -ea:stop  |  Select -ExpandProperty Path
if (-Not(Test-Path $BUILD)) { fatal_error_exit "No BUILD path" }
Write-Host "Build directory is: $BUILD"

#
# No user interaction
#
[System.Environment]::SetEnvironmentVariable('FINISH_WITHOUT_USER_PROMPT', 1)

#
# Build
#
$CMAKE = Resolve-Path "${VC}\..\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\CMake\bin\cmake.exe" -ea:stop    |    Select -ExpandProperty Path
if (-Not(Test-Path $CMAKE)) { fatal_error_exit "No CMAKE path" }
$CLEAN_FIRST = ""
IF ($CLEAN) { $CLEAN_FIRST = "--clean-first" }
&$CMAKE --build $BUILD  --config  RelWithDebInfo  $CLEAN_FIRST
if ($LASTEXITCODE -ne 0) { fatal_error_exit "CMake build failed."; }

#
# Run unit tests
#
$UNIT_TESTS = Resolve-Path "${BUILD}/Unit-Tests/Unit-Tests.exe" -ea:stop    |    Select -ExpandProperty Path
if (-Not(Test-Path $UNIT_TESTS)) { fatal_error_exit "No UNIT_TESTS path" }
cls
&$UNIT_TESTS --build_info  --color_output=false  --report_level=short  --log_level=message;
if ($LASTEXITCODE -ne 0) { fatal_error_exit "Unit tests failed."; }
play_finished_sound

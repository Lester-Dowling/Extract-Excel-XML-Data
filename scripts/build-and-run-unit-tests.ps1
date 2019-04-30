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

#
# Audio Player
#
New-Variable -Name AUDIO -Scope Script -Force
$AUDIO = New-Object System.Media.SoundPlayer

#
# Sounds Directory
#
New-Variable -Name SOUNDS_DIR  -Scope Script -Force
$SOUNDS_DIR = Resolve-Path "${ENV:USERPROFILE}/sounds" -ea:ignore


Function play_start_sound() {   # Commencement sound.
    if ($QUIET) {return}
    $START_SOUND = Resolve-Path "${SOUNDS_DIR}/Computer Data 01.wav" -ea:ignore
    if ($START_SOUND) {
        $AUDIO.SoundLocation = "$START_SOUND"
	    $AUDIO.Play()
    }
}


Function play_finished_sound() { # Successful completion.
    if ($QUIET) {return}
    $FINISH_SOUND = Resolve-Path "${SOUNDS_DIR}/Computer Data 02.wav" -ea:ignore
    if ($FINISH_SOUND) {
        $AUDIO.SoundLocation = "$FINISH_SOUND"
	    $AUDIO.Play()
    }
}


Function fatal_error_exit($ERROR_MESSAGE) {   # Fatal error; cannot continue.
    if (-Not($QUIET)) {
        $ERROR_SOUND = Resolve-Path "${SOUNDS_DIR}/Computer-Data-Error-Sound.wav" -ea:ignore
        if ($ERROR_SOUND) {
            $AUDIO.SoundLocation = "$ERROR_SOUND"
	        $AUDIO.Play()
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

#
# Choose the version of Visual Studio
#
$VSYEAR = "2017"
$VSARCH = "Win64"
$VCTAG  = "vc141"
$VC = Resolve-Path "${ENV:ProgramFiles(x86)}\Microsoft Visual Studio\$VSYEAR\Community\VC" -ea:ignore
if (-Not($VC)) { fatal_error_exit "Visual C++ is not installed." }
$VCVARSALL = Resolve-Path "${VC}\Auxiliary\Build\vcvarsall.bat" -ea:ignore
if (-Not($VCVARSALL)) { fatal_error_exit "No such file: vcvarsall.bat" }

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
$CMAKE_BINARY_DIR_FILE = Resolve-Path "${PSScriptRoot}/cmake_binary_dir.txt" -ea:Ignore
if (-Not($CMAKE_BINARY_DIR_FILE)) {
    fatal_error_exit "Build the project in Visual Studio 2017 first before running this script.  Missing binary directory."
}
$BINARY_DIR = Get-Content $CMAKE_BINARY_DIR_FILE
$BINARY_DIR = Resolve-Path "${BINARY_DIR}" -ErrorAction:Ignore
if (-Not($BINARY_DIR)) {
	fatal_error_exit "No such CMake binary directory: $BINARY_DIR"
}

#
# No user interaction
#
[System.Environment]::SetEnvironmentVariable('FINISH_WITHOUT_USER_PROMPT', 1)

#
# Build
#
play_start_sound
$CMAKE = Resolve-Path "${VC}\..\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\CMake\bin\cmake.exe" -ea:ignore
if (-Not($CMAKE)) { fatal_error_exit "cmake is not on PATH" }
$CLEAN_FIRST = ""
IF ($CLEAN) { $CLEAN_FIRST = "--clean-first" }
&$CMAKE --build $BINARY_DIR  --config  RelWithDebInfo  $CLEAN_FIRST
if ($LASTEXITCODE -ne 0) { fatal_error_exit "CMake build failed."; }

#
# Run unit tests
#
$UNIT_TESTS = Resolve-Path "${BINARY_DIR}/Unit-Tests/Unit-Tests.exe" -ea:ignore
if (-Not($UNIT_TESTS)) { fatal_error_exit "No UNIT_TESTS path" }
cls
&$UNIT_TESTS --color_output=false  --report_level=short  --log_level=message;
if ($LASTEXITCODE -ne 0) { Write-Host "Last error = $LASTEXITCODE"; fatal_error_exit "Unit tests failed."; }
play_finished_sound


# CMake Kit
# 1> Command line: C:\PROGRAM FILES (X86)\MICROSOFT VISUAL STUDIO\2017\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\CMake\bin\cmake.exe  -G "Ninja" -DCMAKE_INSTALL_PREFIX:PATH="C:\Users\ljdowling\CMakeBuilds\29884886-1304-9437-a529-31f5a8869390\install\x64-Release"  -DCMAKE_CXX_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.16.27023/bin/HostX64/x64/cl.exe"  -DCMAKE_C_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.16.27023/bin/HostX64/x64/cl.exe"  -DCMAKE_BUILD_TYPE="RelWithDebInfo" -DCMAKE_MAKE_PROGRAM="C:\PROGRAM FILES (X86)\MICROSOFT VISUAL STUDIO\2017\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\Ninja\ninja.exe" "C:\Users\ljdowling\Projects\Extract-Excel-XML-Data-2017"
# 1> Working directory: C:\Users\ljdowling\CMakeBuilds\29884886-1304-9437-a529-31f5a8869390\build\x64-Release

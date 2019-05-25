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
# Sounds Directory (ignored if non-existant)
#
New-Variable -Name SOUNDS_DIR  -Scope Script -Force
$SOUNDS_DIR = Resolve-Path "${ENV:USERPROFILE}/sounds/Program-Events/Microsoft-Visual-Studio" -ea:ignore


Function play_start_sound() {   # Commencement sound.
    if ($QUIET) {return}
    $START_SOUND = Resolve-Path "${SOUNDS_DIR}/Build-Start.wav" -ea:ignore
    if ($START_SOUND) {
        $AUDIO.SoundLocation = "$START_SOUND"
	    $AUDIO.Play()
    }
}


Function play_finished_sound() { # Successful completion.
    if ($QUIET) {return}
    $FINISH_SOUND = Resolve-Path "${SOUNDS_DIR}/Build-Succeeded.wav" -ea:ignore
    if ($FINISH_SOUND) {
        $AUDIO.SoundLocation = "$FINISH_SOUND"
	    $AUDIO.Play()
    }
}


Function fatal_error_exit($ERROR_MESSAGE) {   # Fatal error; cannot continue.
    if (-Not($QUIET)) {
        $ERROR_SOUND = Resolve-Path "${SOUNDS_DIR}/Build-Failed.wav" -ea:ignore
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
# Project Directory
#
$SCRIPT_DIR = Get-Item ${PSScriptRoot}
$SCRIPT_DIRNAME = $SCRIPT_DIR | Select -ExpandProperty Name
$PROJECT_DIR = $SCRIPT_DIR | Select -ExpandProperty Parent | Select -ExpandProperty FullName
if ($SCRIPT_DIRNAME -ne "scripts") { $PROJECT_DIR = Get-Item ${PSScriptRoot} | Select -ExpandProperty FullName }
Write-Host "PROJECT_DIR == $PROJECT_DIR"

#
# Choose the version of Visual Studio
#
$ARCH = "x64"
$VSYEAR = "2017"
$VC = Resolve-Path "${ENV:ProgramFiles(x86)}\Microsoft Visual Studio\$VSYEAR\Community\VC" -ea:ignore
if (-Not($VC)) { fatal_error_exit "Visual C++ is not installed." }
$VCVARSALL = Resolve-Path "${VC}\Auxiliary\Build\vcvarsall.bat" -ea:ignore
if (-Not($VCVARSALL)) { fatal_error_exit "No such file: vcvarsall.bat" }
$COMMONEXTENSIONS = Resolve-Path "${VC}\..\COMMON7\IDE\COMMONEXTENSIONS" -ea:Ignore
if (-Not($COMMONEXTENSIONS)) { fatal_error_exit "No common extensions directory for Visual Studio." }
$CMAKE = Resolve-Path "${COMMONEXTENSIONS}\MICROSOFT\CMAKE\CMake\bin\cmake.exe" -ea:Ignore
if (-Not($CMAKE)) { fatal_error_exit "cmake is not available" }
$MAKE_PROGRAM = Resolve-Path "${COMMONEXTENSIONS}\MICROSOFT\CMAKE\Ninja\ninja.exe" -ea:Ignore
if (-Not($MAKE_PROGRAM)) { fatal_error_exit "ninja is not available" }
$C_COMPILER = Resolve-Path "${VC}/Tools/MSVC/14.16.27023/bin/HostX64/x64/cl.exe" -ea:Ignore
if (-Not($C_COMPILER)) { fatal_error_exit "cl is not available" }
$CXX_COMPILER = Resolve-Path "${VC}/Tools/MSVC/14.16.27023/bin/HostX64/x64/cl.exe" -ea:Ignore
if (-Not($CXX_COMPILER)) { fatal_error_exit "cl is not available" }

#
# Build Type
#
$BUILD_TYPE = "RelWithDebInfo"

#
# Setup Compiler Variables
#
New-Variable VCVARSALL_SETUP_ONLY_ONCE
$VCVARSALL_SETUP_ONLY_ONCE = [System.Environment]::GetEnvironmentVariable("VCVARSALL_SETUP_ONLY_ONCE")
if (-Not($VCVARSALL_SETUP_ONLY_ONCE)) {
    Write-Host "Calling compiler setup script..."
    $DQ = '"'
    Invoke-Environment   "${DQ}${VCVARSALL}${DQ}"   "${ARCH}"
    [System.Environment]::SetEnvironmentVariable('VCVARSALL_SETUP_ONLY_ONCE', 1)
}

#
# Build Root Directory (aka Binary Directory)
#
$CMAKE_DEFAULT_BUILD_DIR = "build"
$CMAKE_DEFAULT_BINARY_DIR = "${ARCH}-Release"
$BINARY_DIR = ""
$CMAKE_BINARY_DIR_FILE = Resolve-Path "${PSScriptRoot}/cmake_binary_dir.txt" -ea:Ignore
if (-Not($CMAKE_BINARY_DIR_FILE)) {
    $BUILD_DIR = Resolve-Path "${PROJECT_DIR}/${CMAKE_DEFAULT_BUILD_DIR}" -ea:Ignore
    if (-Not($BUILD_DIR)) {
        New-Item -Path ${PROJECT_DIR} -Name ${CMAKE_DEFAULT_BUILD_DIR} -ItemType "directory" -Verbose
        $BUILD_DIR = Resolve-Path "${PROJECT_DIR}/${CMAKE_DEFAULT_BUILD_DIR}"
    }
    $BINARY_DIR = Resolve-Path "${BUILD_DIR}/${CMAKE_DEFAULT_BINARY_DIR}" -ea:Ignore
    if (-Not($BINARY_DIR)) {
        New-Item -Path ${BUILD_DIR} -Name ${CMAKE_DEFAULT_BINARY_DIR} -ItemType "directory" -Verbose
        $BINARY_DIR = Resolve-Path "${BUILD_DIR}/${CMAKE_DEFAULT_BINARY_DIR}"
    }
} else {
    $BINARY_DIR = Get-Content $CMAKE_BINARY_DIR_FILE
    $BINARY_DIR = Resolve-Path "${BINARY_DIR}" -ea:Ignore
}
if (-Not($BINARY_DIR)) {
	fatal_error_exit "No such CMake binary directory: $BINARY_DIR"
}
Write-Host "Binary directory: $BINARY_DIR"

cd "$BINARY_DIR"
if (-Not(Test-Path "CMakeCache.txt")) {
    $INSTALL_DIR = ${ENV:USERPROFILE}
    $INSTALL_DIR = "$INSTALL_DIR" -replace '\\','/'
    $CXX_COMPILER = "$CXX_COMPILER" -replace '\\','/'
    $C_COMPILER = "$C_COMPILER" -replace '\\','/'
    $MAKE_PROGRAM = "$MAKE_PROGRAM" -replace '\\','/'
    $PROJECT_DIR = "$PROJECT_DIR" -replace '\\','/'

    Write-Host "INSTALL_DIR == $INSTALL_DIR"
    Write-Host "CXX_COMPILER == $CXX_COMPILER"
    Write-Host "C_COMPILER == $C_COMPILER"
    Write-Host "MAKE_PROGRAM == $MAKE_PROGRAM"

    &$CMAKE -G "Ninja" -DCMAKE_INSTALL_PREFIX:PATH="$INSTALL_DIR" -DCMAKE_CXX_COMPILER:PATH="$CXX_COMPILER" -DCMAKE_C_COMPILER:PATH="$C_COMPILER" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_MAKE_PROGRAM="$MAKE_PROGRAM"  "$PROJECT_DIR"
}

#
# No user interaction
#
[System.Environment]::SetEnvironmentVariable('FINISH_WITHOUT_USER_PROMPT', 1)

#
# Build
#
play_start_sound
$CLEAN_FIRST = ""
if ($CLEAN) { $CLEAN_FIRST = "--clean-first" }
&$CMAKE --build $BINARY_DIR  --config  "$BUILD_TYPE"  $CLEAN_FIRST
if ($LASTEXITCODE -ne 0) { fatal_error_exit "CMake build failed."; }

#
# Run unit tests
#
$UNIT_TESTS = Resolve-Path "${BINARY_DIR}/Unit-Tests/Unit-Tests.exe" -ea:ignore
if (-Not($UNIT_TESTS)) { fatal_error_exit "No Unit_Tests executable." }
cls
Write-Host $UNIT_TESTS
& $UNIT_TESTS --color_output=false  --report_level=short  --log_level=message;
if ($LASTEXITCODE -ne 0) { fatal_error_exit "Unit tests failed."; }
cd "$PROJECT_DIR"
play_finished_sound


# CMake Kit
# 1> Command line:
# C:\PROGRAM FILES (X86)\MICROSOFT VISUAL STUDIO\2017\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\CMake\bin\cmake.exe
# -G "Ninja"
# -DCMAKE_INSTALL_PREFIX:PATH="C:\Users\ljdowling\CMakeBuilds\29884886-1304-9437-a529-31f5a8869390\install\x64-Release"
# -DCMAKE_CXX_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.16.27023/bin/HostX64/x64/cl.exe"
# -DCMAKE_C_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.16.27023/bin/HostX64/x64/cl.exe"
# -DCMAKE_BUILD_TYPE="RelWithDebInfo"
# -DCMAKE_MAKE_PROGRAM="C:\PROGRAM FILES (X86)\MICROSOFT VISUAL STUDIO\2017\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\Ninja\ninja.exe"
# "C:\Users\ljdowling\Projects\Extract-Excel-XML-Data-2017"
# 1> Working directory: C:\Users\ljdowling\CMakeBuilds\29884886-1304-9437-a529-31f5a8869390\build\x64-Release

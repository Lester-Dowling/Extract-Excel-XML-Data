## PowerShell Script to conveniently clean completely the CMake project in the
## current directory.
##
## @file        clean.ps1
## @author      Lester J. Dowling
## @date        Started 2019-05-01
Param
(
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
# Project Directory
#
$SCRIPT_DIR = Get-Item ${PSScriptRoot}
$SCRIPT_DIRNAME = $SCRIPT_DIR | Select -ExpandProperty Name
$PROJECT_DIR = $SCRIPT_DIR | Select -ExpandProperty Parent | Select -ExpandProperty FullName
if ($SCRIPT_DIRNAME -ne "scripts") { $PROJECT_DIR = Get-Item ${PSScriptRoot} | Select -ExpandProperty FullName }
Write-Host "PROJECT_DIR == $PROJECT_DIR"


#
# Build Root Directory and Binary Directory
#
$CMAKE_DEFAULT_BUILD_DIR = "build"
$CMAKE_DEFAULT_BINARY_DIR = "${ARCH}-Release"
$BINARY_DIR = ""
$BUILD_DIR = ""
$CMAKE_BINARY_DIR_FILE = Resolve-Path "${PSScriptRoot}/cmake_binary_dir.txt" -ea:Ignore
if (-Not($CMAKE_BINARY_DIR_FILE)) {
    $BUILD_DIR = Resolve-Path "${PROJECT_DIR}/${CMAKE_DEFAULT_BUILD_DIR}" -ea:Ignore
    if ($BUILD_DIR) {
		$BINARY_DIR = Resolve-Path "${BUILD_DIR}/${CMAKE_DEFAULT_BINARY_DIR}" -ea:Ignore
    }
} else {
    $BINARY_DIR = Get-Content $CMAKE_BINARY_DIR_FILE
    $C = @( $BINARY_DIR -split '/')
    $N = $C.Count
    if ($N -gt 3) {
        $N = $N - 1
        $BINARY_DIR_NAME = $C[$N]
        $N = $N - 1
        $BUILD_DIR_NAME = $C[$N]
        $BUILD_DIR = $C[0..$N] -join '/'
        $N = $N - 1
        $BUILD_PARENT = $C[0..$N] -join '/'
        $BUILD_DIR = Resolve-Path "${BUILD_DIR}" -ea:Ignore
        if ($BUILD_DIR) {
            $BINARY_DIR = Resolve-Path "${BUILD_DIR}/${BINARY_DIR_NAME}" -ea:Ignore
        }
    } else {
        fatal_error_exit "Binary directory path too short."
    }
}
if ($BUILD_DIR) {
	rm $BUILD_DIR -Recurse -Force -Verbose
}

#
# Visual Studio 2017 Settings
#
$VS_DIR = Resolve-Path "${PSScriptRoot}/../.vs" -ea:Ignore
if (-Not($VS_DIR)) {
	$VS_DIR = Resolve-Path "${PSScriptRoot}/.vs" -ea:Ignore
}
if ($VS_DIR) {
    rm $VS_DIR -Recurse -Force -Verbose
}

play_finished_sound

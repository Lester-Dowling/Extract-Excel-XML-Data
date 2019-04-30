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


#
# Build Directory
#
$CMAKE_BINARY_DIR_FILE = Resolve-Path "${PSScriptRoot}/cmake_binary_dir.txt" -ea:Ignore
if (-Not($CMAKE_BINARY_DIR_FILE)) {
    Write-Host "No such CMake output file: cmake_binary_dir.txt"
} else {
    $BINARY_DIR = Get-Content $CMAKE_BINARY_DIR_FILE
    $BINARY_DIR = Resolve-Path "${BINARY_DIR}" -ErrorAction:Ignore
    if (-Not($BINARY_DIR)) {
	    Write-Host "No such CMake binary directory: $BINARY_DIR"
    } else {
        rm $BINARY_DIR -Recurse -Force -Verbose
    }
}

#
# Visual Studio 2017 Settings
#
$VS_DIR = Resolve-Path "${PSScriptRoot}/../.vs" -ea:Ignore
if (-Not($VS_DIR)) {
    Write-Host "No such Visual Studio directory: ../.vs"
} else {
    rm $VS_DIR -Recurse -Force -Verbose
}


play_finished_sound

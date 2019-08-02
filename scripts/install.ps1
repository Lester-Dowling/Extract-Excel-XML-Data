## PowerShell Script to conveniently install the project.
##
## @file        install.ps1
## @author      Lester J. Dowling
## @date        Started 2019-04-25
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
if (-Not($QUIET)) {
    $AUDIO = New-Object System.Media.SoundPlayer
}

#
# Sounds Directory
#
New-Variable -Name SOUNDS_DIR  -Scope Script -Force
[string] $SOUNDS_DIR = Resolve-Path "${ENV:USERPROFILE}/sounds" -ea:ignore    |    Select -ExpandProperty Path

Function play_finished_sound() { # Successful completion.
    if ($QUIET) {return}
    $FINISH_SOUND = Resolve-Path "${SOUNDS_DIR}/Computer Data 02.wav" -ea:ignore    |    Select -ExpandProperty Path
    if (Test-Path $FINISH_SOUND) {
        $AUDIO.SoundLocation = "$FINISH_SOUND"
	    $AUDIO.Play()
    }
}

Function fatal_error_exit($ERROR_MESSAGE) {   # Fatal error; cannot continue.
    if (-Not($QUIET)) {
        $ERROR_SOUND = Resolve-Path "${SOUNDS_DIR}/Computer-Data-Error-Sound.wav" -ea:ignore    |    Select -ExpandProperty Path
        if (Test-Path $ERROR_SOUND) {
            $AUDIO.SoundLocation = "$ERROR_SOUND"
	        $AUDIO.Play()
        }
    }
    Write-Output "!!! [ERROR] $ERROR_MESSAGE"
    Write-VolumeCache C
    exit 1
}

#
# MAIN
#
$CMAKE_INFO = Resolve-Path "${PSScriptRoot}/cmake_binary_dir.txt" -ErrorAction:Ignore
if (-Not(Test-Path "${CMAKE_INFO}")) {
    fatal_error_exit "Build the project before installing it.  Missing: ${CMAKE_INFO}"
}
$BINARY_DIR = Get-Content "${CMAKE_INFO}"
$BINARY_DIR = Resolve-Path "${BINARY_DIR}" -ErrorAction:Ignore
if (-Not(Test-Path "$BINARY_DIR")) {
	fatal_error_exit "No such CMake binary directory: $BINARY_DIR"
}
$CMAKE_INSTALL = Resolve-Path "${BINARY_DIR}/cmake_install.cmake" -ErrorAction:Ignore
if (-Not(Test-Path "$CMAKE_INSTALL")) {
	fatal_error_exit "No such CMake install file: $CMAKE_INSTALL"
}

if (Get-Command cmake -ea:sil) {
    cmake -P "$CMAKE_INSTALL"
} 

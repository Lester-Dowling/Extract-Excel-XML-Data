### Build Boost with MSVC 2019 and address model x64:


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
    exit 0
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
play_start_sound

#
# Choose the version of Visual Studio
#
$ARCH = 'x64'
$VSYEAR = '2019'
$VC = Resolve-Path "${ENV:ProgramFiles(x86)}\Microsoft Visual Studio\$VSYEAR\Preview\VC" -ea:ignore
if (-Not($VC)) { fatal_error_exit "Visual C++ ${VSYEAR} is not installed." }
$TOOLS_MSVC = "${VC}\Tools\MSVC"
if (-Not($TOOLS_MSVC)) { fatal_error_exit "No Visual C++ tools." }
$TOOLS_VERSIONED = Get-ChildItem "$TOOLS_MSVC" -Directory | Sort -Property Name | Select -Last 1  |  Select -ExpandProperty FullName
$VCVARSALL = Resolve-Path "${VC}\Auxiliary\Build\vcvarsall.bat" -ea:ignore
if (-Not($VCVARSALL)) { fatal_error_exit "No such file: vcvarsall.bat" }
$COMMONEXTENSIONS = Resolve-Path "${VC}\..\COMMON7\IDE\COMMONEXTENSIONS" -ea:Ignore
if (-Not($COMMONEXTENSIONS)) { fatal_error_exit "No common extensions directory for Visual Studio." }
$CMAKE = Resolve-Path "${COMMONEXTENSIONS}\MICROSOFT\CMAKE\CMake\bin\cmake.exe" -ea:Ignore
if (-Not($CMAKE)) { fatal_error_exit "cmake is not available" }
$MAKE_PROGRAM = Resolve-Path "${COMMONEXTENSIONS}\MICROSOFT\CMAKE\Ninja\ninja.exe" -ea:Ignore
if (-Not($MAKE_PROGRAM)) { fatal_error_exit "ninja is not available" }
$C_COMPILER = Resolve-Path "${TOOLS_VERSIONED}/bin/HostX64/x64/cl.exe" -ea:Ignore
if (-Not($C_COMPILER)) { fatal_error_exit "C compiler in Visual Studio ${VSYEAR} is not available" }
$CXX_COMPILER = Resolve-Path "${TOOLS_VERSIONED}/bin/HostX64/x64/cl.exe" -ea:Ignore
if (-Not($CXX_COMPILER)) { fatal_error_exit  "C++ compiler in Visual Studio ${VSYEAR} is not available" }


Write-Host "================================================================================"
Write-Host "$PSCommandPath"
Write-Host "================================================================================"
$PROJECT_NAME='boost'
$PROJECT_VERSION='1_70_0'
$PROJECT_ARCHIVE_BASENAME="${PSScriptRoot}\${PROJECT_NAME}_${PROJECT_VERSION}"
$PROJECT_ARCHIVE_SUFFIXES_LIST=@( '7z', 'zip' ) # List of possible archive formats.
# Find an archive:
$PROJECT_ARCHIVE=""
foreach ($SUFFIX in $PROJECT_ARCHIVE_SUFFIXES_LIST)
{
    $PROJECT_ARCHIVE="${PSScriptRoot}\${PROJECT_NAME}_${PROJECT_VERSION}.${SUFFIX}"
    if ( Test-Path ${PROJECT_ARCHIVE} ) {
        break;
    }
}
$PROJECT_DIR="${PSScriptRoot}\${PROJECT_NAME}_${PROJECT_VERSION}"
$DST="${PSScriptRoot}\boost_MSVC_${VSYEAR}_${ARCH}"
$LOG="${DST}\build-log.txt"
Write-Host "Boost version:  $PROJECT_VERSION"
Write-Host "Boost archive:  $PROJECT_ARCHIVE"
Write-Host "Boost source:   $PROJECT_DIR"
Write-Host "Destination:    $DST"
Write-Host "Log file:       $LOG"
if ( -not( Test-Path ${PROJECT_ARCHIVE} )) {
    fatal_error_exit "No project archive for ${PROJECT_NAME} and version ${PROJECT_VERSION}"
}
if ( Test-Path ${PROJECT_DIR} ) {
    Write-Host "Removing previous project directory ${PROJECT_DIR} before unzipping ${PROJECT_ARCHIVE}..."
    Remove-Item -path ${PROJECT_DIR} -Force -Recurse -ErrorAction SilentlyContinue
}

# Unzip the project archive:
if(-Not(Test-Path 'C:\Program Files\7-Zip\7z.exe')) {
    fatal_error_exit '7z is required but is not installed.'
}
& 'C:\Program Files\7-Zip\7z.exe' x ${PROJECT_ARCHIVE}
if (-Not(Test-Path ${PROJECT_DIR})) {
    fatal_error_exit 'Unzipping project archive failed.'
}

# Remove previous build:
if ( Test-Path ${DST} ) {
    Write-Host "Removing previous destination ${DST} before unzipping ${PROJECT_ARCHIVE}..."
    Remove-Item -path ${DST} -Force -Recurse -ErrorAction SilentlyContinue
}
if ( -not( Test-Path ${DST} )) {
    New-Item -Path ${DST} -ItemType directory
}

# Setup compiler:
if ( -not( Test-Path "$VCVARSALL" )) {
    fatal_error_exit "MSVC ${YEAR} is required but is not installed."
}
cd ${PROJECT_DIR}
pwd
Write-Host 'Calling compiler setup script...'
$DQ='"'
Invoke-Environment   "${DQ}${VCVARSALL}${DQ}"   ${ARCH}

$ZLIB = Resolve-Path "${ENV:USERPROFILE}/oss/zlib" -ea:Ignore
if (-Not($ZLIB)) { fatal_error_exit  "zlib is not available" }

# Bootstrap:
Write-Host "Invoking bootstrap..."
& .\bootstrap.bat vc142
& .\b2.exe --help  |  out-file "${LOG}"
Get-ChildItem Env:  | out-file "${LOG}" -Append

Write-Host 'Invoking b2...'
& .\b2.exe -a -d+2 `
  'address-model=64' `
  'architecture=x86' `
  'threading=multi' `
  'toolset=msvc-14.2' `
  'link=shared' `
  "-sZLIB_SOURCE=${DQ}${ZLIB}${DQ}" `
  "-j${env:NUMBER_OF_PROCESSORS}" `
  '--without-mpi' `
  "--prefix=${DST}"  `
  '--build-type=complete' `
  install | tee-object -FilePath "${LOG}" -Append

play_finished_sound

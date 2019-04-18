cmake_minimum_required (VERSION 3.12)
set(TARGET "Unit-Practices")
add_executable (
  ${TARGET}
  "main-practice.cpp"
  "GlobalFixture.hpp"
  "GlobalFixture.cpp"
  )
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 17)
file(GLOB_RECURSE SRC_PRACTICE_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "practice/*")
target_sources(${TARGET} PRIVATE ${SRC_PRACTICE_FILES})
file(GLOB IO_SRC_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "io/*")
target_sources(${TARGET} PRIVATE ${IO_SRC_FILES})
target_include_directories(${TARGET} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${TARGET} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/practice)

foreach(src_file ${SRC_PRACTICE_FILES})
  message(STATUS "${TARGET} src file: ${src_file}")
endforeach(src_file)


if(WIN32)
  target_compile_definitions(
	${TARGET}
	PRIVATE
	UNICODE
	_UNICODE
	_CONSOLE
	NOMINMAX
	WIN64
	_WIN64
	WINAPI_FAMILY=WINAPI_FAMILY_DESKTOP_APP
	_WIN32_WINNT=_WIN32_WINNT_WIN10
	_CRT_SECURE_NO_WARNINGS
	_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
  target_compile_options(${TARGET} PRIVATE -GR) # Turn on RTTI.
endif()

#
# Pre-compiled Header
# Downloaded external project https://github.com/larsch/cmake-precompiled-header
#
get_filename_component(
  LOCAL_PRECOMPILED_CMAKE
  "${LOCALLY_CACHED_REPOS_DIR}/cmake-precompiled-header/PrecompiledHeader.cmake" ABSOLUTE)
if(EXISTS ${LOCAL_PRECOMPILED_CMAKE})
  message(STATUS "PrecompiledHeader.cmake: " ${LOCAL_PRECOMPILED_CMAKE})
  include(${LOCAL_PRECOMPILED_CMAKE})
  target_sources(${TARGET} PRIVATE "pch-practice.cpp")
  add_precompiled_header(${TARGET} "pch-practice.hpp" SOURCE_CXX "pch-practice.cpp")
else()
  message(WARNING "No such repo available: cmake-precompiled-header")
endif()

#
# Threads
#
target_link_libraries(${TARGET} PRIVATE Threads::Threads)

#
# Boost 1.69
#
target_link_libraries(
  ${TARGET}
  PRIVATE
  Boost::boost
  Boost::filesystem
  Boost::date_time
  Boost::program_options
  Boost::regex
  Boost::unit_test_framework
  )
add_custom_command(
  TARGET ${TARGET} PRE_BUILD
  COMMAND START /MIN "RoboCopy of Boost redist from parent directory" ROBOCOPY
  /COPY:DAT /NP /R:2 /W:2
  "${CMAKE_BINARY_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}"
  "boost*dll")

#
# Discover all the unit practices
#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")
include(Discover.Boost.Test)
boost_discover_tests(${TARGET})


message(STATUS "------------------ CMake Variable Logging ------------------")
message(STATUS "PROJECT_NAME:                " ${PROJECT_NAME})
message(STATUS "TARGET:                      " ${TARGET})
message(STATUS "CMAKE_SYSTEM_VERSION:        " ${CMAKE_SYSTEM_VERSION})
message(STATUS "CMAKE_SYSTEM_NAME:           " ${CMAKE_SYSTEM_NAME})
message(STATUS "CMAKE_HOST_SYSTEM_VERSION:   " ${CMAKE_HOST_SYSTEM_VERSION})
message(STATUS "CMAKE_BINARY_DIR:            " ${CMAKE_BINARY_DIR})
message(STATUS "CMAKE_CURRENT_BINARY_DIR:    " ${CMAKE_CURRENT_BINARY_DIR})

# This is the directory from which cmake was started, i.e. the top level source
# directory:
message(STATUS "CMAKE_SOURCE_DIR:            " ${CMAKE_SOURCE_DIR})

# This is the directory where the currently processed CMakeLists.txt is located
# in:
message(STATUS "CMAKE_CURRENT_SOURCE_DIR:    " ${CMAKE_CURRENT_SOURCE_DIR})

# Contains the full path to the top level directory of your build tree:
message(STATUS "PROJECT_BINARY_DIR:          " ${PROJECT_BINARY_DIR})

# Contains the full path to the root of your project source directory, i.e. to
# the nearest directory where CMakeLists.txt contains the PROJECT() command:
message(STATUS "PROJECT_SOURCE_DIR:          " ${PROJECT_SOURCE_DIR})

# Tell CMake to search first in directories listed in CMAKE_MODULE_PATH when you
# use FIND_PACKAGE() or INCLUDE():
message(STATUS "CMAKE_MODULE_PATH:           " ${CMAKE_MODULE_PATH})

# This is the complete path of the cmake which runs currently (e.g.
# /usr/local/bin/cmake):
message(STATUS "CMAKE_COMMAND:               " ${CMAKE_COMMAND})

# This is the CMake installation directory:
message(STATUS "CMAKE_ROOT:                  " ${CMAKE_ROOT})

# This is the filename including the complete path of the file where this
# variable is used:
message(STATUS "CMAKE_CURRENT_LIST_FILE:     " ${CMAKE_CURRENT_LIST_FILE})

# This is used when searching for include files e.g. using the FIND_PATH()
# command:
message(STATUS "CMAKE_INCLUDE_PATH:          " ${CMAKE_INCLUDE_PATH})

# This is used when searching for libraries e.g. using the FIND_LIBRARY()
# command:
message(STATUS "CMAKE_LIBRARY_PATH:          " ${CMAKE_LIBRARY_PATH})

# The complete system name, e.g. "Linux-2.4.22", "FreeBSD-5.4-RELEASE" or
# "Windows 5.1":
message(STATUS "CMAKE_SYSTEM:                " ${CMAKE_SYSTEM})

# The short system name, e.g. "Linux", "FreeBSD" or "Windows":
message(STATUS "CMAKE_SYSTEM_NAME:           " ${CMAKE_SYSTEM_NAME})

# Only the version part of CMAKE_SYSTEM:
message(STATUS "CMAKE_SYSTEM_VERSION:        " ${CMAKE_SYSTEM_VERSION})

# The CPU name (e.g. "Intel(R) Pentium(R) M processor 2.00GHz"):
message(STATUS "CMAKE_SYSTEM_PROCESSOR:      " ${CMAKE_SYSTEM_PROCESSOR})

# Is TRUE on all UNIX-like OS's, including Apple OS X and CygWin:
message(STATUS "UNIX:                        " ${UNIX})

# Is TRUE on Windows, including CygWin:
message(STATUS "WIN32:                       " ${WIN32})

# Is TRUE on Apple OS X:
message(STATUS "APPLE:                       " ${APPLE})

# Is TRUE when using the MinGW compiler in Windows:
message(STATUS "MINGW:                       " ${MINGW})

# Is TRUE on Windows when using the CygWin version of cmake:
message(STATUS "CYGWIN:                      " ${CYGWIN})

# Microsoft compiler:
message(STATUS "MSVC:                        " ${MSVC})
message(STATUS "MSVC_IDE:                    " ${MSVC_IDE})

# If set, runtime paths are not added when using shared libraries.  Default it
# is set to OFF:
message(STATUS "CMAKE_SKIP_RPATH:            " ${CMAKE_SKIP_RPATH})

# Set this to true if you are using makefiles and want to see the full compile
# and link commands instead of only the shortened ones:
message(STATUS "CMAKE_VERBOSE_MAKEFILE:      " ${CMAKE_VERBOSE_MAKEFILE})

# The compiler flags for compiling C sources:
message(STATUS "CMAKE_C_FLAGS:               " ${CMAKE_C_FLAGS})

# The compiler flags for compiling C++ sources:
message(STATUS "CMAKE_CXX_FLAGS:             " ${CMAKE_CXX_FLAGS})

# Choose the type of build.  Example: SET(CMAKE_BUILD_TYPE Debug):
message(STATUS "CMAKE_BUILD_TYPE:            " ${CMAKE_BUILD_TYPE})

# If this is set to ON, then all libraries are built as shared libraries by
# default:
message(STATUS "BUILD_SHARED_LIBS:           " ${BUILD_SHARED_LIBS})

# The compiler used for C files:
message(STATUS "CMAKE_C_COMPILER:            " ${CMAKE_C_COMPILER})

# The compiler used for C++ files:
message(STATUS "CMAKE_CXX_COMPILER_ID:       " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "CMAKE_CXX_COMPILER:          " ${CMAKE_CXX_COMPILER})

message(STATUS "---------------- End CMake Variable Logging ----------------")

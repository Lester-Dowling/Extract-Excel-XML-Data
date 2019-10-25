cmake_minimum_required(VERSION 3.15)
project(
  "Extract-Excel-XML-Data"
  VERSION 0.6.0
  DESCRIPTION
    "A command line tool to extract data from Excel files which were saved in the Excel 2003 XML format."
)
option(GRANT_INTERNET_ACCESS "Permit CMake to download from the Internet" OFF)
message(STATUS "Access Internet permission: ${GRANT_INTERNET_ACCESS}")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_sources(${PROJECT_NAME} PRIVATE main.cpp)
if(WIN32)
  target_compile_definitions(
    ${PROJECT_NAME}
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
  target_compile_options(${PROJECT_NAME} PRIVATE -GR) # Turn on RTTI.
endif()

#
# Config
#
configure_file("config.h.in" "${PROJECT_BINARY_DIR}/config.h")
include_directories("${PROJECT_BINARY_DIR}")

#
# Parent directory for locally cached repositories of open source software (oss)
#
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  get_filename_component(HOME $ENV{USERPROFILE} ABSOLUTE)
else()
  get_filename_component(HOME $ENV{HOME} ABSOLUTE)
endif()
get_filename_component(LOCALLY_CACHED_REPOS_DIR "${HOME}/oss" ABSOLUTE)
if(EXISTS ${LOCALLY_CACHED_REPOS_DIR})
  message(STATUS "Locally cached repositories folder: "
                 ${LOCALLY_CACHED_REPOS_DIR})
else()
  message(WARNING "No local repositories folder: " ${LOCALLY_CACHED_REPOS_DIR})
endif()

#
# External libraries
#
add_subdirectory("external")
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

#
# Boost
#
if(NOT DEFINED ENV{BOOST_REQUIRED_COMPONENTS})
  set(ENV{BOOST_REQUIRED_COMPONENTS} "boost")
endif()
function(boost_link_component)
  foreach(boost_component ${ARGV})
    set(ENV{BOOST_REQUIRED_COMPONENTS}
        "$ENV{BOOST_REQUIRED_COMPONENTS};${boost_component}")
  endforeach(boost_component)
endfunction()

# Build unit tests only if this project isn't a submodule in a super-project.
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
  message(STATUS "Building unit tests: ${BUILD_TESTING}")
  if(BUILD_TESTING)
    boost_link_component(filesystem date_time regex unit_test_framework)
  endif(BUILD_TESTING)
endif()

#
# Boost
#
boost_link_component(filesystem date_time regex program_options)
add_subdirectory("boost")
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost)
foreach(Boost_Lib $ENV{BOOST_REQUIRED_COMPONENTS})
  target_link_libraries(${PROJECT_NAME} PRIVATE Boost::${Boost_Lib})
endforeach(Boost_Lib)
if(NOT WIN32) # Support for <boost/dll.hpp> on Linux:
  target_link_libraries(${PROJECT_NAME} PUBLIC dl)
endif()

if(WIN32)
  if(NOT CMAKE_BINARY_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    message(STATUS "RoboCopy of Boost redist from parent directory")
    message(STATUS "CMAKE_BINARY_DIR         == ${CMAKE_BINARY_DIR}")
    message(STATUS "CMAKE_CURRENT_BINARY_DIR == ${CMAKE_CURRENT_BINARY_DIR}")
    add_custom_command(
      TARGET ${PROJECT_NAME} PRE_BUILD
      COMMAND
        START /MIN "RoboCopy of Boost redist from parent directory" #
        ROBOCOPY /COPY:DAT /NP /R:2 /W:2 "${CMAKE_BINARY_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}" "boost*dll")
  endif()
endif()

# I/OStream-Extras git submodule
if(NOT TARGET "IStream-Extra")
  add_subdirectory("IStream-Extra")
endif()
if(NOT TARGET "OStream-Extra")
  add_subdirectory("OStream-Extra")
endif()

# String-Extra git submodule
if(NOT TARGET "String-Extra")
  add_subdirectory("String-Extra")
endif()

# Pseudo XPath Parser submodule
if(NOT TARGET "Pseudo-XPath")
  add_subdirectory("Pseudo-XPath")
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "Pseudo-XPath")

# Simple XML submodule
if(NOT TARGET "Simple-XML")
  add_subdirectory("Simple-XML")
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "Simple-XML")

# The program operations
add_subdirectory("Operations")
target_link_libraries(${PROJECT_NAME} PRIVATE "Operations")

#
# Unit tests
#
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory("Unit-Tests")
endif()

#
# Installation
#
install(TARGETS ${TARGET} RUNTIME DESTINATION bin)

#
# Documentation
#
option(BUILD_DOCS "Whether to build or ignore documentation" OFF)
if(BUILD_DOCS)
  set(DOXYGEN_GENERATE_HTML YES)
  set(DOXYGEN_HTML_DYNAMIC_SECTIONS YES)
  set(DOXYGEN_HTML_EXTRA_STYLESHEET larger-font-sizes.css)
  set(DOXYGEN_HTML_COLORSTYLE_HUE 120)
  set(DOXYGEN_GENERATE_MAN NO)
  find_package(Doxygen REQUIRED dot OPTIONAL_COMPONENTS mscgen dia)
  doxygen_add_docs(
    ${PROJECT_NAME}
    "Pseudo-XPath"
    "Simple-XML"
    "Operations"
    "String-Extra"
    ALL
    COMMENT
    "Generate HTML Documentation")
endif(BUILD_DOCS)

#
# Info for scripts
#
file(WRITE "scripts/cmake_binary_dir.txt" ${CMAKE_BINARY_DIR})

message(STATUS "------------------ CMake Variable Logging ------------------")
message(STATUS "CMAKE_CURRENT_LIST_FILE:     " ${CMAKE_CURRENT_LIST_FILE})
message(STATUS "PROJECT_NAME:                " ${PROJECT_NAME})
message(STATUS "TARGET:                      " ${TARGET})
message(STATUS "CMAKE_SYSTEM:                " ${CMAKE_SYSTEM})
message(STATUS "CMAKE_SYSTEM_VERSION:        " ${CMAKE_SYSTEM_VERSION})
message(STATUS "CMAKE_SYSTEM_NAME:           " ${CMAKE_SYSTEM_NAME})
message(STATUS "CMAKE_SYSTEM_PROCESSOR:      " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "CMAKE_HOST_SYSTEM_VERSION:   " ${CMAKE_HOST_SYSTEM_VERSION})
message(STATUS "CMAKE_GENERATOR:             " ${CMAKE_GENERATOR})
message(STATUS "CMAKE_GENERATOR_INSTANCE:    " ${CMAKE_GENERATOR_INSTANCE})
message(STATUS "CMAKE_GENERATOR_PLATFORM:    " ${CMAKE_GENERATOR_PLATFORM})
message(STATUS "CMAKE_GENERATOR_TOOLSET:     " ${CMAKE_GENERATOR_TOOLSET})
message(STATUS "CMAKE_EXTRA_GENERATOR:       " ${CMAKE_EXTRA_GENERATOR})
message(STATUS "CMAKE_VS_PLATFORM_TOOLSET:   " ${CMAKE_VS_PLATFORM_TOOLSET})
message(STATUS "CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE: "
               ${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE})
message(STATUS "CMAKE_VS_PLATFORM_TOOLSET_VERSION: "
               ${CMAKE_VS_PLATFORM_TOOLSET_VERSION})
message(STATUS "CMAKE_COMMAND:               " ${CMAKE_COMMAND})
message(STATUS "CMAKE_ROOT:                  " ${CMAKE_ROOT})
message(STATUS "PROJECT_SOURCE_DIR:          " ${PROJECT_SOURCE_DIR})
message(STATUS "PROJECT_BINARY_DIR:          " ${PROJECT_BINARY_DIR})
message(STATUS "CMAKE_BINARY_DIR:            " ${CMAKE_BINARY_DIR})
message(STATUS "CMAKE_CURRENT_BINARY_DIR:    " ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "CMAKE_SOURCE_DIR:            " ${CMAKE_SOURCE_DIR})
message(STATUS "CMAKE_CURRENT_SOURCE_DIR:    " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "CMAKE_SYSTEM_PREFIX_PATH     " ${CMAKE_SYSTEM_PREFIX_PATH})
message(STATUS "CMAKE_SYSTEM_INCLUDE_PATH    " ${CMAKE_SYSTEM_INCLUDE_PATH})
message(STATUS "CMAKE_SYSTEM_LIBRARY_PATH    " ${CMAKE_SYSTEM_LIBRARY_PATH})
message(STATUS "CMAKE_SYSTEM_PROGRAM_PATH    " ${CMAKE_SYSTEM_PROGRAM_PATH})
message(STATUS "CMAKE_SYSTEM_IGNORE_PATH     " ${CMAKE_SYSTEM_IGNORE_PATH})
message(STATUS "CMAKE_LIBRARY_PATH:          " ${CMAKE_LIBRARY_PATH})
message(STATUS "CMAKE_MODULE_PATH:           " ${CMAKE_MODULE_PATH})
message(STATUS "CMAKE_INCLUDE_PATH:          " ${CMAKE_INCLUDE_PATH})
message(STATUS "UNIX:                        " ${UNIX})
message(STATUS "WIN32:                       " ${WIN32})
message(STATUS "APPLE:                       " ${APPLE})
message(STATUS "MINGW:                       " ${MINGW})
message(STATUS "CYGWIN:                      " ${CYGWIN})
message(STATUS "MSVC:                        " ${MSVC})
message(STATUS "MSVC_IDE:                    " ${MSVC_IDE})
message(STATUS "CMAKE_SKIP_RPATH:            " ${CMAKE_SKIP_RPATH})
message(STATUS "CMAKE_VERBOSE_MAKEFILE:      " ${CMAKE_VERBOSE_MAKEFILE})
message(STATUS "CMAKE_C_FLAGS:               " ${CMAKE_C_FLAGS})
message(STATUS "CMAKE_CXX_FLAGS:             " ${CMAKE_CXX_FLAGS})
message(STATUS "CMAKE_BUILD_TYPE:            " ${CMAKE_BUILD_TYPE})
message(STATUS "BUILD_SHARED_LIBS:           " ${BUILD_SHARED_LIBS})
message(STATUS "CMAKE_C_COMPILER:            " ${CMAKE_C_COMPILER})
message(STATUS "CMAKE_CXX_COMPILER_ID:       " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "CMAKE_EXE_LINKER_FLAGS:      " ${CMAKE_EXE_LINKER_FLAGS})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS:   " ${CMAKE_SHARED_LINKER_FLAGS})
message(STATUS "CMAKE_STATIC_LINKER_FLAGS:   " ${CMAKE_STATIC_LINKER_FLAGS})
message(STATUS "CMAKE_INSTALL_PREFIX:        " ${CMAKE_INSTALL_PREFIX})
message(STATUS "CMAKE_INSTALL_DEFAULT_COMPONENT_NAME: "
               ${CMAKE_INSTALL_DEFAULT_COMPONENT_NAME})
message(STATUS "CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS: "
               ${CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS})
message(STATUS "CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION: "
               ${CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION})

message(STATUS "---------------- End CMake Variable Logging ----------------")

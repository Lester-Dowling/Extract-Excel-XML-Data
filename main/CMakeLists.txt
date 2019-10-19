cmake_minimum_required(VERSION 3.14)
set(TARGET "Extract-Excel-XML-Data") # The name of the main exe.
add_executable(${TARGET})
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 17)
target_sources(${TARGET} PRIVATE main.cpp)

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
# Config
#
configure_file("config.h.in" "${PROJECT_BINARY_DIR}/config.h")
include_directories("${PROJECT_BINARY_DIR}")

#
# Import Program Operations
#
target_link_libraries(${TARGET} PRIVATE "Operations")

#
# Import XML Library
#
target_link_libraries(${TARGET} PRIVATE "Simple-XML")

#
# Import Pseudo XPath Library
#
target_link_libraries(${TARGET} PRIVATE "Pseudo-XPath")

#
# Threads
#
target_link_libraries(${TARGET} PRIVATE Threads::Threads)

#
# Boost
#
target_link_libraries(
  ${TARGET}
  PRIVATE #
          Boost::boost #
          Boost::filesystem #
          Boost::program_options #
          Boost::date_time #
          Boost::regex #
)
if(WIN32)
  add_custom_command(
    TARGET ${TARGET} PRE_BUILD
    COMMAND
      START /MIN "RoboCopy of Boost redist from parent directory" #
      ROBOCOPY /COPY:DAT /NP /R:2 /W:2 "${CMAKE_BINARY_DIR}"
      "${CMAKE_CURRENT_BINARY_DIR}" "boost*dll")
endif()

if(NOT WIN32) # Support for <boost/dll.hpp> on Linux:
  target_link_libraries(${TARGET} PUBLIC dl)
endif()

#
# Installation
#
install(TARGETS ${TARGET} RUNTIME DESTINATION bin)

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

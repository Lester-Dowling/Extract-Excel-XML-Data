cmake_minimum_required (VERSION 3.14)
set(TARGET "Unit-Practices")
add_executable (
  ${TARGET}
  "main-practice.cpp"
  "GlobalFixture.hpp"
  "GlobalFixture.cpp"
  )
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 17)
file(GLOB_RECURSE SRC_PRACTICE_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/practice/*")
target_sources(${TARGET} PRIVATE ${SRC_PRACTICE_FILES})
file(GLOB IO_SRC_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/io/*")
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
# Import IO Extra Library
#
target_link_libraries(${TARGET} PRIVATE "IO-Extra")

#
# Import Strings Extra Library
#
target_link_libraries(${TARGET} PRIVATE "Strings-Extra")

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

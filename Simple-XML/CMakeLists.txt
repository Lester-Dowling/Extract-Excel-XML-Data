cmake_minimum_required (VERSION 3.12)
set(TARGET "Simple-XML")
# A simplied XML which is enough to parse an Excel file.

add_library( ${TARGET} OBJECT )

file(GLOB SRC_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
file(GLOB PUB_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/public/*)
target_sources(${TARGET}
  PRIVATE ${SRC_FILES}
  PUBLIC  ${PUB_FILES}
  )

foreach(src_file ${SRC_FILES})
  message(STATUS "${TARGET} src file: ${src_file}")
endforeach(src_file)
foreach(pub_file ${PUB_FILES})
  message(STATUS "${TARGET} pub file: ${pub_file}")
endforeach(pub_file)

# Top-level include directories:
target_include_directories(${TARGET}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src)

#
# The public include directory for this library's clients
#
target_include_directories(${TARGET}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/public)


set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 17)
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
	_CRT_SECURE_NO_WARNINGS)
  target_compile_options(${TARGET} PRIVATE -GR) # Turn on RTTI.
endif()

#
# Pre-compiled Header
#
# Downloaded external project https://github.com/larsch/cmake-precompiled-header
#
get_filename_component(
  LOCAL_PRECOMPILED_CMAKE
  "${LOCALLY_CACHED_REPOS_DIR}/cmake-precompiled-header/PrecompiledHeader.cmake" ABSOLUTE)
if(EXISTS ${LOCAL_PRECOMPILED_CMAKE})
  message(STATUS "PrecompiledHeader.cmake: " ${LOCAL_PRECOMPILED_CMAKE})
  include(${LOCAL_PRECOMPILED_CMAKE})
  target_sources(${TARGET} PRIVATE "pch-simple-xml.cpp")
  add_precompiled_header(${TARGET} "pch-simple-xml.hpp" SOURCE_CXX "pch-simple-xml.cpp")
endif()

#
# Import Pseudo XPath Parser Library
#
target_link_libraries(${TARGET} PUBLIC "Pseudo-XPath-Parser")

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
  Boost::regex
  )

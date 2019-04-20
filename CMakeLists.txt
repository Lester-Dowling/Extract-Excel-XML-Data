cmake_minimum_required (VERSION 3.12)
project (
  "Extract-Excel-XML-Data-2017"
  VERSION 1.0.0
  DESCRIPTION "A command line tool to extract data from Excel files which were saved in the Excel 2003 XML format."
  )
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(GRANT_INTERNET_ACCESS "Permit CMake to download from the Internet" OFF)
message(STATUS "Access Internet permission: ${GRANT_INTERNET_ACCESS}")

set(CMAKE_VERBOSE_MAKEFILE ON)

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
  message(STATUS "Locally cached repositories folder: " ${LOCALLY_CACHED_REPOS_DIR})
else()
  message(WARNING "No local repositories folder: " ${LOCALLY_CACHED_REPOS_DIR})
endif()


# Make globally available all required external libraries:
add_subdirectory ("external")

# Simple XML library
add_subdirectory ("Simple-XML")

# XML Parser library
add_subdirectory ("Simple-XML-Parser")

# Pseudo XPath Parser library
add_subdirectory ("Pseudo-XPath-Parser")

# The program operations
add_subdirectory("Operations")

# main is an execution harness around Operations
add_subdirectory ("main")

# Unit tests
include(CTest)
enable_testing()
add_subdirectory ("Unit-Tests")

# Documentation
set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_HTML_DYNAMIC_SECTIONS YES)
set(DOXYGEN_HTML_EXTRA_STYLESHEET larger-font-sizes.css)
set(DOXYGEN_HTML_COLORSTYLE_HUE 120)
set(DOXYGEN_GENERATE_MAN NO)
find_package(Doxygen
  REQUIRED dot
  OPTIONAL_COMPONENTS mscgen dia)

doxygen_add_docs(
  ${PROJECT_NAME}
  "Pseudo-XPath-Parser"
  "Simple-XML"
  "Operations"
  ALL
  COMMENT "Generate HTML Documentation"
  )

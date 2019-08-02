cmake_minimum_required (VERSION 3.14)

set(USE_SYSTEM_TZ_DB ON)
set(DATE_INTERNET_REPO "https://github.com/HowardHinnant/date.git")
get_filename_component(DATE_LOCAL_REPO "${LOCALLY_CACHED_REPOS_DIR}/date" ABSOLUTE)
set(DATE_REPO "${DATE_LOCAL_REPO}") # Default
if(GRANT_INTERNET_ACCESS)
  set(DATE_REPO "${DATE_INTERNET_REPO}") # Download from Internet
else()
  if(NOT EXISTS ${DATE_REPO})
    message(FATAL_ERROR "No date local repo: " ${DATE_DIR})
  endif()
endif()
message(STATUS "date repo is " ${DATE_REPO})
include(FetchContent)
fetchcontent_declare(date_prj GIT_REPOSITORY "${DATE_REPO}" GIT_TAG master)
fetchcontent_getproperties(date_prj)
if(NOT date_POPULATED)
  fetchcontent_populate(date_prj)
endif()
message(STATUS "date source dir is " ${date_prj_SOURCE_DIR})
message(STATUS "date binary dir is " ${date_prj_BINARY_DIR})
# add_subdirectory(${date_prj_SOURCE_DIR} ${date_prj_BINARY_DIR})

add_library(date::interface INTERFACE IMPORTED GLOBAL)

# set_target_properties(
# date_interface
# PROPERTIES IMPORTED_GLOBAL TRUE
# )

target_include_directories(date::interface INTERFACE "${date_prj_SOURCE_DIR}/include")
target_compile_features(date::interface INTERFACE cxx_std_17)

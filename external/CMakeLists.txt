cmake_minimum_required (VERSION 3.12)

add_subdirectory ("threads")

set(USE_SYSTEM_TZ_DB ON)
add_subdirectory ("date")

add_subdirectory ("boost")

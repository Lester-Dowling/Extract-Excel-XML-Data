cmake_minimum_required (VERSION 3.14)
set(TARGET "Strings-Extra")
add_library( ${TARGET} INTERFACE )

file(GLOB_RECURSE PUB_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/public/*)
target_sources(${TARGET}
  INTERFACE  ${PUB_FILES}
  )

foreach(pub_file ${PUB_FILES})
  message(STATUS "${TARGET} pub file: ${pub_file}")
endforeach(pub_file)

# Top-level include directories:
target_include_directories(${TARGET}
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR})

#
# The public include directory for this library's clients
#
target_include_directories(${TARGET}
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/public)

#
# Boost 1.69
#
target_link_libraries(
  ${TARGET}
  INTERFACE
  Boost::boost
  )

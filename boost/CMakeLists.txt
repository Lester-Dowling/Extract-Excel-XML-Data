cmake_minimum_required(VERSION 3.15)
set(BOOST_VERSION 1.71.0)

#
# BOOST_REQUIRED_COMPONENTS
#
if(NOT DEFINED ENV{BOOST_REQUIRED_COMPONENTS})
  message(FATAL_ERROR "Env var BOOST_REQUIRED_COMPONENTS should be defined.")
endif()
set(BOOST_REQUIRED_COMPONENTS $ENV{BOOST_REQUIRED_COMPONENTS})
list(SORT BOOST_REQUIRED_COMPONENTS)
list(REMOVE_DUPLICATES BOOST_REQUIRED_COMPONENTS)
list(REMOVE_ITEM BOOST_REQUIRED_COMPONENTS boost)
set(ENV{BOOST_REQUIRED_COMPONENTS} "${BOOST_REQUIRED_COMPONENTS}")
foreach(Boost_Lib $ENV{BOOST_REQUIRED_COMPONENTS})
  message(STATUS "Boost required component: " ${Boost_Lib})
endforeach(Boost_Lib)

set(BUILD_SHARED_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_DEBUG_LIBS OFF)
set(Boost_USE_DEBUG_RUNTIME OFF)
set(Boost_ADDITIONAL_VERSIONS "${BOOST_VERSION}")
get_filename_component(BOOST_ROOT "$ENV{BOOST_ROOT}" ABSOLUTE)
if(NOT EXISTS ${BOOST_ROOT})
  message(FATAL_ERROR "BOOST_ROOT is not defined.")
endif()
get_filename_component(BOOST_LIBRARYDIR "$ENV{BOOST_LIBRARYDIR}" ABSOLUTE)
if(NOT EXISTS ${BOOST_LIBRARYDIR})
  # Try to guess location of Boost lib directory:
  if(WIN32)
    get_filename_component(BOOST_LIBRARYDIR "$ENV{BOOST_ROOT}/../../lib"
                           ABSOLUTE)
  endif()
  if(NOT EXISTS ${BOOST_LIBRARYDIR})
    message(FATAL_ERROR "BOOST_LIBRARYDIR is not defined.")
  endif()
endif()
get_filename_component(BOOST_CMAKE_DIR "${BOOST_LIBRARYDIR}/cmake" ABSOLUTE)
if(EXISTS ${BOOST_CMAKE_DIR})
  find_package(Boost ${BOOST_VERSION} REQUIRED
               COMPONENTS ${BOOST_REQUIRED_COMPONENTS} PATHS ${BOOST_CMAKE_DIR})
else()
  find_package(Boost ${BOOST_VERSION} REQUIRED
               COMPONENTS ${BOOST_REQUIRED_COMPONENTS})
endif()

if(Boost_FOUND)
  set_target_properties(Boost::boost PROPERTIES IMPORTED_GLOBAL TRUE)
  foreach(Boost_Lib ${BOOST_REQUIRED_COMPONENTS})
    set_target_properties(Boost::${Boost_Lib} PROPERTIES IMPORTED_GLOBAL TRUE)
  endforeach(Boost_Lib)
  target_compile_definitions(
    Boost::boost
    INTERFACE
    BOOST_ALL_DYN_LINK
    BOOST_SYSTEM_NO_DEPRECATED
    BOOST_SYSTEM_NO_LIB
    BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
    _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

  if(WIN32)
    target_compile_definitions(
      Boost::boost INTERFACE BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE
      BOOST_ASIO_HAS_IOCP BOOST_ASIO_HEADER_ONLY
      _WINSOCK_DEPRECATED_NO_WARNINGS)
  endif(WIN32)

  #
  # Copy Boost DLLs
  #
  if(WIN32)
    set(BOOST_REDIST_COPY_LIST "")
    foreach(Boost_Lib ${BOOST_REQUIRED_COMPONENTS})
      get_target_property(BOOST_LIB_ABSOLUTE Boost::${Boost_Lib}
                          IMPORTED_LOCATION_RELEASE)
      get_filename_component(LIB_BASENAME ${BOOST_LIB_ABSOLUTE} NAME_WE)
      get_filename_component(BOOST_REDIST_DIR ${BOOST_LIB_ABSOLUTE} DIRECTORY)
      set(DLL_FILENAME "${LIB_BASENAME}.dll")
      get_filename_component(DLL_ABSOLUTE "${BOOST_REDIST_DIR}/${DLL_FILENAME}"
                             ABSOLUTE)
      if(NOT EXISTS ${DLL_ABSOLUTE})
        message(FATAL_ERROR "No such Boost DLL: ${DLL_ABSOLUTE}")
      endif()
      message(STATUS "Copied: ${DLL_FILENAME}")
      set(BOOST_REDIST_COPY_LIST
          "${BOOST_REDIST_COPY_LIST} \"${DLL_FILENAME}\"")
      # Installation:
      install(FILES "${DLL_ABSOLUTE}" DESTINATION bin)
    endforeach(Boost_Lib)
    file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/RoboCopy-Log-Boost-redist.txt"
         BOOST_REDIST_COPY_LOG)
    # message(STATUS "Boost redist folder = " ${BOOST_REDIST_DIR})
    # message(STATUS "Boost redist copy list = " ${BOOST_REDIST_COPY_LIST})
    message(STATUS "Boost redist copy log = " ${BOOST_REDIST_COPY_LOG})
    add_custom_target(
      RoboCopy_Boost_Redist ALL
      COMMENT "RoboCopy of Boost redist"
      COMMAND
        START /MIN "RoboCopy of Boost redist" #
        ROBOCOPY /LOG:"${BOOST_REDIST_COPY_LOG}" /COPY:DAT /NP /R:2 /W:2
        "${BOOST_REDIST_DIR}" "${CMAKE_BINARY_DIR}" ${BOOST_REDIST_COPY_LIST})
  endif(WIN32)

  message(STATUS "------------------ CMake Variable Logging ------------------")
  message(STATUS "Boost_VERSION:               " ${Boost_VERSION})
  message(STATUS "BOOST_ROOT:                  " ${BOOST_ROOT})
  message(STATUS "BOOST_LIBRARYDIR:            " ${BOOST_LIBRARYDIR})
  message(STATUS "BOOST_CMAKE_DIR:             " ${BOOST_CMAKE_DIR})
  message(STATUS "---------------- End CMake Variable Logging ----------------")

endif(Boost_FOUND)

cmake_minimum_required (VERSION 3.14)

set(BOOST_VERSION 1.71.0)
set(Boost_LIBRARIES filesystem program_options date_time regex unit_test_framework)

set(BUILD_SHARED_LIBS            ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_RELEASE_LIBS       ON)
set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(Boost_USE_DEBUG_LIBS        OFF)
set(Boost_USE_DEBUG_RUNTIME     OFF)
set(Boost_ADDITIONAL_VERSIONS   "${BOOST_VERSION}")
get_filename_component(BOOST_ROOT "$ENV{BOOST_ROOT}" ABSOLUTE)
if(NOT EXISTS ${BOOST_ROOT})
  message(FATAL_ERROR "BOOST_ROOT is not defined.")
endif()
get_filename_component(BOOST_LIBRARYDIR "$ENV{BOOST_LIBRARYDIR}" ABSOLUTE)
if(NOT EXISTS ${BOOST_LIBRARYDIR})
  message(FATAL_ERROR "BOOST_LIBRARYDIR is not defined.")
endif()
get_filename_component(BOOST_CMAKE_DIR "${BOOST_LIBRARYDIR}/cmake" ABSOLUTE)
if(NOT EXISTS ${BOOST_CMAKE_DIR})
  message(FATAL_ERROR "BOOST_CMAKE_DIR is not defined.")
endif()

find_package(
  Boost ${BOOST_VERSION} REQUIRED
  COMPONENTS ${Boost_LIBRARIES}
  PATHS ${BOOST_CMAKE_DIR}
  )

if (Boost_FOUND)
  set_target_properties(
	Boost::boost
	PROPERTIES IMPORTED_GLOBAL TRUE
	)
  foreach(Boost_Lib ${Boost_LIBRARIES})
    set_target_properties(
	  ${Boost_Lib}
	  PROPERTIES IMPORTED_GLOBAL TRUE
	  )
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
	  Boost::boost
	  INTERFACE
	  BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE
	  BOOST_ASIO_HAS_IOCP
	  BOOST_ASIO_HEADER_ONLY
	  WINAPI_FAMILY=WINAPI_FAMILY_DESKTOP_APP
	  _WIN32_WINNT=_WIN32_WINNT_WIN10
	  _WINSOCK_DEPRECATED_NO_WARNINGS)
  endif(WIN32)

  if(NOT WIN32)
	# Support for <boost/dll.hpp> on Linux:
	target_link_libraries(Boost::boost PUBLIC dl)
  endif()

  #
  # Copy Boost DLLs
  #
  if(WIN32)
	set(BOOST_REDIST_COPY_LIST "")
    foreach(Boost_Lib ${Boost_LIBRARIES})
	  get_target_property(BOOST_LIB_ABSOLUTE ${Boost_Lib} IMPORTED_LOCATION_RELEASE)
	  get_filename_component(LIB_BASENAME ${BOOST_LIB_ABSOLUTE} NAME_WE)
	  get_filename_component(BOOST_REDIST_DIR ${BOOST_LIB_ABSOLUTE} DIRECTORY)
	  set(DLL_FILENAME "${LIB_BASENAME}.dll")
	  get_filename_component(DLL_ABSOLUTE "${BOOST_REDIST_DIR}/${DLL_FILENAME}" ABSOLUTE)
	  if(NOT EXISTS ${DLL_ABSOLUTE})
	    message(FATAL_ERROR "No such Boost DLL: ${DLL_ABSOLUTE}")
	  endif()
	  message(STATUS "Copied: ${DLL_FILENAME}")
	  set(BOOST_REDIST_COPY_LIST "${BOOST_REDIST_COPY_LIST} \"${DLL_FILENAME}\"")
      # Installation:
      install(FILES "${DLL_ABSOLUTE}" DESTINATION bin )
    endforeach(Boost_Lib)
    file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/RoboCopy-Log-Boost-redist.txt" BOOST_REDIST_COPY_LOG)
    # message(STATUS "Boost redist folder = " ${BOOST_REDIST_DIR})
	# message(STATUS "Boost redist copy list = " ${BOOST_REDIST_COPY_LIST})
	message(STATUS "Boost redist copy log = " ${BOOST_REDIST_COPY_LOG})
	add_custom_target(RoboCopy_Boost_Redist ALL
	  COMMENT "RoboCopy of Boost redist"
	  COMMAND START /MIN "RoboCopy of Boost redist" ROBOCOPY
	  /LOG:"${BOOST_REDIST_COPY_LOG}" /COPY:DAT /NP /R:2 /W:2
	  "${BOOST_REDIST_DIR}" "${CMAKE_BINARY_DIR}"
	  ${BOOST_REDIST_COPY_LIST})
  endif(WIN32)

  message(STATUS "------------------ CMake Variable Logging ------------------")
  message(STATUS "Boost_VERSION:               " ${Boost_VERSION})
  message(STATUS "BOOST_ROOT:                  " ${BOOST_ROOT})
  message(STATUS "BOOST_LIBRARYDIR:            " ${BOOST_LIBRARYDIR})
  message(STATUS "BOOST_CMAKE_DIR:             " ${BOOST_CMAKE_DIR})
  foreach(Boost_Lib ${Boost_LIBRARIES})
    message(STATUS "Boost_Lib:                   " ${Boost_Lib})
  endforeach(Boost_Lib)
  message(STATUS "---------------- End CMake Variable Logging ----------------")

endif(Boost_FOUND)

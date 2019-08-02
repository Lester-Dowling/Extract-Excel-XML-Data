cmake_minimum_required (VERSION 3.14)

#
# Threads
#
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if (Threads_FOUND)
  set_target_properties(
	Threads::Threads
	PROPERTIES IMPORTED_GLOBAL TRUE
	)
endif (Threads_FOUND)

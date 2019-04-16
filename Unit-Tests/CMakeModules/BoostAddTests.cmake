# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
set(prefix "${TEST_PREFIX}")
set(suffix "${TEST_SUFFIX}")
set(spec ${TEST_SPEC})
set(extra_args ${TEST_EXTRA_ARGS})
set(properties ${TEST_PROPERTIES})
set(script)
set(suite)
set(tests)

function(add_command NAME)
  set(_args "")
  foreach(_arg ${ARGN})
    if(_arg MATCHES "[^-./:a-zA-Z0-9_]")
      set(_args "${_args} [==[${_arg}]==]") # form a bracket argument
    else()
      set(_args "${_args} ${_arg}")
    endif()
  endforeach()
  set(script "${script}${NAME}(${_args})\n" PARENT_SCOPE)
endfunction()

if(NOT EXISTS "${TEST_EXECUTABLE}")
  message(FATAL_ERROR "No such test executable: ${TEST_EXECUTABLE}")
endif()

# Discover a list of tests from Boost.Test.  Use GraphViz to parse the DOT output
# from --list_content.  Assumes GraphViz's bin is in the execution PATH.
# get_filename_component(
	# PARSE_DOT_OUTPUT_C "C:/Users/ljdowling/Projects/Extract-Excel-XML-Data-2017/Unit-# Tests/CMakeModules/parse-dot-output.c" ABSOLUTE)

get_filename_component(PARSE_DOT_OUTPUT_C "parse-dot-output.c" ABSOLUTE)

file(WRITE ${PARSE_DOT_OUTPUT_C} [=[
/*
 * gvpr script
 * ===========
 *
 * Discover a list of tests from Boost.Test.  This script parses the DOT output
 * from Boost.Test's --list_content command line option.
 *
 */
BEGIN {
    int i, indent;
    int seen[string];
    string breadcrumb[int];
    void print_breadcrumbs (int cnt) {
        for (i = 1; i < cnt-1; i++) {
            printf ("%s/",breadcrumb[i]);
        }
        print(breadcrumb[cnt-1]);
    }
}
BEG_G {
    $tvtype = TV_prepostfwd;
    /* $tvroot = node($,ARGV[0]); */
}
N {
    if (seen[$.name]) indent--;
    else {
        string labelsplit[int];
        split($.label, labelsplit, "|");
        breadcrumb[indent] = labelsplit[0];
        seen[$.name] = 1;
        indent++;
        if ($.outdegree == 0) {
            print_breadcrumbs(indent);
        }
    }
}
]=])

execute_process(
  COMMAND "${TEST_EXECUTABLE}" ${spec} --list_content=DOT --report_sink=stdout
  COMMAND gvpr -f "${PARSE_DOT_OUTPUT_C}"
  OUTPUT_VARIABLE discovered_test_list
  RESULT_VARIABLE discovered_result)

if(${discovered_result} EQUAL 0)
  # Convert process output to a CMake list:
  string(REPLACE "\n" ";" discovered_test_list "${discovered_test_list}")

  # Discover which tests are enabled/disabled:
  execute_process(
    COMMAND "${TEST_EXECUTABLE}" ${spec} --list_content=HRF --report_sink=stdout
    OUTPUT_VARIABLE abled_test_list
    RESULT_VARIABLE abled_result)
  if(${abled_result} EQUAL 0)
    # Convert process output to a CMake list:
    string(REPLACE "\n" ";" abled_test_list "${abled_test_list}")
    string(REPLACE " " "" abled_test_list "${abled_test_list}")
    string(REGEX REPLACE ":[^;]*;" ";" abled_test_list "${abled_test_list}")
    string(REGEX REPLACE ":.*$" "" abled_test_list "${abled_test_list}")

    # Compose a run test command for each discovered test:
    foreach(test_name ${discovered_test_list})
      string(REPLACE "/" ";" test_name_components ${test_name})
      list(GET test_name_components -1 test_name_leaf)
      string(CONCAT test_name_leaf_enabled ${test_name_leaf} "*")
      if (test_name_leaf_enabled IN_LIST abled_test_list)
        list(REMOVE_ITEM abled_test_list test_name_leaf_enabled)
        add_command(add_test
          "${prefix}${test_name}${suffix}"
          ${TEST_EXECUTOR}
          "${TEST_EXECUTABLE}"
          "--run_test=${test_name}"
          ${extra_args})
        add_command(set_tests_properties
          "${prefix}${test_name}${suffix}"
          PROPERTIES
          WORKING_DIRECTORY
          "${TEST_WORKING_DIR}"
          ${properties})
        list(APPEND tests "${prefix}${test_name}${suffix}")
      endif()
    endforeach()

    # Create a list of all discovered tests, which users may use, for example, to
    # set properties on the tests:
    add_command(set ${TEST_LIST} ${tests})

    # Write out the CTest script:
    file(WRITE "${CTEST_FILE}" "${script}")
  else()
    message(FATAL_ERROR
      "Error running test executable '${TEST_EXECUTABLE}':\n"
      "  Result: ${abled_result}\n"
      "  Output: ${abled_test_list}\n")
  endif()
else()
  message(FATAL_ERROR
    "Error running test executable '${TEST_EXECUTABLE}':\n"
    "  Result: ${discovered_result}\n"
    "  Output: ${discovered_test_list}\n")
endif()

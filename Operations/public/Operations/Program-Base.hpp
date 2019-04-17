/**
 * @file   Operations/Program-Base.hpp
 * @date   Started Wed 02 Mar 2011 12:02:06 PM EST
 * @author Lester J. Dowling
 */
#pragma once
#include <boost/program_options.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include "Operations/No-Op.hpp"
namespace operations {

	/**
	 * Operational parameters for this program such as can be configured from the
	 * command line.
	 */
	class Program_Base
	{
	  public:
		int argc;
		char** argv;
		int gExitCode = EXIT_SUCCESS;
		bool gVerbose = false;
		std::ostream* gOut;
		std::ostream* gErr;
		struct Version
		{
			int major, minor, small;
		};
		Version gVersion{ 1, 0, 0 };

	  protected:
		/*
		 * Options for only the command line.
		 */
		boost::program_options::options_description gGeneric{ "Generic options" };

		/*
		 * Options for the command line, but wont show in user help.
		 */
		boost::program_options::options_description gHidden{ "Hidden options" };

		boost::program_options::positional_options_description gPositional;
		boost::program_options::options_description gCmdLine{ "Command line options" };
		boost::program_options::variables_map gVM;

		Program_Base(int argc, char** argv, std::ostream& out, std::ostream& err);
		virtual ~Program_Base() = default;

		virtual void print_help_if_asked();
		virtual void print_version_if_asked();
		virtual void setup_option_descriptions();
		virtual void parse_command_line_options();
		virtual void print_custom_help();
		virtual void perform_requested_operation() = 0;
		virtual void set_out(std::ostream& out);
		virtual void set_err(std::ostream& err);
		virtual void save() = 0;

	  public:
		/**
		 * Run the program and let all exceptions through.
		 */
		virtual void run();

		/**
		 * Run the program and catch all exceptions.  Write an error report to the user for
		 * any thrown exception.
		 */
		virtual void run_and_report_exceptions() noexcept;

		/**
		 * Ask the user to press <Enter> to finish the program run.
		 */
		virtual void politely_finish_if_asked();

		/**
		 * Present to the user a report of an exception within the program.
		 */
		virtual void report_exception(
		  const char* const title,
		  const std::string& what = std::string{}) noexcept;

		/**
		 * Report an emergency exit of program because it failed to start.
		 */
		static void report_startup_error(
		  const char* const title,
		  std::string const& what = std::string{}) noexcept;
	};
} // namespace operations

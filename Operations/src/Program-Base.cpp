/**
 * @file   Operations/src/Program-Base.cpp
 * @date   Started Wed 02 Mar 2011 12:02:06 PM EST
 * @author Lester J. Dowling
 */
#include "pch-operations.hpp"
#include "Operations/Program-Base.hpp"
#include <boost/dll.hpp>
namespace std {
	ostream& operator<<(ostream& os, operations::Program_Base::Version const& v)
	{
		return os << v.major << '.' << v.minor << '.' << v.small;
	}
}
namespace operations {

	Program_Base::Program_Base(int argc, char** argv, std::ostream& o, std::ostream& e)
	  : argc{ argc }
	  , argv{ argv }
	{
		this->set_out(o);
		this->set_err(e);
	}

	void Program_Base::setup_option_descriptions()
	{
		gGeneric.add_options()
		  // -----------------------------------------------------------------
		  ("version,v", "Print version string.")
		  // -----------------------------------------------------------------
		  ("help,h", "Print this help message.")
		  // -----------------------------------------------------------------
		  ("verbose",
		   po::bool_switch(&gVerbose),
		   "Print informational messages during processing.")
		  // -----------------------------------------------------------------
		  ("politely_finish",
		   "Wait for user to press <Enter> before finishing the program.")
		  // -----------------------------------------------------------------
		  ;
	}

	/**
	 * Custom option parser to recognise a single dash, '-', that denotes
	 * input should come from stdin.
	 */
	static std::pair<std::string, std::string> single_dash(const std::string& s__)
	{
		if (s__ == "-")
			return std::make_pair(std::string("stdin"), std::string("true"));
		else
			return make_pair(std::string(), std::string());
	}

	void Program_Base::parse_command_line_options()
	{
		setup_option_descriptions();
		store(
		  po::command_line_parser(argc, argv)
			.options(gCmdLine)
			.positional(gPositional)
			.extra_parser(&single_dash)
			.run(),
		  gVM);
		notify(gVM);
	}

	void Program_Base::print_custom_help()
	{
		using namespace std;
		po::options_description visible;
		visible.add(gGeneric);
		cout << visible << endl;
	}

	void Program_Base::print_help_if_asked()
	{
		using std::cout;
		using std::endl;
		if (gVM.count("help")) {
			cout << endl;
			cout << "Help for " << argv[0] << ',' << ' ' << gVersion << endl
				 << "Copyright (C) 2019 Lester Dowling" << endl
				 << "This software comes with ABSOLUTELY NO WARRANTY. This is free software"
				 << endl
				 << "and you are welcome to modify & distribute it under the GPL license."
				 << endl;
			cout << endl;
			cout
			  << "You can specify input should come from stdin instead of files listed"
			  << endl
			  << "on the command line.  End the command line with a single dash to do this."
			  << endl;
			print_custom_help();
			throw No_Op{}; // Exit program without doing anything.
		}
	}

	void Program_Base::print_version_if_asked()
	{
		using std::cout;
		using std::endl;
		if (gVM.count("version")) {
			cout << boost::dll::program_location().string() << endl;
			cout << gVersion << endl;
			throw No_Op{}; // Exit program without doing anything.
		}
	}

	void Program_Base::politely_finish_if_asked()
	{
		if (gVM.count("politely_finish")) {
			using std::cout;
			using std::endl;
			using std::flush;
			cout << endl << endl << "Please press <Enter> to finish:" << flush;
			getchar();
		}
	}

	void Program_Base::set_out(std::ostream& out)
	{
		gOut = &out;
		gOut->exceptions(
		  std::ios_base::badbit | std::ios_base::failbit | std::ios_base::eofbit);
	}

	void Program_Base::set_err(std::ostream& err)
	{
		gErr = &err;
		gErr->exceptions(
		  std::ios_base::badbit | std::ios_base::failbit | std::ios_base::eofbit);
	}

	void Program_Base::report_exception(
	  const char* const title,
	  const std::string& what) noexcept
	{
		try {
			using std::endl;
			gExitCode = EXIT_FAILURE;
			const char* const prefix_tag = "!!! ";
			*gErr << prefix_tag << boost::dll::program_location().leaf().string() << endl;
			*gErr << prefix_tag << title << " error";
			if (what.empty()) {
				*gErr << '.' << endl;
			} else {
				*gErr << ':' << endl;
				*gErr << prefix_tag << what << endl;
			}
		} catch (...) {
			std::terminate();
		}
	}

	/*static*/ void Program_Base::report_startup_error(
	  const char* const title,
	  std::string const& what) noexcept
	{
		try {
			using std::cerr;
			using std::endl;
			const char* const error_msg_prefix = "!!! ";
			cerr << error_msg_prefix << boost::dll::program_location().leaf().string()
				 << endl;
			cerr << error_msg_prefix << "Failed to start.  " << title << " error" << endl;
			if (what.empty()) {
				cerr << '.' << endl;
			} else {
				cerr << ':' << endl;
				cerr << error_msg_prefix << what << endl;
			}
		} catch (...) {
			/* Ignore.  Cannot do anything but terminate... */
		}
		std::terminate();
	}

	void Program_Base::run()
	{
		print_help_if_asked();
		print_version_if_asked();
		perform_requested_operation();
		save();
	}

	void Program_Base::run_and_report_exceptions() noexcept
	{
		try {
			this->run();
			if (this->gVerbose)
				*gOut << "Okay." << std::endl;
		} catch (operations::No_Op const& ex) { // Nothing for program to do.
			if (0 < std::strlen(ex.what()))
				this->report_exception("No Operation", ex.what());
			else
				this->gExitCode = EXIT_FAILURE;
		} catch (boost::filesystem::filesystem_error const& ex) {
			this->report_exception("File system", ex.what());
		} catch (boost::system::system_error const& ex) {
			this->report_exception("System", ex.what());
		} catch (std::ios_base::failure const& ex) {
			this->report_exception("I/O", ex.what());
		} catch (std::bad_alloc const&) {
			this->report_exception("Out of memory");
		} catch (std::domain_error const& ex) {
			this->report_exception("Domain", ex.what());
		} catch (std::length_error const& ex) {
			this->report_exception("Length", ex.what());
		} catch (std::invalid_argument const& ex) {
			this->report_exception("Invalid argument", ex.what());
		} catch (std::out_of_range const& ex) {
			this->report_exception("Out of range", ex.what());
		} catch (std::range_error const& ex) {
			this->report_exception("Range", ex.what());
		} catch (std::logic_error const& ex) {
			this->report_exception("Logic", ex.what());
		} catch (std::runtime_error const& ex) {
			this->report_exception("Runtime", ex.what());
		} catch (std::exception const& ex) {
			this->report_exception("Exception", ex.what());
		} catch (...) {
			this->report_exception("Unexpected");
		}
	}

} // namespace operations

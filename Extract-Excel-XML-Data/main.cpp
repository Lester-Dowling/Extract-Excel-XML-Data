#include "pch-main.hpp"
#include "Operations/Program.hpp"
#include "config.h"

int main(int argc, char* argv[])
{
	using operations::Program;
	try {
		using namespace std;
		cin.exceptions(ios_base::badbit);
		Program program{ argc, argv, std::cout, std::cerr };
		program.gVersion.major = PROJECT_VERSION_MAJOR;
		program.gVersion.minor = PROJECT_VERSION_MINOR;
		program.gVersion.small = PROJECT_VERSION_PATCH;
		program.run_and_report_exceptions();
		program.politely_finish_if_asked();
		return program.gExitCode;
	} catch (std::bad_alloc const&) {
		Program::report_startup_error("Out of memory");
	} catch (std::exception const& ex) {
		Program::report_startup_error("Exception", ex.what());
	} catch (...) {
		Program::report_startup_error("Unexpected");
	}
}

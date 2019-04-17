#include "pch-unit-tests.hpp"
#include "GlobalFixture.hpp"
#include <date/date.h>
#include <boost/test/unit_test.hpp>
using namespace std;

GlobalFixture::GlobalFixture()
  : m_start_time{ std::chrono::system_clock::now() }
  , env_var{ getenv("FINISH_WITHOUT_USER_PROMPT") }
  , m_finish_without_user_prompt{ string{ env_var == nullptr ? "" : env_var } == "1" }
{
	cin.exceptions(ios_base::badbit | ios_base::failbit);
	cout.exceptions(ios_base::badbit | ios_base::failbit | ios_base::eofbit);
	wcout.exceptions(ios_base::badbit | ios_base::failbit | ios_base::eofbit);
	using namespace date;
	wcout << "Unit testing started at " << m_start_time << endl;
	// namespace a = boost::algorithm;
	// const string env_path{ getenv("PATH") };
	// list<string> env_paths;
	// a::split(env_paths, env_path, boost::is_any_of(";"));
	// for (auto const& p : env_paths) {
	//	if (!p.empty())
	//		cout << "PATH == " << p << endl;
	//}
}


GlobalFixture::~GlobalFixture()
{
	using namespace date;
	const auto m_finish_time = std::chrono::system_clock::now();
	const auto unit_testing_duration = m_finish_time - m_start_time;
	// wcout << "Unit testing finished at " << m_finish_time << endl;
	// wcout << "Unit testing took " << unit_testing_duration << endl;
	if (!m_finish_without_user_prompt) {
		cout << endl << endl << "Please press <Enter> to finish: " << flush;
		getchar();
	}
}

BOOST_GLOBAL_FIXTURE(GlobalFixture);

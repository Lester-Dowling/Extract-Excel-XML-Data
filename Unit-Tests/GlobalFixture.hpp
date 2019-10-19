#pragma once
#include <chrono>
class GlobalFixture // This is invoked pre and post the whole test suite.
{
	const std::chrono::time_point<std::chrono::system_clock> m_start_time;
	const char* const env_var;
	const bool m_finish_with_user_prompt;

public:
	GlobalFixture();
	~GlobalFixture();
};

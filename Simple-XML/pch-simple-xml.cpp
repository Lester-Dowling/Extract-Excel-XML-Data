#include "pch-simple-xml.hpp"


#ifdef BOOST_ENABLE_ASSERT_HANDLER
namespace boost {
	static void assertion_failed(char const* e, char const* fn, char const* f, long l)
	{
		std::ostringstream oss;
		oss << "[Failed Assertion] " << e << std::endl;
		oss << "[Function] " << fn << std::endl;
		oss << "[File] " << f << std::endl;
		oss << "[Line] " << l << std::endl;
		throw std::runtime_error{ oss.str() };
	}
} // namespace boost
#endif // BOOST_ENABLE_ASSERT_HANDLER

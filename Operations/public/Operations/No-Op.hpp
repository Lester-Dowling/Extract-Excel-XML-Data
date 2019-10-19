/**
 * @file   Operations/No-Op.hpp
 * @date   Started Wed 02 Mar 2011 12:02:06 PM EST
 * @author Lester J. Dowling
 */
#pragma once
#include <stdexcept>
namespace operations {

	/*
	 * An exception to indicate there's no operation to perform.
	 */
	class No_Op : public std::exception {
	public:
		No_Op()
		  : std::exception()
		{
		}

		// No_Op(const char* what)
		// : std::exception(what)
		// {
		//}
	};
} // namespace operations

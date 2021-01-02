/*

*/



#pragma once



#include "LAL_Cpp_STL.hpp"



namespace LAL
{
	using RuntimeError = std::runtime_error;



	enum class EExitCode
	{
		Success  = EXIT_SUCCESS,
		Failure  = EXIT_FAILURE
	};



	class NotImplementedException : public std::logic_error
	{
	public:
		NotImplementedException () : std::logic_error{ "Function not yet implemented." } {}
	};
}

/*

*/



#pragma once



#include "LAL_Cpp_STL.hpp"



namespace LAL
{
	using RuntimeError = STL::runtime_error;



	enum class EExitCode
	{
		Success  = EXIT_SUCCESS,
		Failure  = EXIT_FAILURE
	};



	class NotImplementedException : public STL::logic_error
	{
	public:
		NotImplementedException () : STL::logic_error{ "Function not yet implemented." } {}
	};
}

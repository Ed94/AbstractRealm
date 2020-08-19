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
}

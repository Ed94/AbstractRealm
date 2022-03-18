/*

*/


#pragma once


#include "Meta/Config/CoreDev_Config.hpp"
#include "LAL_Cpp_STL.hpp"


namespace LAL
{
#ifdef Meta_UseCpp_Exceptions
	using RuntimeError = STL::runtime_error;

	class NotImplementedException : public STL::logic_error
	{
	public:
		NotImplementedException() : STL::logic_error{ "Function not yet implemented." } {}
	};
#endif

	enum class EExitCode
	{
		Success  = EXIT_SUCCESS,
		Failure  = EXIT_FAILURE
	};
}

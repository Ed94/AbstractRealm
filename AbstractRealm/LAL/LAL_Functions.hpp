#pragma once



#include "LAL_Cpp_STL.hpp"




namespace LAL
{
	#ifndef In

		// Specifies the function declarer will provide the parameter's value.
		#define In

	#endif
	#ifndef Out

		// Specifies the function will provide the parameter value.
		#define Out

	#endif
		
	template<class FunctionType>
	using Function = std::function<FunctionType>;
}

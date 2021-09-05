#pragma once


#include "LAL_Cpp_STL.hpp"


namespace LAL
{
	template<class FunctionType>
	using Function = STL::function<FunctionType>;

	template<class ReferenceType>
	using WRef = STL::reference_wrapper<ReferenceType>;
}

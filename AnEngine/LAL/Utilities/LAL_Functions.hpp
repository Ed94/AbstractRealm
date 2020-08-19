#pragma once



#include "LAL_Cpp_STL.hpp"



namespace LAL
{
	template<class FunctionType>
	using Function = std::function<FunctionType>;
}

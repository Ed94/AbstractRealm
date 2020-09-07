/*


*/



#pragma once



#include "LAL_Cpp_STL.hpp"
#include "LAL_Types.hpp"



namespace LAL
{
	template<typename Type>
	using DynamicArray = std::vector<Type>;

	template<typename Type>
	using Queue = std::queue<Type>;

	template<typename Type>
	using Deque = std::deque<Type>;

	template<typename Type, WordSize NumberOfElements>
	using StaticArray = std::array<Type, NumberOfElements>;
}

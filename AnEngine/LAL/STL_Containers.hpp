/*


*/



#pragma once



#include "Cpp_STL.hpp"
#include "STL_BasicTypes.hpp"




namespace LAL
{
	template<typename Type>
	using Queue = std::queue<Type>;

	template<typename Type, DataSize NumberOfElements>
	using StaticArray = std::array<Type, NumberOfElements>;

	template<typename Type>
	using DynamicArray = std::vector<Type>;
}

/*


*/



#pragma once



#include "LAL_Cpp_STL.hpp"
#include "LAL_Types.hpp"



namespace LAL
{
	template<typename Type>
	using DynamicArray = STL::vector<Type>;

	template<typename Type>
	using Queue = STL::queue<Type>;

	template<typename Type>
	using Deque = STL::deque<Type>;

	template<typename KeyType, typename ValueType>
	using Map = STL::map<KeyType, ValueType>;

	template<typename KeyType, typename ValueType>
	using UnorderedMap = STL::unordered_map<KeyType, ValueType>;

	template<typename Type, uDM NumberOfElements>
	using StaticArray = STL::array<Type, NumberOfElements>;
}

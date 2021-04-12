/*
STL Basic Types

Last Modified: 5/18/2020
*/



#pragma once



#include "LAL_Cpp_STL.hpp"



namespace LAL
{
	// Data

	using Byte = std::byte;

	using uDM = std::size_t;   // Defined according to the target processor's pointer arithmetic capabilities (Not memory capabilities).

	using PtrDiff  = std::ptrdiff_t;
	using sIntPtr  = std::intptr_t ;
	using uIntPtr  = std::uintptr_t;

	// Pairs and Tuples

	using std::get;

	template<typename TypeA, typename TypeB>
	using Pair = std::pair<TypeA, TypeB>;

	//template<typename TypeA, typename TypeB>
	//Pair<TypeA, TypeB> MakePair(TypeA&& _typeA, TypeB&& _typeB) { return std::make_pair<TypeA, TypeB>; }

	using std::make_pair;

	template<typename... Types>
	using Tuple = std::tuple<Types...>;
}

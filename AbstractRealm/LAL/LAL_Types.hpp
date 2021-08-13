/*
STL Basic Types

Last Modified: 5/18/2020
*/



#pragma once



#include "LAL_Cpp_STL.hpp"



namespace LAL
{
	// Data

	using Byte = STL::byte;

	using uDM = STL::size_t;   // Defined according to the target processor's pointer arithmetic capabilities (Not memory capabilities).

	using PtrDiff  = STL::ptrdiff_t;
	using sIntPtr  = STL::intptr_t ;
	using uIntPtr  = STL::uintptr_t;

	// Pairs and Tuples

	using STL::get;

	template<typename TypeA, typename TypeB>
	using Pair = STL::pair<TypeA, TypeB>;

	//template<typename TypeA, typename TypeB>
	//Pair<TypeA, TypeB> MakePair(TypeA&& _typeA, TypeB&& _typeB) { return std::make_pair<TypeA, TypeB>; }

	using STL::make_pair;

	template<typename... Types>
	using Tuple = STL::tuple<Types...>;
}

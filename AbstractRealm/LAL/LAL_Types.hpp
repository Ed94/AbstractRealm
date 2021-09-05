/*
STL Basic Types
*/


#pragma once


#include "LAL_Cpp_STL.hpp"


namespace LAL
{
	// Data

	template<typename... Type>
	using VoidType = STL::void_t<Type...>;

	using Byte = STL::byte;

	using uDM = STL::size_t;   // Defined according to the target processor's pointer arithmetic capabilities (Not memory capabilities).

	using ptrDiff  = STL::ptrdiff_t;
	using sIntPtr  = STL::intptr_t ;
	using uIntPtr  = STL::uintptr_t;

	// Pairs and Tuples

	using STL::get;

	template<typename TypeA, typename TypeB>
	using Pair = STL::pair<TypeA, TypeB>;

	template<class TypeA, class TypeB>
	constexpr auto MakePair = STL::make_pair<TypeA, TypeB>;

	template<typename... Types>
	using Tuple = STL::tuple<Types...>;
}

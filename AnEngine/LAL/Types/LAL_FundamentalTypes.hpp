/*
C++ STL Fundamental Types

Last Modified: 5/18/2020
*/



#pragma once



#include "LAL_Cpp_STL.hpp"
#include "LAL_Reflection.hpp"



namespace LAL
{
	// Types

	// Integers

	// Flexible

	using uint32 = unsigned int;
	using sint32 = signed   int;


	// Strict

	// Signed
	using sInt8  = signed           char;
	using sInt16 = signed short     int ;
	using sInt32 = signed long      int ;
	using sInt64 = signed long long int ;

	// Unsigned

	using uInt8  = unsigned            char;
	using uInt16 = unsigned short      int ;
	using uInt32 = unsigned long       int ;
	using uInt64 = unsigned long long  int ;

	// Floats

	using float32 =      float ;
	using float64 =      double;

	// Data

	using DataSize = std::size_t;
	using sIntPtr  = std::intptr_t;
	using uIntPtr  = std::uintptr_t;


	// Functions

	using std::signbit;

	template<typename Type>
	constexpr bool IsInt()
	{
		return IsSameType<Type, sInt8 >() || 
			   IsSameType<Type, sInt16>() || 
			   IsSameType<Type, sInt32>() || 
			   IsSameType<Type, sInt64>() ||
			   IsSameType<Type, uInt8 >() ||
			   IsSameType<Type, uInt16>() ||
			   IsSameType<Type, uInt32>() ||
			   IsSameType<Type, uInt64>()   ;
	}

	template<typename Type>
	constexpr bool IsFloat()
	{
		return IsSameType<Type, float32>() || IsSameType<Type, float64>();
	}
}
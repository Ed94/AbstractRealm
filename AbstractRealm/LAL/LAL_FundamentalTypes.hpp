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

	using ui16 = unsigned int;
	using si16 = signed   int;
	using ui32 = unsigned int;
	using si32 = signed   int;

	// Strict

	// Signed
	using sI8  = signed           char;
	using sI16 = signed short     int ;
	using sI32 = signed long      int ;
	using sI64 = signed long long int ;

	// Unsigned

	using uI8  = unsigned            char;
	using uI16 = unsigned short      int ;
	using uI32 = unsigned long       int ;
	using uI64 = unsigned long long  int ;

	// Floats

	using f32 = float ;
	using f64 = double;



	// Functions

	using std::signbit;

	template<typename Type>
	constexpr bool IsInt()
	{
		return IsSameType<Type, sI8 >() || 
			   IsSameType<Type, sI16>() || 
			   IsSameType<Type, sI32>() || 
			   IsSameType<Type, sI64>() ||
			   IsSameType<Type, uI8 >() ||
			   IsSameType<Type, uI16>() ||
			   IsSameType<Type, uI32>() ||
			   IsSameType<Type, uI64>()   ;
	}

	template<typename Type>
	constexpr bool IsFloat()
	{
		return IsSameType<Type, f32>() || IsSameType<Type, f64>();
	}


	// Literals

	constexpr uI16 operator"" _uI16 (unsigned long long _uI16)
	{
		return uI16(_uI16);
	}

	constexpr short operator"" _s (unsigned long long _s)
	{
		return short(_s);
	}
}
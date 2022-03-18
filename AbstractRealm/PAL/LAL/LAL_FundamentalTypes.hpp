/*
C++ STL Fundamental Types
*/


#pragma once


#include "LAL_Cpp_STL.hpp"
#include "LAL_Reflection.hpp"


namespace LAL
{
	// Types

	// Integers

	// Flexible

	using ui32 = unsigned int;
	using si32 = signed   int;

	// Strict

	// Signed
	using s8  = signed           char;
	using s16 = signed short     int ;
	using s32 = signed long      int ;
	using s64 = signed long long int ;

	// Unsigned

	using u8  = unsigned            char;
	using u16 = unsigned short      int ;
	using u32 = unsigned long       int ;
	using u64 = unsigned long long  int ;

	// Floats

	using f32 = float ;
	using f64 = double;


	// Functions

	using STL::signbit;

	// Literals

	constexpr u16 operator"" _uI16 (unsigned long long _uI16)
	{
		return u16(_uI16);
	}

	constexpr short operator"" _s (unsigned long long _s)
	{
		return short(_s);
	}
}

/*
Character Types

Contains type definitions for commonly used raw character types (such as raw char strings),
*/



#pragma once


#include "LAL_Cpp_STL.hpp"


namespace LAL
{
	using CStr   =       char*;
	using RoCStr = const char*;

	using CStr16   =       wchar_t*;
	using RoCStr16 = const wchar_t*;


	constexpr auto CStrCompare = strcmp;
}

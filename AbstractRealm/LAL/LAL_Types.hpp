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
}

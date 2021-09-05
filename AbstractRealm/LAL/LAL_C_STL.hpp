/*
Due to the nature of C STL headers, they cannot be included 
in a header without polluting a namespace.

Wrapping the namespace is also not always possible.
*/


#pragma once


#ifndef LAL_C_STL_HEADER_GUARD
#define LAL_C_STL_HEADER_GUARD

#ifdef _WIN32 
#ifdef LAL_Avoid_WindowsIssues
		#define NOMINMAX              // Prevents the numeric limits error.
		#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif
#endif

// C Libraries
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#endif // LAL_C_STL_HEADER_GUARD


namespace LAL
{

}


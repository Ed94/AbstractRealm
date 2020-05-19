/*
Language Abstraction Layer

Last Modified: 5/18/2020

This is a all around wrapping of C++/STL features into a format I perfer. 
Since I'm writting an entire engine, having an LAL to make things more efficent and less error prone,
by havng more explicit format for dealing with syntax makes things easier.

Eventually I would like to make a parser plugin that would auto covert file to either LAL wrap or
standard c++.
*/



#pragma once



#include "Cpp_STL.hpp"
#include "Casting.hpp"
#include "Explicit_Specifiers.hpp"
#include "Types/STL_BasicTypes.hpp"
#include "Types/STL_FundamentalTypes.hpp"
#include "Types/STL_FundamentalLimits.hpp"
#include "Utilities/STL_SmartPtrs.hpp"
#include "STL_Memory.hpp"
#include "STL_Reflection.hpp"



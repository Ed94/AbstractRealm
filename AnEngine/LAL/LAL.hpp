/*
Language Abstraction Layer

Last Modified: 5/18/2020

This is a all around wrapping of C++/STL features into a format I prefer. 
Since I'm witting an entire engine, having an LAL to make things more efficient and less error prone,
by having more explicit format for dealing with syntax makes things easier.

Eventually I would like to make a parser plugin that would auto covert file to either LAL wrap or
standard c++.
*/



#pragma once


#include "Cpp_STL.hpp"
#include "Casting.hpp"
#include "ConventionEnforcer.hpp"
#include "Declarations.hpp"
#include "Types/STL_BasicTypes.hpp"
#include "Types/Bitmask.hpp"
#include "Types/CharacterTypes.hpp"
#include "Types/STL_FundamentalTypes.hpp"
#include "Types/STL_FundamentalLimits.hpp"
#include "Utilities/STL_Ambiguous.hpp"
#include "Utilities/STL_SmartPtrs.hpp"
#include "STL_Memory.hpp"
#include "STL_Reflection.hpp"
#include "STL_IO.hpp"
#include "STL_Containers.hpp"
#include "STL_String.hpp"
#include "STL_Exceptions.hpp"
#include "Utilities/STL_Functions.hpp"



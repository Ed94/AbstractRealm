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



#include "LAL_Cpp_STL.hpp"
#include "LAL_Algorithms.hpp"
#include "LAL_Ambiguous.hpp"
#include "LAL_Bitmask.hpp"
#include "LAL_Casting.hpp"
#include "LAL_CharacterTypes.hpp"
#include "LAL_ConventionEnforcer.hpp"
#include "LAL_Declarations.hpp"
#include "LAL_FundamentalTypes.hpp"
#include "LAL_FundamentalLimits.hpp"
#include "LAL_SmartPtrs.hpp"
#include "LAL_Memory.hpp"
#include "LAL_Reflection.hpp"
#include "LAL_IO.hpp"
#include "LAL_Containers.hpp"
#include "LAL_String.hpp"
#include "LAL_Exceptions.hpp"
#include "LAL_Functions.hpp"
#include "LAL_Chrono.hpp"
#include "LAL_Thread.hpp"
#include "LAL_Types.hpp"

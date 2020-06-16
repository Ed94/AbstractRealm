/*
Unit

Last Modified: 5/18/2020
*/



#pragma once



// Includes

#include "LAL/LAL.hpp"

#include "Meta/Specification.hpp"
#include "Meta/Config/Simulation.hpp"



namespace Sim
{
	// Alias

	using LAL ::Choose     ;
	using Meta::BitAccuracy;

	template<BitAccuracy _accuracy> using DecN_Choose = 
	Choose
	<Meta::Is<BitAccuracy::_32_Bit>(_accuracy), 
		
		LAL::float32, 
		LAL::float64
	>;

	/*
	Used to represent a real number using a float-point approximation.

	The accuracy is dependent on what accuracy was specified in the Meta flags for the engine.
	*/
	using DecN = DecN_Choose< Meta::DecN_Accuracy >;

	template<BitAccuracy _accuracy> using IntN_Choose =
	Choose
	<Meta::Is<BitAccuracy::_32_Bit>(_accuracy),

		LAL::sInt32,
		LAL::sInt64
	>;

	/*
	Used to represent a real number using a integer fundamental type approximation.

	The accuracy is dependent on what accuracy was specified in the Meta flags for the engine.
	*/
	using IntN = IntN_Choose< Meta::IntN_Accuracy >;
}

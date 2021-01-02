/*
Unit

Last Modified: 5/18/2020
*/



#pragma once



// Includes

#include "LAL/LAL.hpp"

#include "Meta/Specification.hpp"
#include "Meta/Config/Simulation_Config.hpp"



namespace Sim
{
	// Alias

	using LAL ::ChooseEither     ;
	using Meta::BitAccuracy;

	template<BitAccuracy _accuracy> using DecN_Choose = 
	ChooseEither
	<Meta::Is<BitAccuracy::_32_Bit>(_accuracy), 
		
		LAL::f32, 
		LAL::f64
	>;

	/*
	Used to represent a real number using a float-point approximation.

	The accuracy is dependent on what accuracy was specified in the Meta flags for the engine.
	*/
	using DecN = DecN_Choose< Meta::DecN_Accuracy >;

	template<BitAccuracy _accuracy> using IntN_Choose =
	ChooseEither
	<Meta::Is<BitAccuracy::_32_Bit>(_accuracy),

		LAL::sI32,
		LAL::sI64
	>;

	/*
	Used to represent a real number using a integer fundamental type approximation.

	The accuracy is dependent on what accuracy was specified in the Meta flags for the engine.
	*/
	using IntN = IntN_Choose< Meta::IntN_Accuracy >;
}

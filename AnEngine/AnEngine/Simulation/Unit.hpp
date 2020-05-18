#pragma once



#include "Meta/Specification.hpp"
#include "Meta/Config/Flags.hpp"
#include "LAL/LAL.hpp"



namespace Sim
{
	using LAL::Choose;

	using Meta::BitAccuracy;

	template<BitAccuracy _accuracy> using FloatN_Choose = 
	Choose
	<Meta::Is<BitAccuracy::_32_Bit>(_accuracy), 
		
		LAL::float32, 
		LAL::float64
	>;

	/*
	Used to represent a real number using a float-point approximation.

	The accuracy is dependent on what accuracy was specified in the Meta flags for the engine.
	*/
	using FloatN = FloatN_Choose< Meta::Sim_FloatN_Accuracy() >;
}

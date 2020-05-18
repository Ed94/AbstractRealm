/*
Macro Flags

Last Modified: 5/20/2020
*/



#pragma once


#include "Specification.hpp"


// Accuarcy specified for FloatN type used in simulation real number units.
#define SIM_FLOATN_ACCURACY \
BitAccuracy::_32_Bit


namespace Meta
{
	constexpr BitAccuracy Sim_FloatN_Accuracy()
	{
		return SIM_FLOATN_ACCURACY;
	}
}

/*
Macro Flags

Last Modified: 5/20/2020
*/



#pragma once



#include "Specification.hpp"

// Will use a more accurate epsilon for math floating-point operations.
#define SIM_DecN_USE_HIGH_ACCURACY_COMPARE \
true

// Only if using a type higher than FP32.
#define SIM_DecN_USE_NANO_ACCURACY_COMPARE \
false

// Accuarcy specified for DecN type used in simulation's real number units.
#define SIM_DecN_ACCURACY \
BitAccuracy::_32_Bit

// Will use a more accurate epsilon for math floating-point operations.
#define SIM_DecN_USE_HIGH_ACCURACY_UNIT \
true

// Only if using a type higher than FP32.
#define SIM_DecN_USE_HIGH_ACCURACY_UNIT \
false

// Accuracy specified for IntN type used in simulation's integer number units. (Will use nano-accurate epsilon)
#define SIM_IntN_ACCURACY \
BitAccuracy::_64_Bit



namespace Meta
{
	constexpr bool Sim_DecN_UseHighAccuracy_Compare()
	{
		return SIM_DecN_USE_HIGH_ACCURACY_COMPARE;
	}

	constexpr bool Sim_DecN_UseNanoAccuracy_Compare()
	{
		return SIM_DecN_USE_NANO_ACCURACY_COMPARE
	}

	constexpr BitAccuracy Sim_DecN_Accuracy()
	{
		return SIM_DecN_ACCURACY;
	}

	constexpr bool Sim_DecN_UseHighAccuracy_Unit()
	{
		return SIM_DecN_USE_HIGH_ACCURACY_UNIT;
	}

	constexpr bool Sim_DecN_UseNanoAccuracy_Unit()
	{
		return SIM_DecN_USE_HIGH_ACCURACY_UNIT;
	}

	constexpr BitAccuracy Sim_IntN_Accuracy()
	{
		return SIM_IntN_ACCURACY;
	}
}

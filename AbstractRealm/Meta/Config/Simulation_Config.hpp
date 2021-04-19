/*
Macro Flags

Last Modified: 5/20/2020


For now I'm using macro level definitions for these and the accuracy is determined at compile time.

The reason for this is guess less complexity in implementation for now instead of having a runtime abstraction interface
that can handle multiple different datatype accuracies. (As well as conversion between them)
*/



#pragma once



#include "Meta/Specification.hpp"

// Will use a more accurate epsilon for math floating-point operations.
//#define SIM_DecN_USE_HIGH_ACCURACY_COMPARE \
true

// Only if using a type higher than FP32.
//#define SIM_DecN_USE_NANO_ACCURACY_COMPARE \
false

// Accuracy specified for DecN type used in simulation's real number units.
//#define SIM_DecN_ACCURACY \
BitAccuracy::_32_Bit

// Will use a more accurate epsilon for math floating-point operations.
//#define SIM_DecN_USE_HIGH_ACCURACY_UNIT \
true

// Only if using a type higher than FP32.
//#define SIM_DecN_USE_NANO_ACCURACY_UNIT \
false

// Accuracy specified for IntN type used in simulation's integer number units. (Will use nano-accurate epsilon)
//#define SIM_IntN_ACCURACY \
BitAccuracy::_64_Bit



namespace Meta
{
	enum class ECompareAccuracy
	{
		Low ,
		High,
		Nano
	};


	// Compile-time

	constexpr ECompareAccuracy DecN_CompareAccuracy = ECompareAccuracy::High;
	constexpr ECompareAccuracy DecN_UnitAccuracy    = ECompareAccuracy::High;

	constexpr BitAccuracy IntN_Accuracy = BitAccuracy::_64_Bit;
	constexpr BitAccuracy DecN_Accuracy = BitAccuracy::_32_Bit;
}

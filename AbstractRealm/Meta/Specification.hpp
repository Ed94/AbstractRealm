/*
Specification
*/


#pragma once


#include "LAL.hpp"


// Macros

// Macro: EComplationIntention:

#define Meta_Build_Tuned \
	EBuild::Tuned

#define Meta_Build_Tuned_WithDebug \
	EBuild::Tuned_WithDebug

#define Meta_Build_Project \
	EBuild::Project

#define Meta_Build_User \
	EBuild::User

#define Meta_Build_User_WithDebug \
	EBuild::User_WithDebug

#define Meta_Build_Development \
	EBuild::EngineDevelopment

// End of Macro: EComplationIntention

/*
Macro version of the compilation intention.
*/
#define Meta_BuildType \
	Meta_Build_Development


namespace Meta
{
	using namespace LAL;

	// Enums

	enum class BitAccuracy
	{
		_8__Bit            = 0,
		_16_Bit            = 1,
		_32_Bit            = 2,
		_64_Bit            = 3,
		_ExtendedPrecision = 4,
	};

	enum class EBuild
	{
		// Build for a packaged project
		Tuned,

		// Builds for packaged project with tooling for debug and profiling.
		Tuned_WithDebug,

		/*
		Builds for the engine user with debug and profiling tools only for the project level.
		*/
		User,

		/*
		Builds for engine user but with debug and profiling tools available.
		*/
		User_WithDebug,

		/*
		Builds for engine development.
		*/
		EngineDevelopment
	};


	// Compile-time

	template<BitAccuracy _accuracyT>
	constexpr bool Is(BitAccuracy _accuracy)
	{
		return _accuracy == _accuracyT ? true : false;
	}

	template<BitAccuracy _accuracy>
	using Choose_FloatAccuracy =
	ChooseEither
	<Meta::Is<BitAccuracy::_32_Bit>(_accuracy),
		f32, 
		f64
	>;

	constexpr EBuild BuildType = Meta_BuildType;

	constexpr bool BuildIsNonTuned = BuildType > EBuild::Tuned_WithDebug;
}

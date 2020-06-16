/*
Specification

Last Modified: 5/18/2020
*/



#pragma once




// Macros

// Macro: EComplationIntention:

#define Meta_CompileFor_User \
	ECompilationIntention::User

#define Meta_CompileFor_User_WithDebug \
	ECompilationIntention::User_WithDebug

#define Meta_CompileFor_Development	\
	ECompilationIntention::Development

// End of Macro: EComplationIntention

/*
Macro version of the compilation intention.
*/
#define Meta_BuildType \
	Meta_CompileFor_Development





namespace Meta
{
	// Enums

	enum class BitAccuracy
	{
		_8__Bit            = 0,
		_16_Bit            = 1,
		_32_Bit            = 2,
		_64_Bit            = 3,
		_ExtendedPrecision = 4,
	};

	enum class ECompilationIntention
	{
		/*
		Builds for the user with tooling.
		*/
		User,

		/*
		Builds for user but with debug tools available.
		*/
		User_WithDebug,

		/*
		Builds for development.
		*/
		Development
	};


	// Compile-time

	template<BitAccuracy _accuracyT>
	constexpr bool Is(BitAccuracy _accuracy)
	{
		return _accuracy == _accuracyT ? true : false;
	}

	constexpr ECompilationIntention BuildType = Meta_BuildType;
}

#pragma once



// Engine

#include "LAL/LAL.hpp"



namespace Meta
{
	// Usings

	using namespace LAL;



	// Structs

	struct AppVersion
	{
		const uInt32 Major, Minor, Patch;
	};
}

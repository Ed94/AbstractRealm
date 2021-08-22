#pragma once


#include "LAL/LAL.hpp"


namespace Meta
{
	using LAL::u32;


	// Structs

	struct AppVersion
	{
		const u32 Major, Minor, Patch;
	};
}

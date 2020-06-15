#pragma once



#include "LAL/LAL.hpp"



namespace Core::Meta
{
	using namespace LAL;

	struct AppVersion
	{
		const uInt32 Major, Minor, Patch;
	};
}

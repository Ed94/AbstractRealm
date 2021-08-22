#pragma once


#include "Meta/Meta.hpp"


Meta_SetupEngineModule(Memory);


namespace Memory
{
	using LAL::FormatByFill;
	using LAL::FormatWithData;

	void LoadBackend();
}

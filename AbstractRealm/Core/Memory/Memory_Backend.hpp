#pragma once


#include "Meta/Meta.hpp"


Meta_SetupEngineModule(Memory);


namespace Memory
{
	using LAL::Format_ByFill;
	using LAL::Format_WithData;

	void LoadBackend();
}

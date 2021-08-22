#pragma once

#define Meta_EngineModule
#include "Meta/Meta.hpp"

Meta_SetupEngineModule(Core);


// Temp
#include "SAL/SAL_ImGui.hpp"


//Meta_SetupSubmodule(Core, Dev);

namespace Core
{
	void LoadBackend();
}


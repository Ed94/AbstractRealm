#pragma once


#include "Meta/Meta.hpp"
Meta_SetupModule(Core);


#include "LAL.hpp"
#include "OSAL.hpp"

// Temp
#include "SAL/SAL_ImGui.hpp"


//Meta_SetupSubmodule(Core, Dev);

namespace Core
{
	void LoadBackend();
}


#pragma once

#define Meta_EngineModule
#include "Meta.hpp"

Meta_SetupEngineModule(Concurrency);


namespace Concurrency 
{
	// Functions

	void Load();
	void Unload();

	void Record_EditorDevDebugUI();
}

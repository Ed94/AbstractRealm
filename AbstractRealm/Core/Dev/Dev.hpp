#pragma once


// Engine
#include "Dev/Console.hpp"
#include "Dev/Log.hpp"


namespace Dev
{
	Dev_Declare_LogAPI();

	// Functions

	void Record_EditorDevDebugUI();

	void Load();
	void Unload();
}

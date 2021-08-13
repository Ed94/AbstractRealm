// Parent
#include "Concurrency.hpp"

// Engine
#include "SAL/SAL_Imgui.hpp"


namespace Concurrency
{
#pragma region Editor
	
	void Record_EditorDevDebugUI()
	{
		using namespace SAL::Imgui;

		if (CollapsingHeader("Concurrency"))
		{
			
		}
	}

#pragma endregion Editor

#pragma region Fn

	Dev_Declare_Log(Concurrency);

	void Load()
	{
		SubLogger.Init();

		Log("Loading module.");

		SAL::Imgui::Queue("Dev Debug", Record_EditorDevDebugUI);

		Log("Module loaded.");
	}

	void Unload()
	{
		Log("Unloading module...");
	}
	
#pragma endregion Fn
}
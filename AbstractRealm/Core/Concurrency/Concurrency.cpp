// Parent
#include "Concurrency.hpp"

Meta_SetupEngineModule_Implementation(Concurrency);


#include "SAL/SAL_Imgui.hpp"


namespace Concurrency
{
#pragma region Editor
	
	void Record_EditorDevDebugUI()
	{
		using namespace SAL::Imgui;
	}

#pragma endregion Editor

#pragma region Fn

	void Load()
	{
		Meta_EngineModule_Init();

		Log("Loading module.");

		//SAL::Imgui::Queue("Dev Debug", Record_EditorDevDebugUI);

		Log("Module loaded.");
	}

	void Unload()
	{
		Log("Unloading module...");
	}
	
#pragma endregion Fn
}
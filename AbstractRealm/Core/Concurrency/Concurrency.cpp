// Parent
#include "Concurrency.hpp"


#include "Concurrency_Backend.hpp"


namespace Concurrency
{
#pragma region Fn

	void Load()
	{
		Load_Backend();

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
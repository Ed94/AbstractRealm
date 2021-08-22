// Parent Header
#include "Dev.hpp"

// PAL
#include "SAL/SAL_Imgui.hpp"

#include "Dev_Backend.hpp"


namespace Dev
{
	// Alias

	using namespace LAL;

	void Record_EditorDevDebugUI()
	{
		using namespace SAL::Imgui;

		Console_Record_EditorDevDebugUI();

		/*if (CollapsingHeader("Dev"))
		{
		}*/
	}


	// Fn

	// Public

	void Load()
	{
		Logger::GlobalInit();

		LoadBackend();

		Log("Loading module.");

		Load_DevConsole();

		//SAL::Imgui::Queue("Dev Debug", Record_EditorDevDebugUI);

		Logger::Queue_DebugUI();

		Log("loaded.");
	}

	void Unload()
	{
		Log("Unloading module...");

		Unload_DevConsole();
	}
}
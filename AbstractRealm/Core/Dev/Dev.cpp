// Parent Header
#include "Dev.hpp"

#include "TPAL/TPAL_Imgui.hpp"
#include "Dev_Backend.hpp"


namespace Dev
{
	// Alias

	using namespace LAL;

	void Record_EditorDevDebugUI()
	{
		using namespace TPAL::Imgui;

		Console_Record_EditorDevDebugUI();
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
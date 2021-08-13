// Parent Header
#include "Dev.hpp"


// Engine
#include "Core_Backend.hpp"


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

	Dev_Declare_Log(Dev);
	
	void Load()
	{
		SubLogger.Init();

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
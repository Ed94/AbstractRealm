// Parent Header
#include "Dev.hpp"



// Engine
#include "LAL.hpp"
#include "Console.hpp"
#include "ImGui_SAL.hpp"



namespace Dev
{
	// Usings

	using namespace LAL;

	void Record_EditorDevDebugUI()
	{
		using namespace SAL::Imgui;

		if (CollapsingHeader("Dev"))
		{
			Console_Record_EditorDevDebugUI();
		}
	}



	// Public

	void Load()
	{
		cout << "Dev: Load Module" << endl;

		Load_DevConsole();

		SAL::Imgui::Queue("Dev Debug", Record_EditorDevDebugUI);

		CLog("Dev: Module loaded");
	}

	void Unload()
	{
		CLog("Dev: Unloading module...");

		Unload_DevConsole();
	}
}
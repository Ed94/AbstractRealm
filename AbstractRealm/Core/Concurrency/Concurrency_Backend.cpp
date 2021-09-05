// Parent
#include "Concurrency_Backend.hpp"


Meta_SetupEngineModule_Implementation(Concurrency);


#include "TPAL/TPAL_Imgui.hpp"


namespace Concurrency
{
#pragma region Editor

	void Record_EditorDevDebugUI()
	{
		using namespace TPAL::Imgui;
	}

	void Load_Backend()
	{
		Meta_EngineModule_Init();
	}

#pragma endregion Editor
}

// Parent Header
#include "Executer.hpp"


#include "Meta/Meta_Backend.hpp"
#include "Meta/AppInfo.hpp"

#include "PAL/PAL.hpp"
	#include "HAL.hpp"
	#include "HAL/HAL_GPU.hpp"
	#include "TPAL/TPAL_ImGui.hpp"

#include "Core.hpp"
	#include "Concurrency/CyclerPool.hpp"
	#include "Cycler.hpp"
	#include "MasterExecution.hpp"

#include "Execution_Backend.hpp"

#include "Renderer/Renderer.hpp"


namespace Execution
{
	// Usings

	using namespace LAL ;
	using namespace Meta;
	using namespace TPAL;

	// Aliases

	using OSAL::Window               ;
	using OSAL::WindowInfo           ;
	using OSAL::FrameBufferDimensions;


#pragma region StaticData
		ptr<Window> EngineWindow;
#pragma endregion StaticData
	

	// Public

	OSAL::ExitValT EntryPoint()
	{
		Meta::LoadModule();

		Load_Backend();

		if (UseDebug())
		{
			if (! OSAL::CreateConsole()) 
				return 0;

			cout << "Initializing Dev Module" << endl;

			Dev::Load();
		}

		Dev::Console_EnableAutoUpdate();

		Log("Initiating");

		PAL ::Load();
		Core::Load();

		Renderer::Load();

		// Editor
		// Later on set this to use its own window, and make sure the regular console is disabled while active.
		Imgui::Initialize(Renderer::EngineWindow());

		// Master execution

		Initialize_MasterCycler();

		Dev::Console_EnableAutoUpdate();

		// App closing

		Imgui::Deinitialize();

		Renderer::Unload();	

		Core::Unload();
		PAL ::Unload();

		if (UseDebug())
		{
			OSAL::DestroyConsole();

			Dev::Unload();
		}

		return OSAL::ExitValT(EExitCode::Success);
	}
}


// OSAL Entrypoint:  Setting function overload.

namespace OSAL
{
	OSAL::ExitValT EntryPoint()
	{
		return Execution::EntryPoint();
	}
}


// Parent Header
#include "Executer.hpp"



// Engine
#include "Cycler.hpp"
#include "Meta/Meta.hpp"
#include "Meta/AppInfo.hpp"
#include "HAL.hpp"
#include "HAL/GPU_HAL.hpp"
#include "MasterExecution.hpp"
#include "Concurrency/CyclerPool.hpp"
#include "ImGui_SAL.hpp"
#include "PAL/PAL.hpp"
#include "Core.hpp"
#include "Renderer/Renderer.hpp"



namespace Core::Execution
{
	// Usings

	using namespace LAL ;
	using namespace Meta;
	using namespace SAL ;



	// Aliases

	using OSAL::Window               ;
	using OSAL::WindowInfo           ;
	using OSAL::FrameBufferDimensions;



	namespace StaticData 
	{
		ptr<Window> EngineWindow;
	}

	StaticData()

		



	// Private

	void CLog(String _info)
	{
		Dev::CLog("Core-Execution: " + _info);
	}

	void CLog_Error(String _info)
	{
		Dev::CLog_Error("Core-Execution: " + _info);
	}

	

	// Public

	OSAL::ExitValT EntryPoint()
	{
		Meta::LoadModule();

		try
		{
			if (UseDebug())
			{
				using namespace LAL;

				if (!OSAL::CreateConsole()) return 0;

				cout << "Initializing Dev Module" << endl;

				Dev::Load();
			}

			Dev::Console_EnableAutoUpdate();

			std::cout.precision(10);

			Dev::CLog("Core-Execution: Initiating");

			PAL::Load();

			Core::Load();

			Renderer::Load();

			Imgui::Initialize(Renderer::EngineWindow());

			// Master execution

			Initialize_MasterCycler();

			Dev::Console_EnableAutoUpdate();

			// App closing

			Imgui::Deinitialize();

			Renderer::Unload();	

			OSAL::Unload();

			if (UseDebug())
			{
				OSAL::DestroyConsole();

				Dev::Unload();
			}
		}
		catch (std::exception e)
		{
			CLog_Error(e.what());

			Dev::Console_UpdateBuffer();
		}

		return OSAL::ExitValT(EExitCode::Success);
	}
}



// OSAL Entrypoint:  Setting function overload.

namespace OSAL
{
	OSAL::ExitValT EntryPoint()
	{
		return Core::Execution::EntryPoint();
	}
}


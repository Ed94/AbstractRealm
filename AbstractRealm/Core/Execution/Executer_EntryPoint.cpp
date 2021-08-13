// Parent Header
#include "Executer.hpp"



// Engine
#include "Cycler.hpp"
#include "Meta/Meta.hpp"
#include "Meta/AppInfo.hpp"
#include "HAL.hpp"
#include "HAL/HAL_GPU.hpp"
#include "MasterExecution.hpp"
#include "Concurrency/CyclerPool.hpp"
#include "SAL_ImGui.hpp"
#include "PAL/PAL.hpp"
#include "Core.hpp"
#include "Renderer/Renderer.hpp"



namespace Execution
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

	Dev_Declare_Log(Execution);

	

	// Public

	OSAL::ExitValT EntryPoint()
	{
		#ifdef Meta_UseCpp_Exceptions
		try
		{
		#endif
			Meta::LoadModule();

			SubLogger.Init();

			if (UseDebug())
			{
				using namespace LAL;

				if (!OSAL::CreateConsole()) return 0;

				cout << "Initializing Dev Module" << endl;

				Dev::Load();
			}

			Dev::Console_EnableAutoUpdate();

			// Set the floating-point precision to 10 places.
			STL::cout.precision(10);

			Log("Initiating");

			PAL ::Module::Load();
			Core::Module::Load();

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

			Core::Module::Unload();
			PAL ::Module::Unload();

			if (UseDebug())
			{
				OSAL::DestroyConsole();

				Dev::Unload();
			}
		#ifdef Meta_UseCpp_Exceptions
		}
		catch (std::exception e)
		{
			Log_Error(e.what());

			Dev::Console_UpdateBuffer();
		}
		#endif

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


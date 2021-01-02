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

		ptr<Window> DemoWindow;

		AppVersion AppVer = 
		{ 
			EEngineVersion::Major, 
			EEngineVersion::Minor, 
			EEngineVersion::Patch 
		};

		FrameBufferDimensions WindowSize = { 1280, 720 };

		bool WindowResized = false;

		void WindowSizeChanged(ptr<Window> /*_window*/, int _width, int _height)
		{
			WindowResized = true;

			WindowSize.Width = _width; WindowSize.Height = _height;
		}



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
		//Meta::

		try
		{
			if (UseDebug())
			{
				using namespace LAL;

				if (!OSAL::CreateConsole()) return 0;

				cout << "Initializing Dev Module" << endl;

				Dev::LoadModule();
			}

			Dev::Console_EnableAutoUpdate();

			std::cout.precision(10);

			Dev::CLog("Core-Execution: Initiating");

			OSAL::Load();

			HAL::LoadModule();

			HAL::GPU::Initialize_GPUComms
			(
				String("Abstract Realm: MVP " + 
				ToString(EEngineVersion::Major) + "." +
				ToString(EEngineVersion::Minor) + "." +
				ToString(EEngineVersion::Patch)).c_str(),
				AppVer
			);

			// Window

			WindowInfo windowSpec = {};

			windowSpec.WindowTitle = "Abstract Realm"               ;
			windowSpec.WindowSize  = WindowSize                     ;
			windowSpec.Windowed    = OSAL::WindowInfo ::WindowedMode;
			windowSpec.Resizable   = OSAL::WinBool::True            ;

			StaticData::EngineWindow = OSAL::Create_Window(windowSpec);

			OSAL::SetWindow_SizeChangeCallback(StaticData::EngineWindow, WindowSizeChanged);

			windowSpec.WindowTitle = "Clear Color Demo";

			DemoWindow = OSAL::Create_Window(windowSpec);

			OSAL::SetWindow_SizeChangeCallback(DemoWindow, WindowSizeChanged);

			HAL::GPU::Default_InitializeRenderer(StaticData::EngineWindow);

			HAL::GPU::Vulkan::Start_ClearColorDemo(DemoWindow);

			Imgui::Initialize(StaticData::EngineWindow);

			// Master execution

			Initialize_MasterCycler();

			Dev::Console_EnableAutoUpdate();

			// App closing

			Imgui::Deinitialize();

			HAL::GPU::Vulkan::Stop_ClearColorDemo();

			HAL::GPU::Default_DeinitializeRenderer(StaticData::EngineWindow);

			OSAL::Destroy_Window(StaticData::EngineWindow);

			HAL::GPU::Cease_GPUComms();

			OSAL::Unload();

			if (UseDebug())
			{
				OSAL::DestroyConsole();

				Dev::UnloadModule();
			}
		}
		catch (std::exception e)
		{
			CLog_Error(e.what());

			Dev::Console_UpdateBuffer();
		}

		return OSAL::ExitValT(EExitCode::Success);
	}

	ptr<OSAL::Window> EngineWindow() { return StaticData::EngineWindow; }
}



// OSAL Entrypoint:  Setting function overload.

namespace OSAL
{
	OSAL::ExitValT EntryPoint()
	{
		return Core::Execution::EntryPoint();
	}
}


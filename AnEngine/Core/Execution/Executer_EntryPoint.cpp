// Parent Header
#include "Executer.hpp"



#include "Cycler.hpp"
#include "Meta/AppInfo.hpp"
#include "HAL/GPU_HAL.hpp"
#include "MasterExecution.hpp"
#include "Concurrency/CyclerPool.hpp"

#include "ImGui_SAL.hpp"



namespace Core::Execution
{
	using namespace LAL;
	using namespace Meta;
	using namespace SAL;

	using OSAL::Window               ;
	using OSAL::WindowInfo           ;
	using OSAL::FrameBufferDimensions;

	ptr<Window> EngineWindow;

	AppVersion AppVer = 
	{ 
		EEngineVersion::Major, 
		EEngineVersion::Minor, 
		EEngineVersion::Patch 
	};

	FrameBufferDimensions WindowSize = { 1280, 720 };

	bool WindowResized = false;


	void WindowSizeChanged(ptr<Window> _window, int _width, int _height)
	{
		WindowResized = true;

		WindowSize.Width = _width; WindowSize.Height = _height;
	}

	OSAL::ExitValT EntryPoint()
	{
		if (UseDebug)
		{
			using namespace LAL;

			if (!OSAL::CreateConsole()) return 0;

			OSAL::Console_Bind_IOBuffersTo_OSIO();

			cout << "Console IO Buffers hooked to OS IO" << endl;

			cout << "EntryPoint: Starting the Core Execution Module" << endl;

			cout << "Core:Execution: Initiate Execution" << endl;

			cout << setfill('-') << setw(60); cout << " " << endl;

			cout << "Abstract Realm: MVP Build - "
			<< EEngineVersion::Major << "."
			<< EEngineVersion::Minor << "."
			<< EEngineVersion::Patch << endl;

			cout << setfill('-') << setw(60); cout << " " << endl << endl;

			cout << "Initializing Dev Module" << endl;

			Dev::LoadModule();
		}

		OSAL::Load();

		HAL::GPU::Load();

		HAL::GPU::Initialize_GPUComms("Abstract Realm: MVP 0.87.0", AppVer);

		// Window

		WindowInfo windowSpec = {};

		windowSpec.WindowTitle = "Abstract Realm"               ;
		windowSpec.WindowSize  = WindowSize                     ;
		windowSpec.Windowed    = OSAL::WindowInfo ::WindowedMode;
		windowSpec.Resizable   = OSAL::WinBool::True            ;

		EngineWindow = OSAL::Create_Window(windowSpec);



		OSAL::SetWindow_SizeChangeCallback(EngineWindow, WindowSizeChanged);

		HAL::GPU::Default_InitializeRenderer(EngineWindow);

		Imgui::Initialize(EngineWindow);

		Concurrency::CyclerPool::Initialize();

		Concurrency::CyclerPool::ActivateUnit();

		// Master execution

		Initialize_MasterCycler();
		
		// App closing

		Imgui::Deinitialize();

		HAL::GPU::Default_DeinitializeRenderer(EngineWindow);

		HAL::GPU::Cease_GPUComms();

		OSAL::Destroy_Window(EngineWindow);

		OSAL::Unload();

		if (UseDebug)
		{
			OSAL::DestroyConsole();
		}

		return OSAL::ExitValT(EExitCode::Success);
	}
}
// Parent Header
#include "Executer.hpp"



#include "Cycler.hpp"
#include "Meta/AppInfo.hpp"
#include "HAL/GPU_HAL.hpp"
#include "PAL/OSAL/OSAL.hpp"

#include "ImGui_SAL.hpp"



namespace Core::Execution
{
	using namespace LAL;
	using namespace Meta;
	using namespace SAL;

	using OSAL::Window;
	using OSAL::WindowCreationSpec;
	using OSAL::FrameBufferDimensions;

	bool exist = false;

	// Quick and dirty setup (Until proper is fleshed out...

	PrimitiveExecuter<void()> Engine_DirtyExecute_Master;
	PrimitiveExecuter<void()> Engine_DirtyExecute1;
	PrimitiveExecuter<void()> Engine_DirtyExecute2;
	PrimitiveExecuter<void()> Engine_DirtyExecute3;
	PrimitiveExecuter<void()> Engine_DirtyExecute4;
	PrimitiveExecuter<void()> Engine_DirtyExecute5;
	PrimitiveExecuter<void()> Engine_DirtyExecute6;
	PrimitiveExecuter<void()> Engine_DirtyExecute7;
	PrimitiveExecuter<void()> Engine_DirtyExecute8;
	PrimitiveExecuter<void()> Engine_DirtyExecute9;
	PrimitiveExecuter<void()> Engine_DirtyExecute10;
	PrimitiveExecuter<void()> Engine_DirtyExecute11;
	PrimitiveExecuter<void()> Engine_DirtyExecute12;
	PrimitiveExecuter<void()> Engine_DirtyExecute13;
	PrimitiveExecuter<void()> Engine_DirtyExecute14;
	PrimitiveExecuter<void()> Engine_DirtyExecute15;

	Cycler Cycler_Master;
	Cycler Cycler_Dirty1;
	Cycler Cycler_Dirty2;
	Cycler Cycler_Dirty3;
	Cycler Cycler_Dirty4;
	Cycler Cycler_Dirty5;
	Cycler Cycler_Dirty6;
	Cycler Cycler_Dirty7;
	Cycler Cycler_Dirty8;
	Cycler Cycler_Dirty9;
	Cycler Cycler_Dirty10;
	Cycler Cycler_Dirty11;
	Cycler Cycler_Dirty12;
	Cycler Cycler_Dirty13;
	Cycler Cycler_Dirty14;
	Cycler Cycler_Dirty15;

	DataSize Thread_1;
	DataSize Thread_2;
	DataSize Thread_3;
	DataSize Thread_4;
	DataSize Thread_5;
	DataSize Thread_6;
	DataSize Thread_7;
	DataSize Thread_8;
	DataSize Thread_9;
	DataSize Thread_10;
	DataSize Thread_11;
	DataSize Thread_12;
	DataSize Thread_13;
	DataSize Thread_14;
	DataSize Thread_15;

	DataSize mainCycle = 0;


		ptr<Window> TestWindow;

		AppVersion AppVer = { 1, 0, 0 };

	FrameBufferDimensions WindowSize = { 1280, 720 };

	bool WindowResized = false;

	void WindowSizeChanged(ptr<Window> _window, int _width, int _height)
	{
		WindowResized = true;

		WindowSize.Width = _width; WindowSize.Height = _height;
	}

	void Dirty_MasterExecute()
	{
		static float64 DeltaSince_DevUpdate = 0.0L;
		static float64 DevUpdateInterval = 1.0L / 240.0L;
		
		if (DeltaSince_DevUpdate >= DevUpdateInterval)
		{
			OSAL::PollEvents();

			Imgui::MakeFrame();

			Imgui::Render();

			HAL::GPU::Default_DrawFrame(TestWindow);
		}
		else
		{
			DeltaSince_DevUpdate += Cycler_Dirty1.GetDeltaTime().count();
		}
		
		if (OSAL::CanClose(TestWindow))
		{
			HAL::GPU::WaitFor_GPUIdle();

			Cycler_Dirty1.Lapse();
			Cycler_Dirty2.Lapse();
			Cycler_Dirty3.Lapse();
			Cycler_Dirty4.Lapse();
			Cycler_Dirty5.Lapse();
			Cycler_Dirty6.Lapse();
			Cycler_Dirty7.Lapse();
			Cycler_Dirty8.Lapse();
			Cycler_Dirty9.Lapse();
			Cycler_Dirty10.Lapse();
			Cycler_Dirty11.Lapse();
			Cycler_Dirty12.Lapse();
			Cycler_Dirty13.Lapse();
			Cycler_Dirty14.Lapse();
			Cycler_Dirty15.Lapse();
		}
		else
		{
			mainCycle++;
		}

		if 
		(
			Cycler_Dirty1.Lapsed() && 
			Cycler_Dirty2.Lapsed() && 
			Cycler_Dirty3.Lapsed() && 
			Cycler_Dirty4.Lapsed() &&
			Cycler_Dirty5.Lapsed() &&
			Cycler_Dirty6.Lapsed() &&
			Cycler_Dirty7.Lapsed() &&
			Cycler_Dirty8.Lapsed() &&
			Cycler_Dirty9.Lapsed() &&
			Cycler_Dirty10.Lapsed() &&
			Cycler_Dirty11.Lapsed() &&
			Cycler_Dirty12.Lapsed() &&
			Cycler_Dirty13.Lapsed() &&
			Cycler_Dirty14.Lapsed() &&
			Cycler_Dirty15.Lapsed()
		)
		{
			OSAL::DecommissionThread(Thread_1);
			OSAL::DecommissionThread(Thread_2);
			OSAL::DecommissionThread(Thread_3);
			OSAL::DecommissionThread(Thread_4);
			OSAL::DecommissionThread(Thread_5);
			OSAL::DecommissionThread(Thread_6);
			OSAL::DecommissionThread(Thread_7);
			OSAL::DecommissionThread(Thread_8);
			OSAL::DecommissionThread(Thread_9);
			OSAL::DecommissionThread(Thread_10);
			OSAL::DecommissionThread(Thread_11);
			OSAL::DecommissionThread(Thread_12);
			OSAL::DecommissionThread(Thread_13);
			OSAL::DecommissionThread(Thread_14);
			OSAL::DecommissionThread(Thread_15);

			Cycler_Master.Lapse();
		}
	}

	void Dirty_Execute1() 
	{
		static float64 DeltaSince_DevUpdate = 0.0L;
		static float64 DevUpdateInterval = 1.0L / 10.0L;

		if (DeltaSince_DevUpdate >= DevUpdateInterval)
		{
			Dev::CLog_Status("Master    Delta: " + ToString(Cycler_Master.GetDeltaTime().count()), 0, 0);
			Dev::CLog_Status("Thread 1  Delta: " + ToString(Cycler_Dirty1.GetDeltaTime().count()), 1, 0);

			Dev::CLog_Status("Thread 2  Delta: " + ToString(Cycler_Dirty2.GetDeltaTime().count()), 2, 0);
			Dev::CLog_Status("Thread 3  Delta: " + ToString(Cycler_Dirty2.GetDeltaTime().count()), 3, 0);

			Dev::CLog_Status("Thread 4  Delta: " + ToString(Cycler_Dirty2.GetDeltaTime().count()), 0, 1);
			Dev::CLog_Status("Thread 5  Delta: " + ToString(Cycler_Dirty1.GetDeltaTime().count()), 1, 1);
			Dev::CLog_Status("Thread 6  Delta: " + ToString(Cycler_Dirty2.GetDeltaTime().count()), 2, 1);
			Dev::CLog_Status("Thread 7  Delta: " + ToString(Cycler_Dirty2.GetDeltaTime().count()), 3, 1);

			Dev::CLog_Status("Thread 8  Delta: " + ToString(Cycler_Dirty1.GetDeltaTime().count()), 0, 2);
			Dev::CLog_Status("Thread 9  Delta: " + ToString(Cycler_Dirty2.GetDeltaTime().count()), 1, 2);
			Dev::CLog_Status("Thread 10 Delta: " + ToString(Cycler_Dirty2.GetDeltaTime().count()), 2, 2);
			Dev::CLog_Status("Thread 11 Delta: " + ToString(Cycler_Dirty2.GetDeltaTime().count()), 3, 2);

			Dev::CLog_Status("Thread 12 Delta: " + ToString(Cycler_Dirty2.GetDeltaTime().count()), 0, 3);
			Dev::CLog_Status("Thread 13 Delta: " + ToString(Cycler_Dirty2.GetDeltaTime().count()), 1, 3);
			Dev::CLog_Status("Thread 14 Delta: " + ToString(Cycler_Dirty2.GetDeltaTime().count()), 2, 3);
			Dev::CLog_Status("Thread 15 Delta: " + ToString(Cycler_Dirty2.GetDeltaTime().count()), 3, 3);

			Dev::Console_UpdateBuffer();

			DeltaSince_DevUpdate = 0.0L;
		}
		else
		{
			DeltaSince_DevUpdate += Cycler_Dirty1.GetDeltaTime().count();
		}
	}

	void Dirty_Execute2() 
	{
		static float64 DeltaSince_DevUpdate = 0.0L;
		static float64 DevUpdateInterval = 1.0L / 10.0L;

		if (DeltaSince_DevUpdate >= DevUpdateInterval)
		{
			//Dev::CLog("MainCycle: " + ToString(mainCycle));
			//OSAL::PollEvents();

			DeltaSince_DevUpdate = 0.0L;
		}
		else
		{
			DeltaSince_DevUpdate += Cycler_Dirty1.GetDeltaTime().count();
		}
	}

	void Dirty_Execute3() 
	{
		static float64 DeltaSince_DevUpdate = 0.0L;
		static float64 DevUpdateInterval = 1.0L / 60.0L;

		if (DeltaSince_DevUpdate >= DevUpdateInterval)
		{
			

			

			DeltaSince_DevUpdate = 0.0L;
		}
		else
		{
			DeltaSince_DevUpdate += Cycler_Dirty1.GetDeltaTime().count();
		}
	}

	void Dirty_Execute4() 
	{
		
	} 

	void Dirty_Execute5() {}
	void Dirty_Execute6() {}
	void Dirty_Execute7() {}
	void Dirty_Execute8() {} 
	void Dirty_Execute9() {}
	void Dirty_Execute10() {}
	void Dirty_Execute11() {}
	void Dirty_Execute12() {} 
	void Dirty_Execute13() {}
	void Dirty_Execute14() {}
	void Dirty_Execute15() {}


	OSAL::ExitValT EntryPoint()
	{
		OSAL::Record_EntryPoint_StartExecution();

		if (UseDebug)
		{
			using namespace LAL;

			if (!AllocConsole()) return 0;

			Dev::SetConsole_IOHooks();

			cout << "EntryPoint: Starting the Core Execution Module" << endl;

			cout << "Core:Execution: Initiate Execution" << endl;

			cout << setfill('-') << setw(140); cout << " " << endl;

			cout << "Abstract Realm: MVP Build - "
			<< EEngineVersion::Major << "."
			<< EEngineVersion::Minor << "."
			<< EEngineVersion::Patch << endl;

			cout << setfill('-') << setw(140); cout << " " << endl << endl;

			cout << "Initializing Dev Module" << endl;

			Dev::LoadModule();
		}

		OSAL::Load();

		HAL::GPU::Load();

		HAL::GPU::Initialize_GPUComms("Abstract Realm: MVP 0.88.0", AppVer);

		// Window

		WindowCreationSpec windowSpec = {};

		windowSpec.WindowTitle = "ImGui Test";
		windowSpec.WindowSize = WindowSize;
		windowSpec.Windowed = OSAL::WindowedMode;
		windowSpec.ClientAPI = GLFW::EClient_API::No_API;
		windowSpec.Resizable = GLFW::EBool::True;

		TestWindow = OSAL::Create_Window(windowSpec);

		OSAL::SetWindow_SizeChangeCallback(TestWindow, WindowSizeChanged);

		HAL::GPU::Default_InitializeRenderer(TestWindow);

		Imgui::Initialize(TestWindow);

		// Threads

		Engine_DirtyExecute_Master.Bind(Function<void()>(Dirty_MasterExecute));

		Engine_DirtyExecute1.Bind(Function<void()>(Dirty_Execute1));
		Engine_DirtyExecute2.Bind(Function<void()>(Dirty_Execute2));
		Engine_DirtyExecute3.Bind(Function<void()>(Dirty_Execute3));
		Engine_DirtyExecute4.Bind(Function<void()>(Dirty_Execute4));
		Engine_DirtyExecute5.Bind(Function<void()>(Dirty_Execute5));
		Engine_DirtyExecute6.Bind(Function<void()>(Dirty_Execute6));
		Engine_DirtyExecute7.Bind(Function<void()>(Dirty_Execute7));
		Engine_DirtyExecute8.Bind(Function<void()>(Dirty_Execute8));
		Engine_DirtyExecute9.Bind(Function<void()>(Dirty_Execute9));
		Engine_DirtyExecute10.Bind(Function<void()>(Dirty_Execute10));
		Engine_DirtyExecute11.Bind(Function<void()>(Dirty_Execute11));
		Engine_DirtyExecute12.Bind(Function<void()>(Dirty_Execute12));
		Engine_DirtyExecute13.Bind(Function<void()>(Dirty_Execute13));
		Engine_DirtyExecute14.Bind(Function<void()>(Dirty_Execute14));
		Engine_DirtyExecute15.Bind(Function<void()>(Dirty_Execute15));

		Cycler_Master.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute_Master));
		Cycler_Dirty1.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute1));
		Cycler_Dirty2.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute2));
		Cycler_Dirty3.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute3));
		Cycler_Dirty4.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute4));
		Cycler_Dirty5.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute5));
		Cycler_Dirty6.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute6));
		Cycler_Dirty7.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute7));
		Cycler_Dirty8.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute8));
		Cycler_Dirty9.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute9));
		Cycler_Dirty10.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute10));
		Cycler_Dirty11.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute11));
		Cycler_Dirty12.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute12));
		Cycler_Dirty13.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute13));
		Cycler_Dirty14.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute14));
		Cycler_Dirty15.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute15));

		Thread_1  = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty1 ));
		Thread_2  = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty2 ));
		Thread_3  = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty3 ));
		Thread_4  = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty4 ));
		Thread_5  = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty5 ));
		Thread_6  = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty6 ));
		Thread_7  = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty7 ));
		Thread_8  = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty8 ));
		Thread_9  = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty9 ));
		Thread_10 = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty10));
		Thread_11 = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty11));
		Thread_12 = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty12));
		Thread_13 = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty13));
		Thread_14 = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty14));
		Thread_15 = OSAL::RequestThread(&Cycler::Initiate, std::ref(Cycler_Dirty15));

		Dev::CLog_ToggleWriteToConsole();

		Cycler_Master.Initiate();

		Imgui::Deinitialize();

		HAL::GPU::Default_DeinitializeRenderer(TestWindow);

		HAL::GPU::Cease_GPUComms();

		OSAL::Destroy_Window(TestWindow);

		OSAL::Unload();

		if (UseDebug)
		{
			FreeConsole();
		}

		return EXIT_SUCCESS;
	}
}
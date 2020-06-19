#pragma once



#include "LAL/LAL.hpp"



#include "Meta/AppInfo.hpp"
#include "HAL/GPU_HAL.hpp"
#include "PAL/OSAL/OSAL.hpp"



namespace Debug::Vulkan_FullTut_Test
{
	using namespace LAL     ;
	using namespace Meta    ;
	using namespace HAL::GPU;
	using namespace OSAL    ;

	BSS
	(
		ptr<Window> WindowRef;

	);

	Data
	(
		AppVersion AppVer = { 1, 0, 0 };

		FrameBufferDimensions WindowSize = { 1280, 720 };

		bool WindowResized = false;
	);


	// Functions

	void WindowSizeChanged(ptr<Window> _window, int, int)
	{
		WindowResized = true;
	}

	void InitializeDependencies()
	{			
		OSAL::Load();

		HAL::GPU::Load();

		HAL::GPU::Initialize_GPUComms("ImGui Test", AppVer);

		// Window

		WindowCreationSpec windowSpec = {};

		windowSpec.WindowTitle = "ImGui Test"             ;
		windowSpec.WindowSize  = WindowSize               ;
		windowSpec.Windowed    = OSAL::WindowedMode       ;
		windowSpec.ClientAPI   = GLFW::EClient_API::No_API;
		windowSpec.Resizable   = GLFW::EBool::True        ;

		WindowRef = OSAL::Create_Window(windowSpec);

		OSAL::SetWindow_SizeChangeCallback(WindowRef, WindowSizeChanged);

		HAL::GPU::Dirty::GetRenderReady(WindowRef);
	}

	void PrimitiveCycler()
	{
		while (!OSAL::CanClose(WindowRef))
		{
			OSAL::PollEvents();

			HAL::GPU::Dirty::DrawFrame(WindowRef);
		}

		HAL::GPU::WaitFor_GPUIdle();
	}

	void DeinitalizeDependencies()
	{
		HAL::GPU::Dirty::DeinitializeRenderReady(WindowRef);

		HAL::GPU::Cease_GPUComms();

		OSAL::Destroy_Window(WindowRef);

		OSAL::Unload();
	}


	// Execution

	int Exec_TestRun()
	{
		try
		{
			InitializeDependencies();

			PrimitiveCycler();

			DeinitalizeDependencies();
		}
		catch (const std::exception& error)
		{
			std::cerr << error.what() << std::endl;

			getchar();

			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}
}

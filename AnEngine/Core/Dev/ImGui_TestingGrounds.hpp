#pragma once



// Includes


#include "LAL.hpp"

#include "Meta/AppInfo.hpp"
#include "HAL/GPU_HAL.hpp"
#include "PAL/OSAL/OSAL.hpp"

#include "ImGui_SAL.hpp"



namespace Debug
{
	namespace Imgui_Testing
	{
		using namespace LAL;
		using namespace Meta;
		using namespace SAL;

		using OSAL::Window;
		using OSAL::WindowCreationSpec;
		using OSAL::FrameBufferDimensions;
		


		BSS
		(
			ptr<Window> TestWindow;

			FrameBufferDimensions WindowSize = { 1280, 720 };

			Imgui::IO IO_Config;
		)

		Data
		(
			AppVersion AppVer = { 1, 0, 0 };

			bool WindowResized = false;
		)



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

			TestWindow = OSAL::Create_Window(windowSpec);

			OSAL::SetWindow_SizeChangeCallback(TestWindow, WindowSizeChanged);

			HAL::GPU::Dirty::GetRenderReady(TestWindow);
		}


		void InitalizeImgui()
		{
			using namespace SAL;

			Imgui::VerifyVersion();

			Imgui::CreateContext();

			IO_Config = Imgui::GetIO(); (void)IO_Config;   // TODO: Find out what that (void) is for...

			Imgui::HookToWindow(TestWindow);
		}


		void PrimitiveCycler()
		{
			while (! OSAL::CanClose(TestWindow))
			{
				OSAL::PollEvents();

				HAL::GPU::Dirty::DrawFrame(TestWindow);
			}

			HAL::GPU::WaitFor_GPUIdle();
		}

		void DeinitalizeImgui()
		{

		}

		void DeinitalizeDependencies()
		{
			HAL::GPU::Dirty::DeinitializeRenderReady(TestWindow);

			HAL::GPU::Cease_GPUComms();

			OSAL::Destroy_Window(TestWindow);

			OSAL::Unload();
		}


		int Exec_ImguiTest()
		{
			try
			{
				InitializeDependencies();

				//InitalizeImgui();

				PrimitiveCycler();

				//DeinitalizeImgui();

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
}

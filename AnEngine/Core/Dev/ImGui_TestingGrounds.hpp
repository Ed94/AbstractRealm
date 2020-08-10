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
		)

		Data
		(
			AppVersion AppVer = { 1, 0, 0 };

			FrameBufferDimensions WindowSize = { 1280, 720 };

			bool WindowResized = false;
		)



		void WindowSizeChanged(ptr<Window> _window, int _width, int _height)
		{
			WindowResized = true;

			WindowSize.Width = _width; WindowSize.Height = _height;
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

			Imgui::Initialize(TestWindow);
		}


		void PrimitiveCycler()
		{
			while (! OSAL::CanClose(TestWindow))
			{
				OSAL::PollEvents();

				if (WindowResized)
				{
					//Imgui::MakeWindow(WindowSize.Width, WindowSize.Height);
				}

				Imgui::MakeFrame();

				Imgui::Render();

				HAL::GPU::Dirty::DrawFrame(TestWindow);
			}

			HAL::GPU::WaitFor_GPUIdle();
		}


		void DeinitalizeDependencies()
		{
			Imgui::Deinitialize();

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
}

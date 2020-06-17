#pragma once

// Includes




#include "ImGui_SAL.hpp"
#include "Meta/AppInfo.hpp"
#include "PAL/OSAL/OSAL.hpp"
#include "HAL/GPU_HAL.hpp"

#include "LAL.hpp"




namespace Debug
{
	namespace Imgui_Testing
	{
		using namespace LAL;
		using namespace Meta;

		using OSAL::Window;

		BSS
		(
			ptr<Window> TestWindow;
		)

		Data
		(
			AppVersion AppVer = { 1, 0, 0 };
		)


		void InitializeDependencies()
		{
			HAL::GPU::Initalize_GPUComms("ImGui Test", AppVer);
		}


		void InitalizeImgui()
		{
			using namespace SAL;

			Imgui::VerifyVersion();


		}


		int Exec_ImguiTest()
		{
			try
			{
				InitializeDependencies();

				InitalizeImgui();
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

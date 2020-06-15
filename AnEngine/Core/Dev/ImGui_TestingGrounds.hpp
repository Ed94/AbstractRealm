
// Includes




#include "ImGui_SAL.hpp"
#include "PAL/OSAL/OSAL.hpp"

#include "LAL.hpp"




namespace Debug
{
	namespace Imgui_Testing
	{
		using namespace LAL;

		using OSAL::Window;

		bss< ptr<Window>> TestWindow;



		void InitializeDependencies()
		{
			
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

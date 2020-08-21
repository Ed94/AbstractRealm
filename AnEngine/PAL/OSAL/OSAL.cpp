// Parent Header
#include "OSAL.hpp"



#include "OSAL_Backend.hpp"
#include "OSAL_Hardware.hpp"



namespace OSAL
{
	using namespace Meta;


	// Public

	void Load()
	{
		CLog("Loading Module...");

		Load_Hardware();

		Load_Platform();

		Load_Timing();	

		QueryThreadInfo();	

		GenerateThreads();

		switch (WindowingPlatform)
		{
			case EWindowingPlatform::GLFW:
			{
				SAL::GLFW::Initalize();

				CLog("Initialized windowing platform: GLFW");
				
				break;
			}
		}
	}

	void Unload()
	{
		switch (WindowingPlatform)
		{
			case EWindowingPlatform::GLFW:
			{
				SAL::GLFW::Terminate();

				CLog("Terminated windowing platform: GLFW");
			}
		}

		CLog("Unloaded module");
	}

	void PollEvents()
	{
		if constexpr (WindowingPlatform == Meta::EWindowingPlatform::GLFW)
		{
			SAL::GLFW::PollEvents();
		}
	}


	// Private


	void GetProessInformation()
	{

	}
}
// Parent Header
#include "OSAL.hpp"





namespace OSAL
{
	using namespace Meta;



	// Public

	

	void Load()
	{
		OS_CLog("Loading Module...");

		Load_Timing();	

		QueryThreadInfo();	

		switch (WindowingPlatform)
		{
			case EWindowingPlatform::GLFW:
			{
				SAL::GLFW::Initalize();

				OS_CLog("Initialized windowing Platform: GLFW");
				
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
			}
		}
	}

	void PollEvents()
	{
		if (WindowingPlatform == Meta::EWindowingPlatform::GLFW)
		{
			SAL::GLFW::PollEvents();
		}
	}


	// Private


	void GetProessInformation()
	{

	}
}
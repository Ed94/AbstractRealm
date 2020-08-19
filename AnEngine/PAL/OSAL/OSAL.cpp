// Parent Header
#include "OSAL.hpp"



#include "OSAL_Backend.hpp"



namespace OSAL
{
	using namespace Meta;


	// Public

	void Load()
	{
		OS_CLog("Loading Module...");

		Load_Timing();	

		QueryThreadInfo();	

		GenerateThreads();

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
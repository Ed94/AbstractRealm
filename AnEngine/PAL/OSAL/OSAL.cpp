// Parent Header
#include "OSAL.hpp"



namespace OSAL
{
	using namespace Meta;



	// Public

	void Load()
	{
		switch (WindowingPlatform)
		{
			case EWindowingPlatform::GLFW:
			{
				SAL::GLFW::Initalize();
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
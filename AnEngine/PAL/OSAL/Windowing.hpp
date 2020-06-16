#pragma once


#include "LAL/LAL.hpp"

#include "Meta/Config/OSAL.hpp"

#include "SAL/GLFW_SAL.hpp"



namespace OSAL
{
	// Usings

	using namespace LAL;


	namespace PlatformBackend
	{
		// Usings

		using namespace Meta;


		// Compile-time

		using Window = 
			Choose<WindowingPlatform == EWindowingPlatform::GLFW,

				SAL::GLFW::Window,
				void*
			>;
	}


	using Window = PlatformBackend::Window;




	Where<WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	void> GetFramebuffer(Window* _window)
	{
		
	}
}

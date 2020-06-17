#pragma once


#include "LAL/LAL.hpp"

#include "Meta/Config/OSAL_Config.hpp"

#include "OSAL/Platform.hpp"

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
	using PlatformTypes = PlatformBackend::PlatformTypes;


	struct FrameBufferDimensions	
	{
		FrameBufferDimensions() { Width = 0; Height = 0; }

		FrameBufferDimensions(int _width, int _height) : Width(_width), Height(_height)
		{}

		int Width, Height;
	};


	Window* Create_Window();

	void Destroy_Window(Window* _window);


	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	PlatformTypes::OS_WindowHandle> GetOS_WindowHandle(Window* _window);

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	FrameBufferDimensions> GetFramebufferDimensions(const ptr<Window> _window);
}

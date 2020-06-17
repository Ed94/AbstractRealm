// Parent Header
#include "Windowing.hpp"




namespace OSAL
{
	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	PlatformTypes::OS_WindowHandle> GetOS_WindowHandle(Window* _window)
	{
		return SAL::GLFW::GetOS_WindowHandle(_window);
	}

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	FrameBufferDimensions> GetFramebufferDimensions(const ptr<Window> _window)
	{
		int width, height;

		SAL::GLFW::GetFramebufferSize(_window, width, height);

		return FrameBufferDimensions(width, height);
	}
}
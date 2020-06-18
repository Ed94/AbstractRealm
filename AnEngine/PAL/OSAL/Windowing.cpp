// Parent Header
#include "Windowing.hpp"




namespace OSAL
{
	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	ptr< Window>> Create_Window
	(
		WindowCreationSpec _windowCreationSpec
	)
	{
		using namespace SAL::GLFW;

		SetWindowCreationParameter(EWindowCreationParameter::ClientAPI, _windowCreationSpec.ClientAPI);
		SetWindowCreationParameter(EWindowCreationParameter::Resizable, _windowCreationSpec.Resizable);

		return MakeWindow
		(
			_windowCreationSpec.WindowSize.Width, 
			_windowCreationSpec.WindowSize.Height, 
			_windowCreationSpec.WindowTitle, 
			_windowCreationSpec.Windowed,
			_windowCreationSpec.WindowToShareWith
		);
	}

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	bool> CanClose(const ptr<Window> _window)
	{
		return SAL::GLFW::CanClose(_window);
	}

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	void> Destroy_Window(const ptr<Window> _window)
	{
		SAL::GLFW::DestroyWindow(_window);
	}

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	void> SetWindow_SizeChangeCallback(const ptr<Window> _window, WindowSize_CallbackFunc _callback)
	{
		SAL::GLFW::SetWidnowSizeCallback(_window, _callback);
	}

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	PlatformTypes::OS_WindowHandle> GetOS_WindowHandle(const ptr<Window> _window)
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
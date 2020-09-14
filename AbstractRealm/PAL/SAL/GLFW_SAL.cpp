// Parent Header
#include "GLFW_SAL.hpp"



namespace SAL::GLFW
{
	bool CanClose(const ptr<Window> _window)
	{
		return glfwWindowShouldClose(_window);
	}

	ptr<Window> MakeWindow(int _width, int _height, ptr<const char> _title, ptr<Monitor> _fullscreenCast, ptr<Window> _windowToShareWith)
	{
		return Heap() glfwCreateWindow(_width, _height, _title, _fullscreenCast, _windowToShareWith);
	}

	void DestroyWindow(ptr<Window> _window)
	{
		Heap() glfwDestroyWindow(_window);
	}

	bool Initalize()
	{
		Heap() return glfwInit() ? true : false;
	}

	void PollEvents()
	{
		glfwPollEvents();

		return;
	}

	void Terminate()
	{
		Heap() glfwTerminate();
	}



	// Vulkan Related

	CStrArray GetRequiredVulkanAppExtensions(uint32& _extensionCount)
	{
		return glfwGetRequiredInstanceExtensions(getAddress(_extensionCount));
	}

	void WaitForEvents()
	{
		glfwWaitEvents();
	}



	// OS Related

	Where<OSAL::IsWindows, OSAL::PlatformTypes::
	OS_WindowHandle> GetOS_WindowHandle(const ptr<Window> _window)
	{
		return glfwGetWin32Window(_window);
	}

	void GetFramebufferSize(const ptr<Window> _window, int& _width, int& _height)
	{
		glfwGetFramebufferSize(_window, &_width, &_height);
	}

	void SetWidnowSizeCallback(const ptr<Window> _window, WindowSize_Callback _callback)
	{
		glfwSetWindowSizeCallback(_window, _callback);
	}
}
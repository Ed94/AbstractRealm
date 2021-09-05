// Parent Header
#include "TPAL_GLFW.hpp"


namespace TPAL::GLFW
{
	bool CanClose(const ptr<Window> _window)
	{
		return glfwWindowShouldClose(_window);
	}

	ptr<Window> MakeWindow(int _width, int _height, ptr<const char> _title, ptr<Monitor> _fullscreenCast, ptr<Window> _windowToShareWith)
	{
		auto handle = glfwCreateWindow(_width, _height, _title, _fullscreenCast, _windowToShareWith);

		return handle;
	}

	void DestroyWindow(ptr<Window> _window_in)
	{
		glfwDestroyWindow(_window_in);
	}

	bool Initalize()
	{
		if (glfwInit())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void PollEvents()
	{
		glfwPollEvents();

		return;
	}

	void Terminate()
	{
		glfwTerminate();
	}


	// Vulkan Related

	CStrArray GetRequiredVulkanAppExtensions(u32& _extensionCount_out)
	{
		return glfwGetRequiredInstanceExtensions(getPtr(RCast<uint32_t>(_extensionCount_out)));
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

	void GetFramebufferSize(const ptr<Window> _window, int& _width_out, int& _height_out)
	{
		glfwGetFramebufferSize(_window, &_width_out, &_height_out);
	}

	void SetWidnowSizeCallback(const ptr<Window> _window, WindowSize_Callback _callback_out)
	{
		glfwSetWindowSizeCallback(_window, _callback_out);
	}
}
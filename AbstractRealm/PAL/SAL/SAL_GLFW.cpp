// Parent Header
#include "SAL_GLFW.hpp"


#include "Core/Memory/MemTracking.hpp"


namespace SAL::GLFW
{
	using namespace TP_API;
	using namespace Memory;

	bool CanClose(const ptr<Window> _window)
	{
		return glfwWindowShouldClose(_window);
	}

	ptr<Window> MakeWindow(int _width, int _height, ptr<const char> _title, ptr<Monitor> _fullscreenCast, ptr<Window> _windowToShareWith)
	{
		auto handle = glfwCreateWindow(_width, _height, _title, _fullscreenCast, _windowToShareWith);

		return handle;
	}

	void DestroyWindow(ptr<Window> _window)
	{
		glfwDestroyWindow(_window);
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

	CStrArray GetRequiredVulkanAppExtensions(u32& _extensionCount)
	{
		return glfwGetRequiredInstanceExtensions(getPtr(RCast<uint32_t&, u32&>(_extensionCount)));
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
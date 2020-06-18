// Parent Header
#include "ImGui_SAL.hpp"



namespace SAL::Imgui
{
	namespace PlatformBackend
	{
		data< bool> GLFW_InstallCallbacks = true;
	}

	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
	void> HookToWindow(ptr<OSAL::Window> _window)
	{
		switch (Meta::GPU_API)
		{
			case Meta::EGPUPlatformAPI::Vulkan:
			{
				ImGui_ImplGlfw_InitForVulkan(_window, PlatformBackend::GLFW_InstallCallbacks);
			}
		}
	}

	void VerifyVersion()
	{
		IMGUI_CHECKVERSION();
	}
}
// Parent Header
#include "ImGui_SAL.hpp"

#include "GPU_HAL.hpp"

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

				switch (Meta::GPU_API)
				{
					case Meta::EGPUPlatformAPI::Vulkan:
					{
						using HAL::GPU::Platform_Vulkan::RenderContext;

						/* Not ready.
						RenderContext& renderContext =  dref(RCast<RenderContext>(HAL::GPU::GetRenderContext(_window)));

						ImGui_ImplVulkan_InitInfo initSpec {};

						initSpec.Instance = renderContext.AppGPU;
						initSpec.PhysicalDevice = renderContext.PhysicalDevice;
						initSpec.Device = renderContext.LogicalDevice;
						initSpec.QueueFamily = renderContext.QueueFamilies.GraphicsFamily.value();
						initSpec.Queue = renderContext.GraphicsQueue;
						initSpec.PipelineCache = renderContext.PipelineCache;
						//initSpec.DescriptorPool TODO: Come back when you have finished the vulkan tutorial and have a full descriptor pool based on the imgui tut.
						initSpec.Allocator = renderContext.Allocator;
						//initSpec.MinImageCount
						//initSpec.ImageCount;
						//initSpec.CheckVkResultFn;

						*/
						
					}
				}
			}
		}
	}

	void VerifyVersion()
	{
		IMGUI_CHECKVERSION();
	}
}
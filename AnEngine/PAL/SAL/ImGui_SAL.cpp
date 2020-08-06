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
						using HAL::GPU::Platform_Vulkan::RawRenderContext;

						RawRenderContext& renderContext =  dref(RCast<RawRenderContext>(HAL::GPU::GetRenderContext(_window)));

						ImGui_ImplVulkan_InitInfo initSpec {};

						initSpec.Instance        = renderContext.ApplicationInstance;
						initSpec.PhysicalDevice  = renderContext.PhysicalDevice     ;
						initSpec.Device          = renderContext.LogicalDevice      ;
						initSpec.QueueFamily     = renderContext.QueueFamilyIndex   ;
						initSpec.Queue           = renderContext.Queue              ;
						initSpec.PipelineCache   = renderContext.PipelineCache      ;
						initSpec.DescriptorPool  = renderContext.DescriptorPool     ;
						initSpec.Allocator       = renderContext.Allocator->operator const VkAllocationCallbacks*();
						initSpec.MinImageCount   = renderContext.MinimumFrameBuffers;
						initSpec.ImageCount      = renderContext.FrameBufferCount   ;
						initSpec.CheckVkResultFn = renderContext.CheckResultFunction;
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
// Parent Header
#include "ImGui_SAL.hpp"



#include "GPU_HAL.hpp"



namespace SAL::Imgui
{
	namespace PlatformBackend
	{
		// Static Data

		// GLFW

		bool GLFW_InstallCallbacks = true;

		// Vulkan

		HAL::GPU::Vulkan::RawRenderContext* RenderContext;

		VT::V4::GraphicsPipeline GraphicsPipeline;

		VT::V4::DescriptorPool DescriptorPool;

		DynamicArray<VT::V4::Framebuffer> SwapChain_Framebuffers;


		// Functions

		void Vulkan_CreateDescriptorObjects()
		{
			using namespace VT::V4;

			DescriptorPool::Size pool_sizes[11];

			pool_sizes[0 ].Type = EDescriptorType::Sampler             ; pool_sizes[0 ].Count = 1000;
			pool_sizes[1 ].Type = EDescriptorType::CombinedImageSampler; pool_sizes[1 ].Count = 1000;
			pool_sizes[2 ].Type = EDescriptorType::SampledImage        ; pool_sizes[2 ].Count = 1000;
			pool_sizes[3 ].Type = EDescriptorType::StorageImage        ; pool_sizes[3 ].Count = 1000;
			pool_sizes[4 ].Type = EDescriptorType::UniformTexelBuffer  ; pool_sizes[4 ].Count = 1000;
			pool_sizes[5 ].Type = EDescriptorType::StorageTexelBuffer  ; pool_sizes[5 ].Count = 1000;
			pool_sizes[6 ].Type = EDescriptorType::UniformBuffer       ; pool_sizes[6 ].Count = 1000;
			pool_sizes[7 ].Type = EDescriptorType::StorageBuffer       ; pool_sizes[7 ].Count = 1000;
			pool_sizes[8 ].Type = EDescriptorType::UniformBufferDynamic; pool_sizes[8 ].Count = 1000;
			pool_sizes[9 ].Type = EDescriptorType::StorageBufferDynamic; pool_sizes[9 ].Count = 1000;
			pool_sizes[10].Type = EDescriptorType::InputAttachment     ; pool_sizes[10].Count = 1000;

			DescriptorPool::CreateInfo info;

			info.Flags.Set(EDescriptorPoolCreateFlag::FreeDescriptorSet);

			info.MaxSets       = 1000 *  ((int)(sizeof(pool_sizes)) / sizeof(*(pool_sizes)));   // TODO: Define this better
			info.PoolSizeCount = (uint32)((int)(sizeof(pool_sizes)) / sizeof(*(pool_sizes)));
			info.PoolSizes     = pool_sizes;

			HAL::GPU::Vulkan::RequestDescriptorPool(DescriptorPool, info);
		}

		void Vulkan_DebugCallback(VkResult _returnCode)
		{
			if (_returnCode != VkResult(VT::V4::EResult::Success))
			{
				std::cerr << "Imgui Callback Error Code: " << int(_returnCode) << std::endl;
			}
		}

		void Vulkan_GPU_DrawRender(VT::V4::CommandBuffer& _buffer, int _index)
		{
			ImDrawData* draw_data = ImGui::GetDrawData();

			// Record dear Imgui primitives into command buffer
			ImGui_ImplVulkan_RenderDrawData(draw_data, _buffer.GetHandle());
		}
	}



	// Static Data

	Imgui::IO IO_Config;



	// Functions

	void BindToPlatformAndRenderer(ptr<OSAL::Window> _window)
	{
		switch (Meta::WindowingPlatform)
		{
			case Meta::EWindowingPlatform::GLFW:
			{
				ImGui_ImplGlfw_InitForVulkan(_window, PlatformBackend::GLFW_InstallCallbacks);

				break;
			}
		}

		switch (Meta::GPU_API)
		{
			case Meta::EGPUPlatformAPI::Vulkan:
			{
				using namespace HAL::GPU;

				using Vulkan::RawRenderContext;

				using namespace PlatformBackend;

				RenderContext = RCast<RawRenderContext>(GetRenderContext(_window));

				ImGui_ImplVulkan_InitInfo initSpec{};

				initSpec.Instance        = RenderContext->ApplicationInstance   ;
				initSpec.PhysicalDevice  = RenderContext->PhysicalDevice        ;
				initSpec.Device          = RenderContext->LogicalDevice         ;
				initSpec.QueueFamily     = RenderContext->Queue.GetFamilyIndex();
				initSpec.Queue           = RenderContext->Queue                 ;
				initSpec.PipelineCache   = RenderContext->PipelineCache         ;
				initSpec.Allocator       = RenderContext->Allocator->operator const VkAllocationCallbacks*();
				initSpec.MinImageCount   = RenderContext->MinimumFrameBuffers   ;
				initSpec.ImageCount      = RenderContext->FrameBufferCount      ;
				initSpec.MSAASamples     = VkSampleCountFlagBits(RenderContext->MSAA_Samples);
				initSpec.CheckVkResultFn = PlatformBackend::Vulkan_DebugCallback;

				SetupGPU_Interface();

				initSpec.DescriptorPool = DescriptorPool.GetHandle();

				ImGui_ImplVulkan_Init(&initSpec, RenderContext->RenderPass);

				ImGui_ImplVulkan_SetMinImageCount(HAL::GPU::Vulkan::GetMinimumFramebufferCount());

				break;
			}
		}
	}

	void Deinitialize()
	{
		ImGui_ImplVulkan_Shutdown();

		PlatformBackend::DescriptorPool.Destroy();
	}

	void Initialize(ptr<OSAL::Window> _window)
	{
		using namespace SAL;

		Imgui::VerifyVersion();

		Imgui::CreateContext();

		IO_Config = Imgui::GetIO(); (void)IO_Config;   // TODO: Find out what that (void) is for...

		Imgui::StyleColorsDark();

		Imgui::BindToPlatformAndRenderer(_window);

		Imgui::SetupFonts();
	}

	void MakeFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame  ();

		ImGui::NewFrame();

		ImGui::ShowDemoWindow();
	}

	void SetupFonts()
	{
		switch (Meta::GPU_API)
		{
			case Meta::EGPUPlatformAPI::Vulkan:
			{
				using namespace HAL::GPU::Vulkan;

				VT::V4::CommandBuffer cmdBuffer = RequestSingleTimeBuffer();

				ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer);

				EndSingleTimeBuffer(cmdBuffer);

				break;
			}
		}
	}

	void SetupGPU_Interface()
	{
		switch (Meta::GPU_API)
		{
			case Meta::EGPUPlatformAPI::Vulkan:
			{
				using namespace HAL::GPU;

				PlatformBackend::Vulkan_CreateDescriptorObjects();

				Vulkan::AddRenderCallback(PlatformBackend::Vulkan_GPU_DrawRender);

				break;
			}
		}
	}	
	
	void VerifyVersion()
	{
		IMGUI_CHECKVERSION();
	}
}
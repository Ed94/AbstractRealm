// Parent Header
#include "ImGui_SAL.hpp"

#include "GPU_HAL.hpp"

namespace SAL::Imgui
{
	namespace PlatformBackend
	{
		data< bool> GLFW_InstallCallbacks = true;

		void Vulkan_DebugCallback(VkResult _returnCode)
		{
			std::cerr << "Imgui Callback Error Code: " << int(_returnCode) << std::endl;
		}
	}


	bool* SwapChain_RecreatedRef;

	HAL::GPU::Vulkan::RawRenderContext* RenderContext;

	VT::V4::DescriptorPool DescriptorPool;


	std::vector<VT::V4::CommandPool          > CommandPools;
	std::vector<VT::V4::CommandBuffer        > CommandBuffers;
	std::vector<VT::V4::CommandBuffer::Handle> CommandBufferHandles;

	std::vector<VT::V4::Framebuffer> SwapChain_Framebuffers;

	VT::V4::RenderPass RenderPass;

	void Vulkan_CreateDescriptorPool();
	void Vulkan_CreateRenderPass();
	void Create_FrameBuffers();
	void Vulkan_CreateFrameObjects();


	Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
		void> BindToPlatformAndRenderer(ptr<OSAL::Window> _window)
	{
		switch (Meta::GPU_API)
		{
			case Meta::EGPUPlatformAPI::Vulkan:
			{
				using namespace HAL::GPU;

				ImGui_ImplGlfw_InitForVulkan(_window, PlatformBackend::GLFW_InstallCallbacks);

				using Vulkan::RawRenderContext;

				RenderContext = RCast<RawRenderContext>(GetRenderContext(_window));

				ImGui_ImplVulkan_InitInfo initSpec{};

				initSpec.Instance = RenderContext->ApplicationInstance;
				initSpec.PhysicalDevice = RenderContext->PhysicalDevice;
				initSpec.Device = RenderContext->LogicalDevice;
				initSpec.QueueFamily = RenderContext->QueueFamilyIndex;
				initSpec.Queue = RenderContext->Queue;
				initSpec.PipelineCache = RenderContext->PipelineCache;
				initSpec.Allocator = RenderContext->Allocator->operator const VkAllocationCallbacks * ();
				initSpec.MinImageCount = Vulkan::GetMinimumFramebufferCount();
				initSpec.ImageCount = Vulkan::GetNumberOfFramebuffers();
				initSpec.CheckVkResultFn = PlatformBackend::Vulkan_DebugCallback;

				SetupGPU_Resources();

				initSpec.DescriptorPool = DescriptorPool.GetHandle();

				ImGui_ImplVulkan_Init(&initSpec, RenderPass.GetHandle());

				break;
			}
		}
	}

	void MakeFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		ImGui::ShowDemoWindow();
	}

	void Render()
	{
		ImGui::Render();

		//FrameRender(wd);
		//FramePresent(wd);
	}

	void GPU_DrawRender()
	{
		ImDrawData* draw_data = ImGui::GetDrawData();

		// Record dear imgui primitives into command buffer
		//ImGui_ImplVulkan_RenderDrawData(draw_data, );
	}

	void SetupGPU_Resources()
	{
		switch (Meta::GPU_API)
		{
			case Meta::EGPUPlatformAPI::Vulkan:
			{
				HAL::GPU::Vulkan::AddRenderCallback(GPU_DrawRender);

				Vulkan_CreateDescriptorPool();

				Vulkan_CreateRenderPass();

				Vulkan_CreateFrameObjects();

				SwapChain_RecreatedRef = &HAL::GPU::Vulkan::SwapChain_Recreated;
			}
		}
	}	

	ImGui_ImplVulkanH_Window WindowDataTemp;

	void MakeWindow(int _width, int _height)
	{
		ImGui_ImplVulkan_SetMinImageCount(HAL::GPU::Vulkan::GetMinimumFramebufferCount());

		ImGui_ImplVulkanH_CreateOrResizeWindow
		(
			RenderContext->ApplicationInstance, 
			RenderContext->PhysicalDevice, 
			RenderContext->LogicalDevice, 
			&WindowDataTemp,
			RenderContext->QueueFamilyIndex, 
			RenderContext->Allocator->operator const VkAllocationCallbacks *(), 
			_width, 
			_height,
			HAL::GPU::Vulkan::GetMinimumFramebufferCount()
		);

		WindowDataTemp.FrameIndex = 0;
	}

	void VerifyVersion()
	{
		IMGUI_CHECKVERSION();
	}

	void SetupFonts()
	{
		switch (Meta::GPU_API)
		{
			case Meta::EGPUPlatformAPI::Vulkan:
			{
				VT::V4::CommandBuffer cmdBuffer = HAL::GPU::Vulkan::Dirty::RequestSingleTimeBuffer();

				ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer);

				HAL::GPU::Vulkan::Dirty::EndSingleTimeBuffer(cmdBuffer);
			}
		}
	}

	void Vulkan_CreateFrameObjects()
	{
		using namespace VT::V4;
		using namespace HAL::GPU;

		CommandPool::CreateInfo poolInfo{};

		poolInfo.QueueFamilyIndex = RenderContext->QueueFamilyIndex;
		poolInfo.Flags = CommandPool::CreateFlgas();             // Optional

		CommandPools.resize(Vulkan::GetNumberOfFramebuffers());
		CommandBuffers.resize(Vulkan::GetNumberOfFramebuffers());
		CommandBufferHandles.resize(Vulkan::GetNumberOfFramebuffers());

		for (DeviceSize index = 0; index < Vulkan::GetNumberOfFramebuffers(); index++)
		{
			if (CommandPools[index].Create(RenderContext->LogicalDevice, poolInfo) != EResult::Success)
			{
				throw std::runtime_error("failed to create command pool!");
			}

			CommandPool::AllocateInfo info;

			info.BufferCount = 1;
			info.Pool = CommandPools[index];
			info.Level = ECommandBufferLevel::Primary;

			if (CommandPools[index].Allocate(info, CommandBuffers[index]) != EResult::Success)
				throw std::runtime_error("failed to allocate command buffers!");

			CommandBufferHandles[index] = CommandBuffers[index].GetHandle();
		}
	}

	void Create_FrameBuffers()
	{
		using namespace VT::V4;
		using namespace HAL::GPU;

		int count = Vulkan::GetNumberOfFramebuffers();

		SwapChain_Framebuffers.resize(count);

		for (DataSize index = 0; index < count; index++)
		{
			ImageView::Handle attachments;

			Framebuffer::CreateInfo framebufferInfo{};

			framebufferInfo.RenderPass = RenderPass.GetHandle();
			framebufferInfo.AttachmentCount = 1;
			framebufferInfo.Attachments = &attachments;
			framebufferInfo.Width = RenderContext->FrameSize.Width;
			framebufferInfo.Height = RenderContext->FrameSize.Height;
			framebufferInfo.Layers = 1;

			if (SwapChain_Framebuffers[index].Create(RenderContext->LogicalDevice, framebufferInfo) != EResult::Success)
			{
				throw std::runtime_error("Failed to create framebuffer!");
			}
		}
	}

	void Vulkan_CreateRenderPass()
	{
		VT::V4::RenderPass::AttachmentDescription attachment;

		using namespace VT::V4;

		attachment.Format = RenderContext->ImageFormat;
		attachment.Samples = ESampleCount::_1;
		attachment.LoadOp = EAttachmentLoadOperation::Load;
		attachment.StoreOp = EAttachmentStoreOperation::Store;
		attachment.StencilLoadOp = EAttachmentLoadOperation::DontCare;
		attachment.StencilStoreOp = EAttachmentStoreOperation::DontCare;
		attachment.InitialLayout = EImageLayout::Color_AttachmentOptimal;
		attachment.FinalLayout = EImageLayout::PresentSource_KHR;

		VT::V4::RenderPass::AttachmentReference colorAttachment;

		colorAttachment.Attachment = 0;
		colorAttachment.Layout = EImageLayout::Color_AttachmentOptimal;

		VT::V4::RenderPass::SubpassDescription subpass;

		subpass.PipelineBindPoint = EPipelineBindPoint::Graphics;
		subpass.ColorAttachmentCount = 1;
		subpass.ColorAttachments = &colorAttachment;

		VT::V4::RenderPass::SubpassDependency dependency;

		dependency.SourceSubpass = Subpass_External;
		dependency.DestinationSubpass = 0;

		dependency.SourceStageMask.Set(EPipelineStageFlag::ColorAttachmentOutput);
		dependency.DestinationStageMask.Set(EPipelineStageFlag::ColorAttachmentOutput);
		dependency.DestinationAccessMask.Set(EAccessFlag::ColorAttachmentWrite);

		VT::V4::RenderPass::CreateInfo info;

		info.AttachmentCount = 1;
		info.Attachments = &attachment;
		info.SubpassCount = 1;
		info.Subpasses = &subpass;
		info.DependencyCount = 1;
		info.Dependencies = &dependency;

		VT::V4::EResult returnCode = RenderPass.Create(RenderContext->LogicalDevice, info);

		if (returnCode != VT::V4::EResult::Success)
			throw RuntimeError("for some reason render pass failed...");
	}

	void Vulkan_CreateDescriptorPool()
	{
		using namespace VT::V4;

		DescriptorPool::Size pool_sizes[11];

		pool_sizes[0].Type = EDescriptorType::Sampler;
		pool_sizes[0].Count = 1000;

		pool_sizes[1].Type = EDescriptorType::CombinedImageSampler;
		pool_sizes[1].Count = 1000;

		pool_sizes[2].Type = EDescriptorType::SampledImage;
		pool_sizes[2].Count = 1000;

		pool_sizes[3].Type = EDescriptorType::StorageImage;
		pool_sizes[3].Count = 1000;

		pool_sizes[4].Type = EDescriptorType::UniformTexelBuffer;
		pool_sizes[4].Count = 1000;

		pool_sizes[5].Type = EDescriptorType::StorageTexelBuffer;
		pool_sizes[5].Count = 1000;

		pool_sizes[6].Type = EDescriptorType::UniformBuffer;
		pool_sizes[6].Count = 1000;

		pool_sizes[7].Type = EDescriptorType::StorageBuffer;
		pool_sizes[7].Count = 1000;

		pool_sizes[8].Type = EDescriptorType::UniformBufferDynamic;
		pool_sizes[8].Count = 1000;

		pool_sizes[9].Type = EDescriptorType::StorageBufferDynamic;
		pool_sizes[9].Count = 1000;

		pool_sizes[10].Type = EDescriptorType::InputAttachment;
		pool_sizes[10].Count = 1000;

		DescriptorPool::CreateInfo info;

		info.Flags.Set(EDescriptorPoolCreateFlag::FreeDescriptorSet);
		info.MaxSets = 1000 *
			((int)(sizeof(pool_sizes)) / sizeof(*(pool_sizes)));   // TODO: Define this better

		info.PoolSizeCount = (uint32)((int)(sizeof(pool_sizes)) / sizeof(*(pool_sizes)));

		info.PoolSizes = pool_sizes;

		HAL::GPU::Vulkan::Dirty::RequestDescriptorPool(DescriptorPool, info);
	}
}
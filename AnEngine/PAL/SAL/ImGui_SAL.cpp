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

	VT::V4::GraphicsPipeline GraphicsPipeline;
	VT::V4::Pipeline::Layout PipelineLayout;
	VT::V4::Pipeline::Layout::DescriptorSet DescriptorSetLayout;

	VT::V4::DescriptorPool DescriptorPool;
	//DynamicArray<VT::V4::DescriptorSet        > DescriptorSets;
	//DynamicArray<VT::V4::DescriptorSet::Handle> DescriptorSetHandles;


	//std::vector<VT::V4::CommandPool          > CommandPools;
	//std::vector<VT::V4::CommandBuffer        > CommandBuffers;
	//std::vector<VT::V4::CommandBuffer::Handle> CommandBufferHandles;

	std::vector<VT::V4::Framebuffer> SwapChain_Framebuffers;

	//VT::V4::RenderPass RenderPass;

	void Vulkan_CreateDescriptorObjects();
	void Vulkan_CreateGraphicsPipeline();
	//void Vulkan_CreateRenderPass();
	//void Create_FrameBuffers();
	//void Vulkan_CreateFrameObjects();


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
				initSpec.MSAASamples = VK_SAMPLE_COUNT_8_BIT;

				SetupGPU_Resources();

				initSpec.DescriptorPool = DescriptorPool.GetHandle();

				ImGui_ImplVulkan_Init(&initSpec, RenderContext->RenderPass);

				ImGui_ImplVulkan_SetMinImageCount(HAL::GPU::Vulkan::GetMinimumFramebufferCount());

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

	void GPU_DrawRender(VT::V4::CommandBuffer& _buffer, int _index)
	{


		/*_buffer.BindDescriptorSets
		(
			VT::V4::EPipelineBindPoint::Graphics,
			PipelineLayout,
			0,
			1,
			&DescriptorSetHandles[_index]
		);*/

		ImDrawData* draw_data = ImGui::GetDrawData();

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(draw_data, _buffer.GetHandle());
	}

	void SetupGPU_Resources()
	{
		switch (Meta::GPU_API)
		{
			case Meta::EGPUPlatformAPI::Vulkan:
			{
				HAL::GPU::Vulkan::AddRenderCallback(GPU_DrawRender);

				Vulkan_CreateDescriptorObjects();

				//Vulkan_CreateRenderPass();

				//Vulkan_CreateFrameObjects();

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

	//void Vulkan_CreateFrameObjects()
	//{
	//	using namespace VT::V4;
	//	using namespace HAL::GPU;

	//	CommandPool::CreateInfo poolInfo{};

	//	poolInfo.QueueFamilyIndex = RenderContext->QueueFamilyIndex;
	//	poolInfo.Flags = CommandPool::CreateFlgas();             // Optional

	//	CommandPools.resize(Vulkan::GetNumberOfFramebuffers());
	//	CommandBuffers.resize(Vulkan::GetNumberOfFramebuffers());
	//	CommandBufferHandles.resize(Vulkan::GetNumberOfFramebuffers());

	//	for (DeviceSize index = 0; index < Vulkan::GetNumberOfFramebuffers(); index++)
	//	{
	//		if (CommandPools[index].Create(RenderContext->LogicalDevice, poolInfo) != EResult::Success)
	//		{
	//			throw std::runtime_error("failed to create command pool!");
	//		}

	//		CommandPool::AllocateInfo info;

	//		info.BufferCount = 1;
	//		info.Pool = CommandPools[index];
	//		info.Level = ECommandBufferLevel::Primary;

	//		if (CommandPools[index].Allocate(info, CommandBuffers[index]) != EResult::Success)
	//			throw std::runtime_error("failed to allocate command buffers!");

	//		CommandBufferHandles[index] = CommandBuffers[index].GetHandle();
	//	}
	//}

	/*void Vulkan_CreateRenderPass()
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
	}*/

	/*void Create_FrameBuffers()
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
	}*/

	void Vulkan_CreateDescriptorObjects()
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

	void Vulkan_CreateGraphicsPipeline()
	{
		using namespace VT::V4;

		Pipeline::InputAssemblyState::CreateInfo inputAssembly;

		inputAssembly.Topology = EPrimitiveTopology::TriangleList;
		inputAssembly.PrimitiveRestartEnable = EBool::False;

		Pipeline::RasterizationState::CreateInfo rasterization;

		rasterization.EnableDepthClamp = EBool::False;
		rasterization.FrontFace = EFrontFace::CounterClockwise;
		rasterization.CullMode.Set(ECullModeFlag::None);
		rasterization.PolygonMode = EPolygonMode::Fill;
		rasterization.LineWidth = 1.0f;

		Pipeline::ColorBlendState::AttachmentState colorBlendAttachment;

		colorBlendAttachment.EnableBlend = EBool::True;
		colorBlendAttachment.ColorWriteMask.Set(EColorComponentFlag::R, EColorComponentFlag::G, EColorComponentFlag::B, EColorComponentFlag::A);
		colorBlendAttachment.Source_ColorBlendFactor = EBlendFactor::AlphaSource;
		colorBlendAttachment.Destination_ColorBlendFactor = EBlendFactor::OneMinus_AlphaSource;
		colorBlendAttachment.Source_AlphaBlendFactor = EBlendFactor::OneMinus_AlphaSource;
		colorBlendAttachment.Destination_AlphaBlendFactor = EBlendFactor::Zero;
		colorBlendAttachment.AlphaOperation = EBlendOperation::Add;

		Pipeline::ColorBlendState::CreateInfo colorBlendState;

		colorBlendState.AttachmentCount = 1;
		colorBlendState.Attachments = &colorBlendAttachment;
		colorBlendState.LogicOperation = ELogicOperation::Clear;
		colorBlendState.EnableLogicOperations = EBool::False;
		colorBlendState.BlendConstants[0] = 0.0f;
		colorBlendState.BlendConstants[1] = 0.0f;
		colorBlendState.BlendConstants[2] = 0.0f;
		colorBlendState.BlendConstants[3] = 0.0f;

		Pipeline::DepthStencilState::CreateInfo depthStencil;

		depthStencil.DepthTestEnable = EBool::False;
		depthStencil.DepthWriteEnable = EBool::False;
		depthStencil.DepthCompareOp = ECompareOperation::LessOrEqual;
		depthStencil.Back.CompareOp = ECompareOperation::Always;
		depthStencil.StencilTestEnable = EBool::False;

		depthStencil.DepthBoundsTestEnable = EBool::False;

		depthStencil.MinDepthBounds = 0.0f;
		depthStencil.MaxDepthBounds = 1.0f;

		StencilOperationState nullStencil {};

		depthStencil.Front = nullStencil;   // Optional
		depthStencil.Back = nullStencil;   // Optional

		Viewport viewport{};

		viewport.X = 0.0f;
		viewport.Y = 0.0f;
		viewport.Width = float(RenderContext->FrameSize.Width);
		viewport.Height = float(RenderContext->FrameSize.Height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		Rect2D scissor{};

		scissor.Offset.X = 0;
		scissor.Offset.Y = 0;	

		Pipeline::ViewportState::CreateInfo viewportState;

		viewportState.ViewportCount = 1;
		viewportState.ScissorCount = 1;
		viewportState.Viewports = &viewport;
		viewportState.Scissors = &scissor;

		Pipeline::MultiSampleState::CreateInfo multisample;

		multisample.RasterizationSamples = ESampleCount::_1;

		std::vector<EDynamicState> dynamicStateEnables = {
			EDynamicState::Viewport,
			EDynamicState::Scissor
		};

		Pipeline::DynamicState::CreateInfo dynamicState;

		dynamicState.StateCount = dynamicStateEnables.size();
		dynamicState.States = dynamicStateEnables.data();

		std::array<Pipeline::ShaderStage::CreateInfo, 2> shaderStages{};

		Pipeline::VertexInputState::BindingDescription vertexInputBind;

		vertexInputBind.Binding = 0;
		vertexInputBind.Stride = sizeof(ImDrawVert);
		vertexInputBind.InputRate = EVertexInputRate::Vertex;

		std::vector<Pipeline::VertexInputState::BindingDescription> vertexInputBindings;

		vertexInputBindings.push_back(vertexInputBind);


		Pipeline::VertexInputState::AttributeDescription attrib1, attrib2, attrib3;

		attrib1.Binding = 0;
		attrib1.Format = EFormat::R32_G32_SFloat;
		attrib1.Offset = offsetof(ImDrawVert, ImDrawVert::pos);
		attrib1.Location = 0;

		attrib2.Binding = 0;
		attrib2.Format = EFormat::R32_G32_SFloat;
		attrib2.Offset = offsetof(ImDrawVert, ImDrawVert::uv);
		attrib2.Location = 1;

		attrib3.Binding = 0;
		attrib3.Format = EFormat::R8_G8_B8_A8_UNormalized;
		attrib3.Offset = offsetof(ImDrawVert, ImDrawVert::col);
		attrib3.Location = 2;



			//vks::initializers::vertexInputBindingDescription(0, sizeof(ImDrawVert), VK_VERTEX_INPUT_RATE_VERTEX),
		std::vector<Pipeline::VertexInputState::AttributeDescription> vertexInputAttributes;

		vertexInputAttributes.push_back(attrib1);
		vertexInputAttributes.push_back(attrib2);
		vertexInputAttributes.push_back(attrib3);
			//vks::initializers::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, pos)),	// Location 0: Position
			//vks::initializers::vertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, uv)),	// Location 1: UV
			//vks::initializers::vertexInputAttributeDescription(0, 2, VK_FORMAT_R8G8B8A8_UNORM, offsetof(ImDrawVert, col)),	// Location 0: Color

		Pipeline::VertexInputState::CreateInfo vertexInputState ;

		vertexInputState.VertexBindingDescriptionCount = vertexInputBindings.size();
		vertexInputState.AttributeDescriptionCount = vertexInputAttributes.size();
		vertexInputState.AttributeDescription = vertexInputAttributes.data();
		vertexInputState.BindingDescriptions = vertexInputBindings.data();
			//vks::initializers::pipelineVertexInputStateCreateInfo();
		//vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
		//vertexInputState.pVertexBindingDescriptions = vertexInputBindings.data();
		//vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
		//vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

		GraphicsPipeline::CreateInfo info;

		info.InputAssemblyState = &inputAssembly;
		info.RasterizationState = &rasterization;
		info.ColorBlendState = &colorBlendState;
		info.MultisampleState = &multisample;
		info.ViewportState = &viewportState;
		info.DepthStencilState = &depthStencil;
		info.DynamicState = &dynamicState;
		info.StageCount = shaderStages.size();
		info.Stages = shaderStages.data();
		info.VertexInputState = &vertexInputState;



		//shaderStages[0] = example->loadShader(shadersPath + "imgui/ui.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		//shaderStages[1] = example->loadShader(shadersPath + "imgui/ui.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);


		GraphicsPipeline.Create(RenderContext->LogicalDevice, info);
	}
}
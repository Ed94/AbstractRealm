/*
Triangle Test

Last Modified: 5/19/2020

https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code
*/



#pragma once


//#include <vulkan/vulkan.h>



#include "Core/IO/Basic_FileIO.hpp"
#include "Core/Memory/MemTypes.hpp"
#include "Meta/EngineInfo.hpp"
#include "Meta/Config/HAL.hpp"

#include "Renderer/Shader/TriangleShader/TriangleShader.hpp"

// GLFW MUST BE FIRST, I DO NOT KNOW WHY JUST FUCKING DO IT.
#include "PAL/HAL/Vulkan/Vulkan_API.hpp"
#include "PAL/SAL/GLFW_SAL.hpp"

#include "LAL.hpp"



namespace Debug
{
	namespace Vulkan_TriangleTest
	{
		// Namespaces

		using namespace Vulkan;

		using namespace LAL;

		using namespace Meta;



		// Typedefs

		// GLFW

		using SAL::GLFW::Window;

		// Vulkan

		using AppInfo                     = AppInstance::AppInfo           ;
		using AppCreateInfo               = AppInstance::CreateInfo        ;
		using AppHandle                   = AppInstance::Handle            ;
		using Bytecode_Buffer             = DynamicArray< SPIR_V::Bytecode>;
		using CommandBufferList           = std::vector< VkCommandBuffer  >;   // TODO: Wrap
		using ExtensionIdentifierList     = std::vector< RoCStr           >;
		using FenceList                   = std::vector< VkFence          >;
		using FrameBufferList             = std::vector< VkFramebuffer    >;   // TODO: Wrap
		using ImageList                   = std::vector< Image::Handle    >;
		using ImageViewList               = std::vector< ImageView::Handle>;
		using SemaphoreList               = std::vector< VkSemaphore      >;   // TODO: Wrap
		using SurfaceFormatList           = std::vector< Surface::Format  >;
		using SurfacePresentationModeList = std::vector< EPresentationMode>;
		using ValidationLayerList         = std::vector< RoCStr           >;


		struct QueueFamilyIndices
		{
			Maybe<uint32> GraphicsFamily    ;
			Maybe<uint32> PresentationFamily;


			bool IsComplete()
			{
				return
					GraphicsFamily    .has_value() &&
					PresentationFamily.has_value()
					;
			}
		};

		struct SwapChainSupportDetails
		{
			Surface::Capabilities       Capabilities     ;
			SurfaceFormatList           Formats          ;
			SurfacePresentationModeList PresentationModes;
		};



		// Static Data

		// GLFW

		data< ptr<Window> > TriangleWindow = nullptr;

		data< int> Width = 1280, Height = 720;

		// Vulkan

		data< DataSize> CurrentFrame = 0;

		data< sInt32> MaxFramesInFlight = 2;

		bss< AppHandle> App;

		bss< CommandBufferList> CommandBuffers;

		bss< VkCommandPool> CommandPool;   // TODO: Wrap

		bss< Messenger::Handle    > DebugMessenger_Handle      ;
		bss< Messenger::CreateInfo> DebugMessenger_CreationSpec;

		bss< LogicalDevice::Handle       > LogicalDevice    ;
		bss< LogicalDevice::Queue::Handle> GraphicsQueue    ;
		bss< LogicalDevice::Queue::Handle> PresentationQueue;

		data< PhysicalDevice::Handle> PhysicalDevice = PhysicalDevice::NullHandle();

		bss< Pipeline::Layout::Handle> PipelineLayout;

		data< ExtensionIdentifierList> DeviceExtensions =
		{
			Swapchain_ExtensionName
		};

		bss< VkPipeline> GraphicsPipeline;

		bss< SemaphoreList> ImageAvailable_Semaphores;
		bss< SemaphoreList> RenderFinished_Semaphores;

		bss< FenceList> InFlightFences;
		bss< FenceList> ImagesInFlight;

		bss< Surface::Handle> SurfaceHandle;

		bss< SwapChain::Handle> SwapChain;

		bss< Extent2D     > SwapChain_Extent     ;
		bss< ImageList    > SwapChain_Images     ;
		bss< EImageFormat > SwapChain_ImageFormat;
		bss< ImageViewList> SwapChain_ImageViews ;

		data< bool> FramebufferResized = false;

		bss< FrameBufferList> SwapChain_Framebuffers;

		data< ValidationLayerList> ValidationLayerIdentifiers =
		{
			ValidationLayer_Khronos
		};

		bss< VkRenderPass> RenderPass;   // TODO: Wrap.


		// Forward Declarations

		// Vulkan

		ShaderModule::Handle    CreateTriShaderModule                   (const IO::FileBuffer& code);
		ExtensionIdentifierList GetRequiredExtensions                   ();
		QueueFamilyIndices      FindQueueFamilies                       (PhysicalDevice::Handle _deviceHandle);
		bool                    PhysicalDevice_CheckExtensionSupport    (PhysicalDevice::Handle _handle);
		void                    PopulateDebugMessengerCreateInfo        (Messenger::CreateInfo& _msngrCreateInfo);
		SwapChainSupportDetails QuerySwapChainSupport                   (PhysicalDevice::Handle _deviceHandle);
		Extent2D                Surface_SwapChain_ChooseExtent          (const Surface::Capabilities& _capabilities);
		Surface::Format         Surface_SwapChain_ChooseFormat          (const SurfaceFormatList& _availableFormats);
		EPresentationMode       Surface_SwapChain_ChoosePresentationMode(const SurfacePresentationModeList _surfacePresentationModes);




		// Functions

		// GLFW

		
		// Vulkan

		bool CheckValidationLayerSupport()
		{
			uInt32&& layerCount = GetNumOf_AvailableGlobalLayerProperties();

			using LayerPropertyList = std::vector<LayerProperties>;

			stack<LayerPropertyList> availableLayers(layerCount);

			GetAvailableGlobalLayerProperties(availableLayers.data() );

			stack<bool> layerFound = false;

			for (auto validationLayerName : ValidationLayerIdentifiers)
			{
				for (const auto& layer : availableLayers)
				{
					if (strcmp(validationLayerName, layer.LayerName) == 0)
					{
						layerFound = true;

						break;
					}
				}
			}

			if (!layerFound)
			{
				return false;
			}

			return true;
		}

		// TODO: Wrap
		void CreateCommandBuffers()
		{
			CommandBuffers.resize(SwapChain_Framebuffers.size());

			VkCommandBufferAllocateInfo allocSpec {};

			allocSpec.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocSpec.commandPool        = CommandPool;
			allocSpec.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocSpec.commandBufferCount = (uint32_t)CommandBuffers.size();

			if (vkAllocateCommandBuffers(LogicalDevice, &allocSpec, CommandBuffers.data()) != VK_SUCCESS) 
			{
				throw std::runtime_error("failed to allocate command buffers!");
			}

			for (size_t index = 0; index < CommandBuffers.size(); index++) 
			{
				VkCommandBufferBeginInfo beginInfo {};

				beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags            = 0; // Optional
				beginInfo.pInheritanceInfo = nullptr; // Optional

				if (vkBeginCommandBuffer(CommandBuffers[index], &beginInfo) != VK_SUCCESS) 
				{
					throw std::runtime_error("Failed to begin recording command buffer!");
				}

				VkRenderPassBeginInfo renderPassInfo{};

				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = RenderPass;
				renderPassInfo.framebuffer = SwapChain_Framebuffers[index];

				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = SwapChain_Extent;

				VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

				renderPassInfo.clearValueCount = 1;
				renderPassInfo.pClearValues = &clearColor;

				vkCmdBeginRenderPass(CommandBuffers[index], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

					vkCmdBindPipeline(CommandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);

					vkCmdDraw(CommandBuffers[index], 3, 1, 0, 0);

				vkCmdEndRenderPass(CommandBuffers[index]);

				if (vkEndCommandBuffer(CommandBuffers[index]) != VK_SUCCESS) 
				{
					throw std::runtime_error("Failed to record command buffer!");
				}
			}
		}

		// TODO: Wrap
		void CreateCommandPool()
		{
			QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(PhysicalDevice);

			VkCommandPoolCreateInfo poolInfo {};

			poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();
			poolInfo.flags            = 0; // Optional

			if (vkCreateCommandPool(LogicalDevice, &poolInfo, nullptr, &CommandPool) != VK_SUCCESS) 
			{
				throw std::runtime_error("failed to create command pool!");
			}
		}

		// TODO: Wrap
		void CreateFrameBuffers()
		{
			SwapChain_Framebuffers.resize(SwapChain_ImageViews.size());

			for (size_t index = 0; index < SwapChain_ImageViews.size(); index++) 
			{
				VkImageView attachments[] = 
				{
					SwapChain_ImageViews[index]
				};

				VkFramebufferCreateInfo framebufferInfo {};

				framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass      = RenderPass;
				framebufferInfo.attachmentCount = 1;
				framebufferInfo.pAttachments    = attachments;
				framebufferInfo.width           = SwapChain_Extent.Width;
				framebufferInfo.height          = SwapChain_Extent.Height;
				framebufferInfo.layers          = 1;

				if (vkCreateFramebuffer(LogicalDevice, &framebufferInfo, nullptr, &SwapChain_Framebuffers[index]) != VK_SUCCESS) 
				{
					throw std::runtime_error("Failed to create framebuffer!");
				}
			}
		}

		void CreateGraphicsPipeline()
		{
			// Shader setup

			using namespace Renderer::Shader;

			auto triShader_VertCode = IO::BufferFile(String(Paths::TriangleShader) + "TriangleShader_Vert.spv");
			auto triShader_FragCode = IO::BufferFile(String(Paths::TriangleShader) + "TriangleShader_Frag.spv");

			//TODO: FIGURE THIS OUT.
			Bytecode_Buffer triShader_Vert_Bytecode(triShader_VertCode.begin(), triShader_VertCode.end());
			Bytecode_Buffer triShader_Frag_Bytecode(triShader_FragCode.begin(), triShader_FragCode.end());

			ShaderModule::Handle triShaderModule_Vert = CreateTriShaderModule(triShader_VertCode);
			ShaderModule::Handle triShaderModule_Frag = CreateTriShaderModule(triShader_FragCode);

			Pipeline::ShaderStage::CreateInfo triShaderStage_Vert_CreationSpec{};
			Pipeline::ShaderStage::CreateInfo triShaderStage_Frag_CreationSpec{};

			triShaderStage_Vert_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			triShaderStage_Vert_CreationSpec.Stage = EShaderStageFlag::Vertex;

			triShaderStage_Vert_CreationSpec.Module = triShaderModule_Vert;
			triShaderStage_Vert_CreationSpec.Name = "main";

			triShaderStage_Frag_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			triShaderStage_Frag_CreationSpec.Stage = EShaderStageFlag::Fragment;

			triShaderStage_Frag_CreationSpec.Module = triShaderModule_Frag;
			triShaderStage_Frag_CreationSpec.Name = "main";

			Pipeline::ShaderStage::CreateInfo shaderStages[] = { triShaderStage_Vert_CreationSpec, triShaderStage_Frag_CreationSpec };

			// Fixed Function

			Pipeline::VertexInputState::CreateInfo vertexInputState_CreationSpec{};

			vertexInputState_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

			vertexInputState_CreationSpec.VertexBindingDescriptionCount = 0;
			vertexInputState_CreationSpec.BindingDescriptions = nullptr;

			vertexInputState_CreationSpec.AttributeDescriptionCount = 0;
			vertexInputState_CreationSpec.AttributeDescription = nullptr;

			Pipeline::InputAssembly::CreateInfo inputAssembly_CreationSpec{};

			inputAssembly_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

			inputAssembly_CreationSpec.Topology = EPrimitiveTopology::TriangleList;
			inputAssembly_CreationSpec.PrimitiveRestartEnable = false;

			Viewport viewport{};

			viewport.X = 0.0f;
			viewport.Y = 0.0f;
			viewport.Width = float(SwapChain_Extent.Width);
			viewport.Height = float(SwapChain_Extent.Height);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;

			Rect2D scissor{};

			scissor.Offset = { 0,0 };
			scissor.Extent = SwapChain_Extent;

			// TODO: Wrap
			VkPipelineViewportStateCreateInfo viewportState_CreationSpec{};

			viewportState_CreationSpec.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState_CreationSpec.viewportCount = 1;
			viewportState_CreationSpec.pViewports = (VkViewport*)(&viewport);
			viewportState_CreationSpec.scissorCount = 1;
			viewportState_CreationSpec.pScissors = (VkRect2D*)(&scissor);

			Pipeline::RasterizationState::CreateInfo rasterizer{};

			rasterizer.SType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.EnableDepthClamp = EBool::False;
			rasterizer.EnableRasterizerDiscard = EBool::False;
			rasterizer.PolygonMode = EPolygonMode::Fill;
			rasterizer.LineWidth = 1.0f;
			rasterizer.CullMode.Set(ECullModeFlag::Back);
			rasterizer.FrontFace = EFrontFace::Clockwise;
			rasterizer.EnableDepthBias = EBool::False;
			rasterizer.DepthBiasConstantFactor = 0.0f;
			rasterizer.DepthBiasClamp = 0.0f;
			rasterizer.DepthBiasSlopeFactor = 0.0f;

			Pipeline::MultisampleState::CreateInfo multisampling_CreationSpec{};

			multisampling_CreationSpec.SType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling_CreationSpec.EnableSampleShading = EBool::False;
			multisampling_CreationSpec.RasterizationSamples = ESampleCount::_1;
			multisampling_CreationSpec.MinSampleShading = 1.0f;
			multisampling_CreationSpec.SampleMask = nullptr;
			multisampling_CreationSpec.EnableAlphaToCoverage = EBool::False;
			multisampling_CreationSpec.EnableAlphaToOne = EBool::False;

			Pipeline::ColorBlend::AttachmentState colorBlend_Attachment{};

			colorBlend_Attachment.ColorWriteMask.Set(EColorComponentFlag::R, EColorComponentFlag::G, EColorComponentFlag::B, EColorComponentFlag::A);
			colorBlend_Attachment.EnableBlend = EBool::False;
			colorBlend_Attachment.Source_ColorBlendFactor = EBlendFactor::One;
			colorBlend_Attachment.Destination_ColorBlendFactor = EBlendFactor::Zero;
			colorBlend_Attachment.ColorOperation = EBlendOperation::Add;
			colorBlend_Attachment.Source_AlphaBlendFactor = EBlendFactor::One;
			colorBlend_Attachment.Destination_AlphaBlendFactor = EBlendFactor::Zero;
			colorBlend_Attachment.AlphaOperation = EBlendOperation::Add;

			Pipeline::ColorBlend::CreateInfo colorBlending_CreationSpec{};

			colorBlending_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending_CreationSpec.EnableLogicOperations = EBool::False;
			colorBlending_CreationSpec.LogicOperation = ELogicOperation::Copy;
			colorBlending_CreationSpec.AttachmentCount = 1;
			colorBlending_CreationSpec.Attachments = &colorBlend_Attachment;
			colorBlending_CreationSpec.BlendConstants[0] = 0.0f;
			colorBlending_CreationSpec.BlendConstants[1] = 0.0f;
			colorBlending_CreationSpec.BlendConstants[2] = 0.0f;
			colorBlending_CreationSpec.BlendConstants[3] = 0.0f;

			EDynamicState States[] =
			{
				EDynamicState::Viewport,
				EDynamicState::LineWidth
			};

			Pipeline::DynamicState::CreateInfo dynamicState {};

			dynamicState.SType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.StateCount = 2;
			dynamicState.States = States;

			Pipeline::Layout::CreateInfo pipelineLayout_CreationSpec {};

			pipelineLayout_CreationSpec.SType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			//pipelineLayout_CreationSpec.Extension = nullptr;
			pipelineLayout_CreationSpec.SetLayoutCount = 0;
			pipelineLayout_CreationSpec.SetLayouts = nullptr;
			pipelineLayout_CreationSpec.PushConstantRangeCount = 0;
			pipelineLayout_CreationSpec.PushConstantRanges = nullptr;
			//pipelineLayout_CreationSpec.Flags = 0;

			//VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			//pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			//pipelineLayoutInfo.setLayoutCount = 0; // Optional
			//pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
			//pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
			//pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
			//pipelineLayoutInfo.

			EResult&& piplineLayout_CreationResult = 
				Pipeline_CreateLayout(LogicalDevice, pipelineLayout_CreationSpec, nullptr, PipelineLayout);

			if (piplineLayout_CreationResult != EResult::Success)
			{
				throw std::runtime_error("Failed to create pipeline layout!");
			}

			// TODO: Wrap

			VkGraphicsPipelineCreateInfo pipelineInfo {};

			pipelineInfo.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages    = (VkPipelineShaderStageCreateInfo*)(shaderStages);

			pipelineInfo.pVertexInputState   = (VkPipelineVertexInputStateCreateInfo*)(&vertexInputState_CreationSpec);
			pipelineInfo.pInputAssemblyState = (VkPipelineInputAssemblyStateCreateInfo*)(&inputAssembly_CreationSpec);
			pipelineInfo.pViewportState      = (VkPipelineViewportStateCreateInfo*)(&viewportState_CreationSpec);
			pipelineInfo.pRasterizationState = (VkPipelineRasterizationStateCreateInfo*)(&rasterizer);
			pipelineInfo.pMultisampleState   = (VkPipelineMultisampleStateCreateInfo*)(&multisampling_CreationSpec);
			pipelineInfo.pDepthStencilState  = nullptr; // Optional
			pipelineInfo.pColorBlendState    = (VkPipelineColorBlendStateCreateInfo*)(&colorBlending_CreationSpec);
			pipelineInfo.pDynamicState       = nullptr; // Optional

			pipelineInfo.layout = (VkPipelineLayout)(PipelineLayout);

			pipelineInfo.renderPass = RenderPass;
			pipelineInfo.subpass = 0;

			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
			pipelineInfo.basePipelineIndex = -1; // Optional

			if (vkCreateGraphicsPipelines(LogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &GraphicsPipeline) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to create graphics pipeline!");
			}

			ShaderModule_Destory(LogicalDevice, triShaderModule_Vert, nullptr);
			ShaderModule_Destory(LogicalDevice, triShaderModule_Frag, nullptr);
		}	

		void CreateImageViews()
		{
			SwapChain_ImageViews.resize(SwapChain_Images.size());

			for (DataSize index = 0; index < SwapChain_Images.size(); index++)
			{
				ImageView::CreateInfo creationSpec {};

				creationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

				creationSpec.Image = SwapChain_Images[index];

				creationSpec.ViewType = EImageViewType::_2D;

				creationSpec.Format = SwapChain_ImageFormat;

				creationSpec.Components.R = EComponentSwizzle::Identitity;
				creationSpec.Components.G = EComponentSwizzle::Identitity;
				creationSpec.Components.B = EComponentSwizzle::Identitity;
				creationSpec.Components.A = EComponentSwizzle::Identitity;

				creationSpec.SubresourceRange.AspectMask.Set(EImageAspect::Color);

				creationSpec.SubresourceRange.BaseMipLevel   = 0;
				creationSpec.SubresourceRange.LevelCount     = 1;
				creationSpec.SubresourceRange.BaseArrayLayer = 0;
				creationSpec.SubresourceRange.LayerCount     = 1;

				EResult&& creationResult = CreateImageView(LogicalDevice, creationSpec, nullptr, &SwapChain_ImageViews[index]);

				if (creationResult != EResult::Success)
				{
					throw std::runtime_error("Failed to create image views!");
				}
			}
		}

		void CreateInstance()
		{
			stack<AppInfo      > appSpec {};
			stack<AppCreateInfo> appCreateSpec {};

			if (Vulkan_ValidationLayersEnabled)
			{
				if (!CheckValidationLayerSupport())
					throw std::runtime_error("Validation layers requested, but are not available!");
			}

			using Meta::EEngineVersion;

			//AppSpec.SType         = EStructureType::ApplicationInformation;   // Use later...
			appSpec.SType         = EStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appSpec.AppName       = "Triangle Test"                       ;
			appSpec.AppVersion    = MakeVersion(1, 0, 0)                  ;
			appSpec.EngineName    = "AnEngine"                            ;
			appSpec.EngineVersion = MakeVersion
			(
				EEngineVersion::Major, 
				EEngineVersion::Minor, 
				EEngineVersion::Patch
			)                                                             ;
			appSpec.API_Version   = EAPI_Version::_1_0                    ;

			//AppCreateSpec.SType   = EStructureType::InstanceCreateInfo;
			appCreateSpec.SType   = EStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			appCreateSpec.AppInfo = getAddress(appSpec)       ;

			/*AppCreateSpec.EnabledExtensionNames = GetRequiredVulkanAppExtensions
			(
				getAddress(AppCreateSpec.EnabledExtensionCount)
			);*/

			ExtensionIdentifierList extensions = GetRequiredExtensions();

			appCreateSpec.EnabledExtensionCount = SCast<uint32>(extensions.size());
			appCreateSpec.EnabledExtensionNames = extensions.data();


			if (Vulkan_ValidationLayersEnabled)
			{
				appCreateSpec.EnabledLayerCount = SCast<uint32>(ValidationLayerIdentifiers.size());

				appCreateSpec.EnabledLayerNames = ValidationLayerIdentifiers.data();

				PopulateDebugMessengerCreateInfo(DebugMessenger_CreationSpec);

				appCreateSpec.Extension = ptr<Messenger::CreateInfo>(getAddress(DebugMessenger_CreationSpec));

				//AppCreateSpec.Extension = nullptr;
			}
			else
			{
				appCreateSpec.EnabledLayerCount = 0;

				appCreateSpec.Extension = nullptr;
			}

			EResult&& creationResult = AppInstance_Create(appCreateSpec, nullptr, App);
			//EResult&& creationResult = EResult(vkCreateInstance(&VkInstanceCreateInfo(appCreateSpec), nullptr, getAddress(App)));

			if (creationResult != EResult::Success) 
				throw std::runtime_error("Triangle Test: Failed to create Vulkan app instance.");
		}

		void CreateLogicalDevice()	
		{
			QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);

			using LogicalDevice_QueueCreateInfoList = std::vector<LogicalDevice::Queue::CreateInfo>;

			LogicalDevice_QueueCreateInfoList queueCreateInfos;

			using IndiceSet = std::set<uint32_t>;

			IndiceSet queueFamiliesToCreate =
			{
				indices.GraphicsFamily.value(),
				indices.PresentationFamily.value()
			};

			float queuePriority = 1.0f;

			for (uint32_t queueFamily : queueFamiliesToCreate)
			{
				LogicalDevice::Queue::CreateInfo queueCreateInfo{};

				queueCreateInfo.SType = EStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

				queueCreateInfo.QueueFamilyIndex = queueFamily;

				queueCreateInfo.QueueCount = 1; 

				queueCreateInfo.QueuePriorities = &queuePriority;

				queueCreateInfos.push_back(queueCreateInfo);
			}

			//queueCreateInfo.QueuePriorities = &queuePriority;

			PhysicalDevice::Features physDeviceFeatures{};

			LogicalDevice::CreateInfo createInfo{};

			createInfo.SType = EStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

			createInfo.QueueCreateInfoCount = queueCreateInfos.size();

			createInfo.QueueCreateInfos = queueCreateInfos.data();

			createInfo.EnabledFeatures = &physDeviceFeatures;

			createInfo.EnabledExtensionNames = DeviceExtensions.data();

			createInfo.EnabledExtensionCount = DeviceExtensions.size();

			if (Vulkan_ValidationLayersEnabled)
			{
				createInfo.EnabledLayerCount = SCast<uint32>(ValidationLayerIdentifiers.size());

				createInfo.EnabledLayerNames = ValidationLayerIdentifiers.data();
			}
			else
			{
				createInfo.EnabledLayerCount = 0;
			}

			EResult&& result = LogicalDevice_CreateDevice(PhysicalDevice, createInfo, nullptr, LogicalDevice);

			if (result != EResult::Success)
			{
				throw std::runtime_error("Vulkan, TraingleTest: Failed to create logical device!");
			}

			LogicalDevice_GetQueue(LogicalDevice, indices.GraphicsFamily    .value(), 0, GraphicsQueue    );
			LogicalDevice_GetQueue(LogicalDevice, indices.PresentationFamily.value(), 0, PresentationQueue);
		}

		// TODO: Wrap.
		void CreateRenderPass()
		{
			VkAttachmentDescription colorAttachment {};

			colorAttachment.format = VkFormat(SwapChain_ImageFormat);
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference colorAttachmentRef{};

			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass {};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;

			VkRenderPassCreateInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = 1;
			renderPassInfo.pAttachments = &colorAttachment;
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;

			
			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;

			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;

			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			renderPassInfo.dependencyCount = 1;
			renderPassInfo.pDependencies = &dependency;


			if (vkCreateRenderPass(LogicalDevice, &renderPassInfo, nullptr, &RenderPass) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create render pass!");
			}
		}

		void CreateSurface()
		{
			Surface::CreateInfo createInfo{};

			createInfo.SType = OS_SurfaceCreateInfoEnum;

			createInfo.OSWinHandle = SAL::GLFW::GetOS_WindowHandle(TriangleWindow);

			createInfo.OSAppHandle = GetOS_AppHandle();

			if (Vulkan::CreateSurface(App, createInfo, nullptr, SurfaceHandle) != EResult::Success) 
			{
				throw std::runtime_error("Vulkan, TriangleTest: Failed to create window surface!");
			}

			/*if (EResult(SAL::GLFW::CreateWindowSurface(App, TriangleWindow, nullptr, SurfaceHandle)) != EResult::Success)
			{
				throw std::runtime_error("Vulkan, TriangleTest: Failed to create window surface!");
			}*/
		}

		void CreateSwapChain()
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(PhysicalDevice);

			Surface::Format surfaceFormat = Surface_SwapChain_ChooseFormat(swapChainSupport.Formats);

			EPresentationMode presentationMode = Surface_SwapChain_ChoosePresentationMode(swapChainSupport.PresentationModes);

			Extent2D extent = Surface_SwapChain_ChooseExtent(swapChainSupport.Capabilities);

			uint32 numImagesDesired = swapChainSupport.Capabilities.MinImageCount;

			numImagesDesired += 1;

			if (swapChainSupport.Capabilities.MaxImageCount > 0 && numImagesDesired > swapChainSupport.Capabilities.MaxImageCount)
			{
				numImagesDesired = swapChainSupport.Capabilities.MaxImageCount;
			}

			SwapChain::CreateInfo creationSpec {};

			creationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			creationSpec.Surface = SurfaceHandle;
			creationSpec.MinImageCount = numImagesDesired;
			creationSpec.ImageFormat = surfaceFormat.Format;
			creationSpec.ImageColorSpace = surfaceFormat.ColorSpace;
			creationSpec.ImageExtent = extent;
			creationSpec.ImageArrayLayers = 1;
			creationSpec.ImageUsage.Set(EImageUsage::Color_Attachment);

			QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);

			uint32_t queueFamilyIndices[] = 
			{
				indices.GraphicsFamily    .value(), 
				indices.PresentationFamily.value() 
			};

			if (indices.GraphicsFamily != indices.PresentationFamily) 
			{
				creationSpec.ImageSharingMode      = ESharingMode::Concurrent;
				creationSpec.QueueFamilyIndexCount = 2;
				creationSpec.QueueFamilyIndices    = queueFamilyIndices;
			}
			else 
			{
				creationSpec.ImageSharingMode      = ESharingMode::Excusive;
				creationSpec.QueueFamilyIndexCount = 0                     ; // Optional
				creationSpec.QueueFamilyIndices    = nullptr               ; // Optional
			}

			creationSpec.PreTransform = swapChainSupport.Capabilities.CurrentTransform;

			creationSpec.CompositeAlpha = ECompositeAlpha::Opaque;

			creationSpec.PresentationMode = presentationMode;

			creationSpec.Clipped = true;

			creationSpec.OldSwapchain = SwapChain::NullHandle();

			EResult&& creationResult = CreateSwapChain(LogicalDevice, creationSpec, nullptr, SwapChain);

			if (creationResult != EResult::Success)
			{
				throw std::runtime_error("Failed to create the swap chain!");
			}

			SwapChain_Images.resize(SwapChain_GetImageCount(LogicalDevice, SwapChain));

			SwapChain_GetImages(LogicalDevice, SwapChain, SwapChain_Images.data());

			

			SwapChain_ImageFormat = surfaceFormat.Format;

			SwapChain_Extent = extent;
		}

		// TODO: Wrap
		void CreateSyncObjects()
		{
			ImageAvailable_Semaphores.resize(MaxFramesInFlight);
			RenderFinished_Semaphores.resize(MaxFramesInFlight);

			InFlightFences.resize(MaxFramesInFlight                      );
			ImagesInFlight.resize(SwapChain_Images.size(), VK_NULL_HANDLE);

			VkSemaphoreCreateInfo semaphore_CreationSpec {};

			semaphore_CreationSpec.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fence_CreationSpec {};

			fence_CreationSpec.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

			fence_CreationSpec.flags = VK_FENCE_CREATE_SIGNALED_BIT;


			for (size_t index = 0; index < MaxFramesInFlight; index++)
			{
				if
				(
					vkCreateSemaphore(LogicalDevice, &semaphore_CreationSpec, nullptr, &ImageAvailable_Semaphores[index]) != VK_SUCCESS ||
					vkCreateSemaphore(LogicalDevice, &semaphore_CreationSpec, nullptr, &RenderFinished_Semaphores[index]) != VK_SUCCESS ||
					vkCreateFence    (LogicalDevice, &fence_CreationSpec    , nullptr, &InFlightFences           [index]) != VK_SUCCESS
				)
				{

					throw std::runtime_error("Failed to create synchronization objects for a frame!");
				}
			}
		}

		ShaderModule::Handle CreateTriShaderModule(const IO::FileBuffer& code)
		{
			using ByteCode = uint32;

			ShaderModule::CreateInfo creationSpec{};

			creationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

			creationSpec.Extension = NULL;

			creationSpec.CodeSize = code.size();

			creationSpec.Code = RCast<const ByteCode>(code.data());

			ShaderModule::Handle createdModule;

			EResult&& creationResult = ShaderModule_Create(LogicalDevice, creationSpec, nullptr, createdModule);

			if (creationResult != EResult::Success)
			{
				throw RuntimeError("Failed to create TriShader module!");
			}

			return createdModule;
		}

		Bool DebugCallback
		(
			      MessageServerityFlags   _messageServerity, 
			      MessageTypeFlags        _messageType     ,
			const Messenger::CallbackData _callbackData    , 
			      ptr<void>               _userData
		)
		{
			std::cerr << "Vulkan: Validation Layer: " << _callbackData.Message << std::endl;

			using Vulkan::EBool;

			return EBool::True;
		}

		QueueFamilyIndices FindQueueFamilies(PhysicalDevice::Handle _deviceHandle)
		{
			QueueFamilyIndices indices{};

			using QueueFamilyPropertiesListing = std::vector<PhysicalDevice::QueueFamilyProperties>;

			uint32 queueFamilyCount = GetPhysicalDevice_NumOf_QueueFamilyProperties(_deviceHandle);

			QueueFamilyPropertiesListing queueFamilies(queueFamilyCount);

			GetPhysicalDevice_QueueFamilyProperties(_deviceHandle, queueFamilies.data());

			int index = 0;

			for (const auto& queueFamily : queueFamilies)
			{
				if (queueFamily.QueueFlags.Has(EQueue::Graphics))
				{
					indices.GraphicsFamily = index;
				}

				Bool presentationSupport = EBool::False;

				Surface_CheckPhysicalDeviceSupport(_deviceHandle, index, SurfaceHandle, presentationSupport);

				if (presentationSupport)
				{
					indices.PresentationFamily = index;
				}

				if (indices.IsComplete())
				{
					break;
				}

				index++;

				if (index == queueFamilyCount)
				{
					break;
				}
			}

			return indices;
		}

		ExtensionIdentifierList GetRequiredExtensions()
		{
			stack<uint32> numExtensions;

			stack<CStrArray> extensionsRequired = CStrArray(SAL::GLFW::GetRequiredVulkanAppExtensions(numExtensions) );

			ExtensionIdentifierList extensions(extensionsRequired, extensionsRequired + numExtensions);

			if (Vulkan_ValidationLayersEnabled)
			{
				extensions.push_back(Vulkan::Extension_DebugUtility);
			}

			return extensions;
		}

		bool IsDeviceSuitable(PhysicalDevice::Handle _deviceHandle)
		{
			stack<PhysicalDevice::Properties> deviceProperties;

			GetPhysicalDevice_Properties(_deviceHandle, deviceProperties);

			stack<PhysicalDevice::Features> deviceFeatures;

			GetPhysicalDevice_Features(_deviceHandle, deviceFeatures);

			QueueFamilyIndices indices = FindQueueFamilies(_deviceHandle);

			bool extensionsSupported = PhysicalDevice_CheckExtensionSupport(_deviceHandle);

			bool swapChainAdequate = false;

			if (extensionsSupported)
			{
				SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_deviceHandle);

				swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentationModes.empty();
			}

			bool&& result = 
				bool(deviceFeatures.GeometryShader) &&
				indices.IsComplete()                &&
				extensionsSupported                 &&
				swapChainAdequate                     ;

			return result;
		}

		bool PhysicalDevice_CheckExtensionSupport(PhysicalDevice::Handle _handle)
		{
			using ExtensionPropertiesList = std::vector<ExtensionProperties>;

			ExtensionPropertiesList availableExtensions(PhysicalDevice_GetNumOfAvailableExtensions(_handle));

			PhysicalDevice_GetAvailableExtensions(_handle, availableExtensions.data());

			using ExtensionNameSet = std::set<std::string>;

			ExtensionNameSet requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

			for (const auto& extension : availableExtensions)
			{
				requiredExtensions.erase(extension.ExtensionName);
			}

			bool&& isSupported = requiredExtensions.empty();

			return isSupported;
		}

		void PickPhysicalDevice()
		{
			stack<uint32> numDevices = GetNumOf_PhysicalDevices(App);

			if (numDevices == 0)
				throw std::runtime_error("Physical device count 0. No GPUs found with vulkan support.");

			using PhysicalDeviceList = std::vector<PhysicalDevice::Handle>;

			PhysicalDeviceList physicalDevices(numDevices);

			GetAvailablePhysicalDevices(App, physicalDevices.data());

			for (const auto& device : physicalDevices)
			{
				if (IsDeviceSuitable(device))
				{
					PhysicalDevice = device;

					break;
				}
			}

			if (PhysicalDevice == PhysicalDevice::NullHandle())
			{
				throw std::runtime_error("Not able to find suitable Vulkan supported GPU.");
			}
		}

		void PopulateDebugMessengerCreateInfo(Messenger::CreateInfo& _msngrCreateInfo)
		{
			_msngrCreateInfo.SType = EStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

			using EMaskS = decltype(_msngrCreateInfo.Serverity)::Enum;

			_msngrCreateInfo.Serverity.Set(EMaskS::Verbose, EMaskS::Warning, EMaskS::Error);

			

			using EMaskT = decltype(_msngrCreateInfo.Type)::Enum;

			_msngrCreateInfo.Type.Set(EMaskT::General, EMaskT::Validation, EMaskT::Performance);

			using addrs = decltype(&DebugCallback);

			_msngrCreateInfo.UserCallback = EnforceConvention(Vulkan::EnforcerID_Vulkan, DebugCallback);

			_msngrCreateInfo.UserData = nullptr;
		}

		SwapChainSupportDetails QuerySwapChainSupport(PhysicalDevice::Handle _deviceHandle)
		{
			SwapChainSupportDetails details;

			Surface_GetPhysicalDeviceCapabilities(_deviceHandle, SurfaceHandle, details.Capabilities);

			uint32 formatCount;

			formatCount = Surface_GetNumOf_AvailableFormats(_deviceHandle, SurfaceHandle);

			if (formatCount > 0)
			{
				details.Formats.resize(formatCount);

				Surface_GetAvailableFormats(_deviceHandle, SurfaceHandle, details.Formats.data());
			}

			uint32 presentationModeCount;

			presentationModeCount = Surface_GetNumOf_SupportedPresentationModes(_deviceHandle, SurfaceHandle);

			if (presentationModeCount > 0)
			{
				details.PresentationModes.resize(presentationModeCount);

				Surface_GetSupportedPresentationModes(_deviceHandle, SurfaceHandle, details.PresentationModes.data());
			}

			return details;
		}

		int RateDeviceSuitability(PhysicalDevice::Handle _deviceHandle)
		{
			stack<PhysicalDevice::Properties> deviceProperties;

			GetPhysicalDevice_Properties(_deviceHandle, deviceProperties);

			stack<PhysicalDevice::Features> deviceFeatures;

			GetPhysicalDevice_Features(_deviceHandle, deviceFeatures);

			int score = 0;

			deviceProperties.Type == EPhysicalDeviceType::DiscreteGPU ? score += 1000 : NULL;

			score += deviceProperties.LimitsSpec.MaxImageDimension2D;

			if (!deviceFeatures.GeometryShader) 
			{
				return 0;
			}

			return score;
		}
		
		void SetupDebugMessenger()
		{
			if (!Vulkan_ValidationLayersEnabled) return;

			stack<Messenger::CreateInfo> msngrCreateSpec {};

			PopulateDebugMessengerCreateInfo(msngrCreateSpec);

			EResult&& creationResult = CreateMessenger(App, msngrCreateSpec, nullptr, DebugMessenger_Handle);
			//EResult&& creationResult = EResult(CreateDebugUtilsMessengerEXT(App, &VkDebugUtilsMessengerCreateInfoEXT(msngrCreateSpec), nullptr, &MsngrHandle));

			if (creationResult != EResult::Success) throw std::runtime_error("Failed to setup debug messenger!");
		}

		Extent2D Surface_SwapChain_ChooseExtent(const Surface::Capabilities& _capabilities)
		{
			if (_capabilities.CurrentExtent.Width != UInt32Max)
			{
				return _capabilities.CurrentExtent;
			}
			else
			{
				stack<int> width, height;

				glfwGetFramebufferSize(TriangleWindow, &width, &height);

				Extent2D actualExtent = 
				{ 
					static_cast<uint32_t>(width ),
					static_cast<uint32_t>(height) 
				};

				actualExtent.Width  = std::clamp(actualExtent.Width , _capabilities.MinImageExtent.Width , _capabilities.MaxImageExtent.Width );
				actualExtent.Height = std::clamp(actualExtent.Height, _capabilities.MinImageExtent.Height, _capabilities.MaxImageExtent.Height);

				return actualExtent;
			}
		}

		Surface::Format Surface_SwapChain_ChooseFormat(const SurfaceFormatList& _availableFormats)
		{
			for (const auto& availableFormat : _availableFormats)
			{
				if 
				(
					availableFormat.Format     == EImageFormat::VK_FORMAT_B8G8R8A8_SRGB &&
					availableFormat.ColorSpace == EColorSpace::KHR_SRGB_NonLinear         
				)
				{
					return availableFormat;
				}
			}

			// Just pick the first format...
			return _availableFormats[0];
		}

		EPresentationMode Surface_SwapChain_ChoosePresentationMode(const SurfacePresentationModeList _surfacePresentationModes)
		{
			for (const auto& availablePresentationMode : _surfacePresentationModes)
			{
				if (availablePresentationMode == EPresentationMode::KHR_Mailbox)
				{
					return availablePresentationMode;
				}
			}

			return EPresentationMode::KHR_FIFO;
		}

		// TODO: Wrap
		void CleanupSwapChain()
		{
			for (size_t index = 0; index < SwapChain_Framebuffers.size(); index++) 
			{
				vkDestroyFramebuffer(LogicalDevice, SwapChain_Framebuffers[index], nullptr);
			}

			vkFreeCommandBuffers(LogicalDevice, CommandPool, static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());

			vkDestroyPipeline(LogicalDevice, GraphicsPipeline, nullptr);

			vkDestroyPipelineLayout(LogicalDevice, PipelineLayout, nullptr);

			vkDestroyRenderPass(LogicalDevice, RenderPass, nullptr);

			for (size_t index = 0; index < SwapChain_ImageViews.size(); index++) 
			{
				vkDestroyImageView(LogicalDevice, SwapChain_ImageViews[index], nullptr);
			}

			vkDestroySwapchainKHR(LogicalDevice, SwapChain, nullptr);
		}

		void FramebufferSizeCallback(Window* _window, int width, int height)
		{
			//auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
			// Not necessary, this isn't object oriented.

			FramebufferResized = true;
		}



		// Execution

		void InitWindow()
		{
			using namespace SAL::GLFW;

			Initalize();

			using SAL::GLFW::EBool;

			SetWindowCreationParameter(EWindowCreationParameter::ClientAPI, EClient_API::No_API);
			SetWindowCreationParameter(EWindowCreationParameter::Resizable, EBool      ::True  );

			TriangleWindow =
				MakeWindow(Width, Height, "AnEngine: Triangle Test", WindowedMode(), NotShared());

			//glfwSetWindowUserPointer(window, this);
			// Not necessary see FramebufferSizeCallback procedure.

			glfwSetWindowSizeCallback(TriangleWindow, FramebufferSizeCallback);   // TODO: Wrap
		}

		void InitVulkan()
		{
			CreateInstance();

			SetupDebugMessenger();

			CreateSurface();

			PickPhysicalDevice();

			CreateLogicalDevice();

			CreateSwapChain();

			CreateImageViews();

			CreateRenderPass();

			CreateGraphicsPipeline();

			CreateFrameBuffers();

			CreateCommandPool();

			CreateCommandBuffers();

			CreateSyncObjects();
		}

		void RecreateSwapChain()
		{
			// TODO: Wrap

			int width = 0, height = 0;

			glfwGetFramebufferSize(TriangleWindow, &width, &height);

			while (width == 0 || height == 0) 
			{
				glfwGetFramebufferSize(TriangleWindow, &width, &height);

				glfwWaitEvents();
			}

			vkDeviceWaitIdle(LogicalDevice);

			CleanupSwapChain();

			CreateSwapChain();

			CreateImageViews();
			CreateRenderPass();
			CreateGraphicsPipeline();
			CreateFrameBuffers();
			CreateCommandBuffers();
		}

		// TODO: Wrap
		void DrawFrame()
		{
			vkWaitForFences(LogicalDevice, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX);

			uint32 imageIndex;

			VkResult&& result = vkAcquireNextImageKHR(LogicalDevice, SwapChain, UINT64_MAX, ImageAvailable_Semaphores[CurrentFrame], VK_NULL_HANDLE, &imageIndex);
			
			if (result == VK_ERROR_OUT_OF_DATE_KHR) 
			{
				RecreateSwapChain();

				return;
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
			{
				throw std::runtime_error("Failed to acquire swap chain image!");
			}

			if (ImagesInFlight[imageIndex] != VK_NULL_HANDLE) 
				vkWaitForFences(LogicalDevice, 1, &ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);

			ImagesInFlight[imageIndex] = InFlightFences[CurrentFrame];


			VkSubmitInfo submitInfo {};

			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


			VkSemaphore waitSemaphores[] = { ImageAvailable_Semaphores[CurrentFrame] };

			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &CommandBuffers[imageIndex];


			VkSemaphore signalSemaphores[] = { RenderFinished_Semaphores[CurrentFrame] };

			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;


			vkResetFences(LogicalDevice, 1, &InFlightFences[CurrentFrame]);

			if (vkQueueSubmit(GraphicsQueue, 1, &submitInfo, InFlightFences[CurrentFrame]) != VK_SUCCESS) 
				throw std::runtime_error("Failed to submit draw command buffer!");


			VkPresentInfoKHR presentInfo{};

			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;


			VkSwapchainKHR swapChains[] = { SwapChain };

			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;
			presentInfo.pImageIndices = &imageIndex;

			presentInfo.pResults = nullptr; // Optional


			result = vkQueuePresentKHR(PresentationQueue, &presentInfo);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || FramebufferResized) 
			{
				FramebufferResized = false;

				RecreateSwapChain();
			}
			else if (result != VK_SUCCESS) 
			{
				throw std::runtime_error("failed to present swap chain image!");
			}

			CurrentFrame = (CurrentFrame + 1) % MaxFramesInFlight;
		}

		void MainLoop()
		{
			using namespace SAL::GLFW;

			while (! CanClose(TriangleWindow))
			{
				PollEvents();

				DrawFrame();
			}

			vkDeviceWaitIdle(LogicalDevice);   // TODO: Wrap
		}

		void Cleanup()
		{
			using namespace SAL::GLFW;

			CleanupSwapChain();

			for (size_t index = 0; index < MaxFramesInFlight; index++) 
			{
				vkDestroySemaphore(LogicalDevice, RenderFinished_Semaphores[index], nullptr);   // TODO: Wrap
				vkDestroySemaphore(LogicalDevice, ImageAvailable_Semaphores[index], nullptr);   // TODO: Wrap
				vkDestroyFence    (LogicalDevice, InFlightFences           [index], nullptr);   // TODO: Wrap
			}

			vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);   // TODO: Wrap

			LogicalDevice_Destory(LogicalDevice, nullptr);

			if (Vulkan_ValidationLayersEnabled) DestroyMessenger(App, DebugMessenger_Handle, nullptr);

			DestroySurface(App, SurfaceHandle, nullptr);

			AppInstance_Destory(App, nullptr);

			DestroyWindow(TriangleWindow);

			Terminate();
		}

		int Execute_VulkanTriTest()
		{
			try
			{
				InitWindow();

				InitVulkan();

				MainLoop();

				Cleanup();
			}
			catch (const std::exception& error)
			{
				std::cerr << error.what() << std::endl;

				getchar();

				return EXIT_FAILURE;
			}

			return EXIT_SUCCESS;
		}
	}
}

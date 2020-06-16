// Parent Header
#include "Vulkan_API.hpp"



// Includes


#include "Core/IO/Basic_FileIO.hpp"

#include "Renderer/Shader/TriangleShader/TriangleShader.hpp"



#if VULCAN_INTERFACE == VAULTED_THERMALS_INTERFACE

	namespace HAL::GPU
	{
		namespace Platform_Vulkan
		{
			// Static Data

			BSS
			(
				AppInstance::Handle AppStateRef;

				CommandBufferList CommandBuffers;
				VkCommandPool     CommandPool   ;   // TODO: Wrap

				//PhysicalDeviceList  PhysicalDevices      ;
				Messenger::Handle   DebugMessenger_Handle;

				LogicalDevice::Handle        LogicalDevice    ;
				LogicalDevice::Queue::Handle GraphicsQueue    ;
				LogicalDevice::Queue::Handle PresentationQueue;

				Pipeline::Layout::Handle PipelineLayout;

				VkPipeline GraphicsPipeline;

				FenceList InFlightFences;
				FenceList ImagesInFlight;

				SemaphoreList ImageAvailable_Semaphores;
				SemaphoreList RenderFinished_Semaphores;

				Surface::Handle SurfaceHandle;

				SwapChain::Handle SwapChain;

				Extent2D      SwapChain_Extent     ;
				ImageList     SwapChain_Images     ;
				EImageFormat  SwapChain_ImageFormat;
				ImageViewList SwapChain_ImageViews ;

				FrameBufferList SwapChain_Framebuffers;

				VkRenderPass RenderPass;   // TODO: Wrap.
			);

			Data
			(
				bool FramebufferResized = false;

				DataSize CurrentFrame = 0;

				sInt32 MaxFramesInFlight = 2;

				ExtensionIdentifierList> DeviceExtensions =
				{
					Swapchain_ExtensionName
				};

				PhysicalDevice::Handle PhysicalDevice = PhysicalDevice::NullHandle();

				ValidationLayerList ValidationLayerIdentifiers = 
				{
					ValidationLayer_Khronos
				};
			);


			// Forwards

			ShaderModule::Handle CreateTriShaderModule (LogicalDevice::Handle _logicalDevice, const IO::FileBuffer& code);
			Bool                 DebugCallback_Internal(MessageServerityFlags _messageServerity, MessageTypeFlags _messageType, const Messenger::CallbackData _callbackData, ptr<void> _userData);



			// Functions

			bool CheckValidationLayerSupport(ValidationLayerList& _validationLayersSpecified)
			{
				using LayerPropertyList = std::vector<LayerProperties>;


				uInt32&& layerCount = GetNumOf_AvailableGlobalLayerProperties();

				stack<LayerPropertyList> availableLayers(layerCount);

				GetAvailableGlobalLayerProperties(availableLayers.data());


				stack<bool> layerFound = false;

				for (auto validationLayerName : _validationLayersSpecified)
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
			void CleanupSwapChain
			(

			)
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

			void CreateApplicationInstance(RoCStr _appName, AppVersion _version, ValidationLayerList _optionalValidationLayers)
			{
				using Meta::EEngineVersion;

				Stack
				(
					AppInstance::AppInfo    appSpec       {};
					AppInstance::CreateInfo appCreateSpec {};
				);

				if (Vulkan_ValidationLayersEnabled)
				{
					if (!CheckValidationLayerSupport(_optionalValidationLayers))
						throw std::runtime_error("Validation layers requested, but are not available!");
				}

				//AppSpec.SType         = EStructureType::ApplicationInformation;   // Use later...
				appSpec.SType         = EStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO         ;
				appSpec.AppName       = _appName                                                   ;
				appSpec.AppVersion    = MakeVersion(_version.Major, _version.Minor, _version.Patch);
				appSpec.EngineName    = Meta::EngineName                                           ;
				appSpec.EngineVersion = MakeVersion
				(
					EEngineVersion::Major, 
					EEngineVersion::Minor, 
					EEngineVersion::Patch
				)                                         ;
				appSpec.API_Version   = EAPI_Version::_1_0;

				//AppCreateSpec.SType   = EStructureType::InstanceCreateInfo;
				appCreateSpec.SType   = EStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
				appCreateSpec.AppInfo = getAddress(appSpec)       ;

				ExtensionIdentifierList extensions = GetRequiredExtensions();

				appCreateSpec.EnabledExtensionCount = SCast<uint32>(extensions.size());
				appCreateSpec.EnabledExtensionNames = extensions.data();


				if (Vulkan_ValidationLayersEnabled)
				{
					appCreateSpec.EnabledLayerCount = SCast<uint32>(_optionalValidationLayers.size());

					appCreateSpec.EnabledLayerNames = _optionalValidationLayers.data();

					Messenger::CreateInfo debugMessenger_CreationSpec;

					PopulateDebugMessengerCreateInfo(debugMessenger_CreationSpec);

					appCreateSpec.Extension = ptr<Messenger::CreateInfo>(getAddress(debugMessenger_CreationSpec));
				}
				else
				{
					appCreateSpec.EnabledLayerCount = 0;

					appCreateSpec.Extension = nullptr;
				}

				EResult&& creationResult = AppInstance_Create(appCreateSpec, nullptr, AppStateRef);

				if (creationResult != EResult::Success) 
					throw std::runtime_error("Triangle Test: Failed to create Vulkan app instance.");
			}

			// TODO: Wrap.
			void CreateCommandBuffers
			(
				LogicalDevice::Handle _logicalDevice         ,

				VkPipeline            _graphicsPipeline      ,
				
				FrameBufferList&      _swapChain_FrameBuffers,
				Extent2D              _swapChain_Extent      ,
				VkRenderPass          _renderPass            ,

				VkCommandPool         _commandPool           ,
				CommandBufferList&    _commandBufferContainer
			)
			{
				_commandBufferContainer.resize(_swapChain_FrameBuffers.size());

				VkCommandBufferAllocateInfo allocSpec {};

				allocSpec.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocSpec.commandPool        = _commandPool                                  ;
				allocSpec.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY               ;
				allocSpec.commandBufferCount = uint32_t(_commandBufferContainer.size())      ;

				if (vkAllocateCommandBuffers(_logicalDevice, &allocSpec, _commandBufferContainer.data()) != VK_SUCCESS) 
				{
					throw std::runtime_error("failed to allocate command buffers!");
				}

				for (size_t index = 0; index < _commandBufferContainer.size(); index++) 
				{
					VkCommandBufferBeginInfo beginInfo {};

					beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
					beginInfo.flags            = 0                                          ; // Optional
					beginInfo.pInheritanceInfo = nullptr                                    ; // Optional

					if (vkBeginCommandBuffer(_commandBufferContainer[index], &beginInfo) != VK_SUCCESS) 
					{
						throw std::runtime_error("Failed to begin recording command buffer!");
					}

					VkRenderPassBeginInfo renderPassInfo{};

					renderPassInfo.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
					renderPassInfo.renderPass  = _renderPass                             ;
					renderPassInfo.framebuffer = _swapChain_FrameBuffers[index]          ;

					renderPassInfo.renderArea.offset = { 0, 0 }         ;
					renderPassInfo.renderArea.extent = _swapChain_Extent;

					VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

					renderPassInfo.clearValueCount = 1          ;
					renderPassInfo.pClearValues    = &clearColor;

					vkCmdBeginRenderPass(_commandBufferContainer[index], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

						vkCmdBindPipeline(_commandBufferContainer[index], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

						vkCmdDraw(_commandBufferContainer[index], 3, 1, 0, 0);

					vkCmdEndRenderPass(_commandBufferContainer[index]);

					if (vkEndCommandBuffer(_commandBufferContainer[index]) != VK_SUCCESS) 
					{
						throw std::runtime_error("Failed to record command buffer!");
					}
				}
			}

			void CreateCommandPool(PhysicalDevice::Handle _physicalDevice, Surface::Handle _surfaceHandle, VkCommandPool _commandPool)
			{
				QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(_physicalDevice, _surfaceHandle);

				VkCommandPoolCreateInfo poolInfo {};

				poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();
				poolInfo.flags            = 0; // Optional

				if (vkCreateCommandPool(LogicalDevice, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) 
				{
					throw std::runtime_error("failed to create command pool!");
				}
			}

			void CreateFrameBuffers
			(
				LogicalDevice::Handle _logicalDevice        ,
				VkRenderPass          _renderPass           ,
				Extent2D              _swapChainExtent      ,
				ImageViewList         _swapChainImageViews  ,
				FrameBufferList       _swapChainFrameBuffers
			)
			{
				_swapChainFrameBuffers.resize(_swapChainImageViews.size());

				for (size_t index = 0; index < _swapChainImageViews.size(); index++) 
				{
					VkImageView attachments[] = 
					{
						_swapChainImageViews[index]
					};

					VkFramebufferCreateInfo framebufferInfo {};

					framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
					framebufferInfo.renderPass      = _renderPass;
					framebufferInfo.attachmentCount = 1;
					framebufferInfo.pAttachments    = attachments;
					framebufferInfo.width           = _swapChainExtent.Width;
					framebufferInfo.height          = _swapChainExtent.Height;
					framebufferInfo.layers          = 1;

					if (vkCreateFramebuffer(_logicalDevice, &framebufferInfo, nullptr, &_swapChainFrameBuffers[index]) != VK_SUCCESS) 
					{
						throw std::runtime_error("Failed to create framebuffer!");
					}
				}
			}

			/*
			Creates a simple graphics pipline

			Note: This was enough
			*/
			void CreateGraphicsPipeline
			(
				LogicalDevice::Handle    _logicalDevice   ,
				Extent2D                 _swapChainExtent ,
				VkRenderPass             _renderPass      ,
				Pipeline::Layout::Handle _pipelineLayout  ,
				VkPipeline               _graphicsPipeline
			)
			{
				using Bytecode_Buffer = DynamicArray< SPIR_V::Bytecode>;

				// Shader setup

				using namespace Renderer::Shader;

				auto triShader_VertCode = IO::BufferFile(String(Paths::TriangleShader) + "TriangleShader_Vert.spv");
				auto triShader_FragCode = IO::BufferFile(String(Paths::TriangleShader) + "TriangleShader_Frag.spv");

				//TODO: FIGURE THIS OUT.
				Bytecode_Buffer triShader_Vert_Bytecode(triShader_VertCode.begin(), triShader_VertCode.end());
				Bytecode_Buffer triShader_Frag_Bytecode(triShader_FragCode.begin(), triShader_FragCode.end());

				ShaderModule::Handle triShaderModule_Vert = CreateTriShaderModule(_logicalDevice, triShader_VertCode);
				ShaderModule::Handle triShaderModule_Frag = CreateTriShaderModule(_logicalDevice, triShader_FragCode);

				Pipeline::ShaderStage::CreateInfo triShaderStage_Vert_CreationSpec{};
				Pipeline::ShaderStage::CreateInfo triShaderStage_Frag_CreationSpec{};

				triShaderStage_Vert_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				triShaderStage_Vert_CreationSpec.Stage = EShaderStageFlag::Vertex;

				triShaderStage_Vert_CreationSpec.Module = triShaderModule_Vert;
				triShaderStage_Vert_CreationSpec.Name   = "main";

				triShaderStage_Frag_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				triShaderStage_Frag_CreationSpec.Stage = EShaderStageFlag::Fragment;

				triShaderStage_Frag_CreationSpec.Module = triShaderModule_Frag;
				triShaderStage_Frag_CreationSpec.Name   = "main";

				Pipeline::ShaderStage::CreateInfo shaderStages[] = { triShaderStage_Vert_CreationSpec, triShaderStage_Frag_CreationSpec };

				// Fixed Function

				Pipeline::VertexInputState::CreateInfo vertexInputState_CreationSpec{};

				vertexInputState_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

				vertexInputState_CreationSpec.VertexBindingDescriptionCount = 0;
				vertexInputState_CreationSpec.BindingDescriptions           = nullptr;

				vertexInputState_CreationSpec.AttributeDescriptionCount = 0;
				vertexInputState_CreationSpec.AttributeDescription      = nullptr;

				Pipeline::InputAssembly::CreateInfo inputAssembly_CreationSpec{};

				inputAssembly_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

				inputAssembly_CreationSpec.Topology               = EPrimitiveTopology::TriangleList;
				inputAssembly_CreationSpec.PrimitiveRestartEnable = false;

				Viewport viewport{};

				viewport.X        = 0.0f;
				viewport.Y        = 0.0f;
				viewport.Width    = float(_swapChainExtent.Width);
				viewport.Height   = float(_swapChainExtent.Height);
				viewport.MinDepth = 0.0f;
				viewport.MaxDepth = 1.0f;

				Rect2D scissor{};

				scissor.Offset = { 0,0 };
				scissor.Extent = _swapChainExtent;

				// TODO: Wrap
				VkPipelineViewportStateCreateInfo viewportState_CreationSpec{};

				viewportState_CreationSpec.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				viewportState_CreationSpec.viewportCount = 1;
				viewportState_CreationSpec.pViewports    = (VkViewport*)(&viewport);
				viewportState_CreationSpec.scissorCount  = 1;
				viewportState_CreationSpec.pScissors     = (VkRect2D*)(&scissor);

				Pipeline::RasterizationState::CreateInfo rasterizer{};

				rasterizer.SType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rasterizer.EnableDepthClamp        = EBool::False;
				rasterizer.EnableRasterizerDiscard = EBool::False;
				rasterizer.PolygonMode             = EPolygonMode::Fill;
				rasterizer.LineWidth               = 1.0f;

				rasterizer.CullMode.Set(ECullModeFlag::Back);

				rasterizer.FrontFace               = EFrontFace::Clockwise;
				rasterizer.EnableDepthBias         = EBool::False;
				rasterizer.DepthBiasConstantFactor = 0.0f;
				rasterizer.DepthBiasClamp          = 0.0f;
				rasterizer.DepthBiasSlopeFactor    = 0.0f;

				Pipeline::MultisampleState::CreateInfo multisampling_CreationSpec{};

				multisampling_CreationSpec.SType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				multisampling_CreationSpec.EnableSampleShading   = EBool::False;
				multisampling_CreationSpec.RasterizationSamples  = ESampleCount::_1;
				multisampling_CreationSpec.MinSampleShading      = 1.0f;
				multisampling_CreationSpec.SampleMask            = nullptr;
				multisampling_CreationSpec.EnableAlphaToCoverage = EBool::False;
				multisampling_CreationSpec.EnableAlphaToOne      = EBool::False;

				Pipeline::ColorBlend::AttachmentState colorBlend_Attachment{};

				colorBlend_Attachment.ColorWriteMask.Set(EColorComponentFlag::R, EColorComponentFlag::G, EColorComponentFlag::B, EColorComponentFlag::A);

				colorBlend_Attachment.EnableBlend                  = EBool::False;
				colorBlend_Attachment.Source_ColorBlendFactor      = EBlendFactor::One;
				colorBlend_Attachment.Destination_ColorBlendFactor = EBlendFactor::Zero;
				colorBlend_Attachment.ColorOperation               = EBlendOperation::Add;
				colorBlend_Attachment.Source_AlphaBlendFactor      = EBlendFactor::One;
				colorBlend_Attachment.Destination_AlphaBlendFactor = EBlendFactor::Zero;
				colorBlend_Attachment.AlphaOperation               = EBlendOperation::Add;

				Pipeline::ColorBlend::CreateInfo colorBlending_CreationSpec{};

				colorBlending_CreationSpec.SType                 = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorBlending_CreationSpec.EnableLogicOperations = EBool::False;
				colorBlending_CreationSpec.LogicOperation        = ELogicOperation::Copy;
				colorBlending_CreationSpec.AttachmentCount       = 1;
				colorBlending_CreationSpec.Attachments           = &colorBlend_Attachment;
				colorBlending_CreationSpec.BlendConstants[0]     = 0.0f;
				colorBlending_CreationSpec.BlendConstants[1]     = 0.0f;
				colorBlending_CreationSpec.BlendConstants[2]     = 0.0f;
				colorBlending_CreationSpec.BlendConstants[3]     = 0.0f;

				EDynamicState States[] =
				{
					EDynamicState::Viewport,
					EDynamicState::LineWidth
				};

				Pipeline::DynamicState::CreateInfo dynamicState {};

				dynamicState.SType      = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				dynamicState.StateCount = 2;
				dynamicState.States     = States;

				Pipeline::Layout::CreateInfo pipelineLayout_CreationSpec {};

				pipelineLayout_CreationSpec.SType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				//pipelineLayout_CreationSpec.Extension = nullptr;
				pipelineLayout_CreationSpec.SetLayoutCount         = 0;
				pipelineLayout_CreationSpec.SetLayouts             = nullptr;
				pipelineLayout_CreationSpec.PushConstantRangeCount = 0;
				pipelineLayout_CreationSpec.PushConstantRanges     = nullptr;
				//pipelineLayout_CreationSpec.Flags = 0;

				//VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
				//pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				//pipelineLayoutInfo.setLayoutCount = 0; // Optional
				//pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
				//pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
				//pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
				//pipelineLayoutInfo.

				EResult&& piplineLayout_CreationResult = 
					Pipeline_CreateLayout(_logicalDevice, pipelineLayout_CreationSpec, nullptr, _pipelineLayout);

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

				pipelineInfo.layout = (VkPipelineLayout)(_pipelineLayout);

				pipelineInfo.renderPass = _renderPass;
				pipelineInfo.subpass    = 0;

				pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
				pipelineInfo.basePipelineIndex  = -1; // Optional

				if (vkCreateGraphicsPipelines(_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) 
				{
					throw std::runtime_error("Failed to create graphics pipeline!");
				}

				ShaderModule_Destory(_logicalDevice, triShaderModule_Vert, nullptr);
				ShaderModule_Destory(_logicalDevice, triShaderModule_Frag, nullptr);
			}	

			void CreateImageViews
			(
				LogicalDevice::Handle _logicalDevice       ,
				ImageList             _swapChainImages     , 
				EImageFormat          _swapChainImageFormat,
				ImageViewList         _imageViewContainer
			)
			{
				_imageViewContainer.resize(_swapChainImages.size());

				for (DataSize index = 0; index < _swapChainImages.size(); index++)
				{
					ImageView::CreateInfo creationSpec {};

					creationSpec.SType    = EStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					creationSpec.Image    = _swapChainImages[index];
					creationSpec.ViewType = EImageViewType::_2D;
					creationSpec.Format   = _swapChainImageFormat;

					creationSpec.Components.R = EComponentSwizzle::Identitity;
					creationSpec.Components.G = EComponentSwizzle::Identitity;
					creationSpec.Components.B = EComponentSwizzle::Identitity;
					creationSpec.Components.A = EComponentSwizzle::Identitity;

					creationSpec.SubresourceRange.AspectMask.Set(EImageAspect::Color);

					creationSpec.SubresourceRange.BaseMipLevel   = 0;
					creationSpec.SubresourceRange.LevelCount     = 1;
					creationSpec.SubresourceRange.BaseArrayLayer = 0;
					creationSpec.SubresourceRange.LayerCount     = 1;

					EResult&& creationResult = CreateImageView(_logicalDevice, creationSpec, nullptr, &_imageViewContainer[index]);

					if (creationResult != EResult::Success)
					{
						throw std::runtime_error("Failed to create image views!");
					}
				}
			}

			void CreateLogicalDevice
			(
				PhysicalDevice::Handle  _physicalDevice     , 
				
				Surface::Handle         _surfaceHanle       ,
				ExtensionIdentifierList _extensionsToSpecify,
				ValidationLayerList _optionalValidationLayers,
				LogicalDevice::Handle& _logicalDevice,
				LogicalDevice::Queue::Handle& _graphicsQueue,
				LogicalDevice::Queue::Handle& _presentationQueue
			)	
			{
				QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice, _surfaceHanle);

				using LogicalDevice_QueueCreateInfoList = std::vector<LogicalDevice::Queue::CreateInfo>;

				LogicalDevice_QueueCreateInfoList queueCreateInfos;

				using IndiceSet = std::set<uint32_t>;

				IndiceSet queueFamiliesToCreate =
				{
					indices.GraphicsFamily    .value(),
					indices.PresentationFamily.value()
				};

				float queuePriority = 1.0f;

				for (uint32_t queueFamily : queueFamiliesToCreate)
				{
					LogicalDevice::Queue::CreateInfo queueCreateInfo{};

					queueCreateInfo.SType            = EStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					queueCreateInfo.QueueFamilyIndex = queueFamily;
					queueCreateInfo.QueueCount       = 1; 
					queueCreateInfo.QueuePriorities  = &queuePriority;

					queueCreateInfos.push_back(queueCreateInfo);
				}

				//queueCreateInfo.QueuePriorities = &queuePriority;

				PhysicalDevice::Features physDeviceFeatures{};

				LogicalDevice::CreateInfo createInfo{};

				createInfo.SType                 = EStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				createInfo.QueueCreateInfoCount  = queueCreateInfos.size();
				createInfo.QueueCreateInfos      = queueCreateInfos.data();
				createInfo.EnabledFeatures       = &physDeviceFeatures;
				createInfo.EnabledExtensionNames = _extensionsToSpecify.data();
				createInfo.EnabledExtensionCount = _extensionsToSpecify.size();

				if (Vulkan_ValidationLayersEnabled)
				{
					createInfo.EnabledLayerCount = SCast<uint32>(_optionalValidationLayers.size());
					createInfo.EnabledLayerNames = _optionalValidationLayers.data();
				}
				else
				{
					createInfo.EnabledLayerCount = 0;
				}

				EResult&& result = LogicalDevice_CreateDevice(_physicalDevice, createInfo, nullptr, _logicalDevice);

				if (result != EResult::Success)
				{
					throw std::runtime_error("Vulkan, TraingleTest: Failed to create logical device!");
				}

				LogicalDevice_GetQueue(_logicalDevice, indices.GraphicsFamily    .value(), 0, _graphicsQueue    );
				LogicalDevice_GetQueue(_logicalDevice, indices.PresentationFamily.value(), 0, _presentationQueue);
			}

			// TODO: Wrap.
			void CreateRenderPass
			(
				LogicalDevice::Handle _logicalDevice,
				EImageFormat _imageFormat,
				VkRenderPass&         _renderPass
			)
			{
				VkAttachmentDescription colorAttachment {};

				colorAttachment.format  = VkFormat(_imageFormat);
				colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

				colorAttachment.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
				colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

				colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

				colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				colorAttachment.finalLayout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				VkAttachmentReference colorAttachmentRef{};

				colorAttachmentRef.attachment = 0;
				colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				VkSubpassDescription subpass {};

				subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.colorAttachmentCount = 1;
				subpass.pColorAttachments    = &colorAttachmentRef;

				VkRenderPassCreateInfo renderPassInfo{};

				renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassInfo.attachmentCount = 1;
				renderPassInfo.pAttachments    = &colorAttachment;
				renderPassInfo.subpassCount    = 1;
				renderPassInfo.pSubpasses      = &subpass;


				VkSubpassDependency dependency{};

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;

				dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = 0;

				dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				renderPassInfo.dependencyCount = 1;
				renderPassInfo.pDependencies   = &dependency;


				if (vkCreateRenderPass(_logicalDevice, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create render pass!");
				}
			}

			void CreateSurface
			(
				AppInstance::Handle _applicationInstance,
				Surface::Handle     _surfaceHandle      ,
				Window              _window 
			)
			{
				Surface::CreateInfo createInfo{};

				createInfo.SType       = OS_SurfaceCreateInfoEnum;
				createInfo.OSWinHandle = SAL::GLFW::GetOS_WindowHandle(_window);
				createInfo.OSAppHandle = GetOS_AppHandle();

				if (Vulkan::CreateSurface(_applicationInstance, createInfo, nullptr, _surfaceHandle) != EResult::Success) 
				{
					throw std::runtime_error("Vulkan, TriangleTest: Failed to create window surface!");
				}

				/*if (EResult(SAL::GLFW::CreateWindowSurface(App, TriangleWindow, nullptr, SurfaceHandle)) != EResult::Success)
				{
				throw std::runtime_error("Vulkan, TriangleTest: Failed to create window surface!");
				}*/
			}

			void CreateSwapChain
			(
				PhysicalDevice::Handle _physicalDevice,
				LogicalDevice::Handle  _logicalDevice,
				Surface::Handle        _surfaceHandle,
				SwapChain::Handle&     _swapChain,
				EImageFormat&          _swapChainImageFormat,
				Extent2D&              _swapChainExtent,
				ImageList&             _swapChain_Images
			)
			{
				SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_physicalDevice);
				Surface::Format         surfaceFormat    = Surface_SwapChain_ChooseFormat(swapChainSupport.Formats);
				EPresentationMode       presentationMode = Surface_SwapChain_ChoosePresentationMode(swapChainSupport.PresentationModes);
				Extent2D                extent           = Surface_SwapChain_ChooseExtent(swapChainSupport.Capabilities);
				uint32                  numImagesDesired = swapChainSupport.Capabilities.MinImageCount;

				numImagesDesired += 1;

				if (swapChainSupport.Capabilities.MaxImageCount > 0 && numImagesDesired > swapChainSupport.Capabilities.MaxImageCount)
				{
					numImagesDesired = swapChainSupport.Capabilities.MaxImageCount;
				}

				SwapChain::CreateInfo creationSpec {};

				creationSpec.SType            = EStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				creationSpec.Surface          = _surfaceHandle;
				creationSpec.MinImageCount    = numImagesDesired;
				creationSpec.ImageFormat      = surfaceFormat.Format;
				creationSpec.ImageColorSpace  = surfaceFormat.ColorSpace;
				creationSpec.ImageExtent      = extent;
				creationSpec.ImageArrayLayers = 1;

				creationSpec.ImageUsage.Set(EImageUsage::Color_Attachment);

				QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice);

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

				creationSpec.PreTransform     = swapChainSupport.Capabilities.CurrentTransform;
				creationSpec.CompositeAlpha   = ECompositeAlpha::Opaque;
				creationSpec.PresentationMode = presentationMode;
				creationSpec.Clipped          = true;
				creationSpec.OldSwapchain     = SwapChain::NullHandle();

				EResult&& creationResult = CreateSwapChain(_logicalDevice, creationSpec, nullptr, _swapChain);

				if (creationResult != EResult::Success)
				{
					throw std::runtime_error("Failed to create the swap chain!");
				}

				_swapChain_Images.resize(SwapChain_GetImageCount(_logicalDevice, _swapChain));

				SwapChain_GetImages(_logicalDevice, _swapChain, _swapChain_Images.data());

				_swapChainImageFormat = surfaceFormat.Format;
				_swapChainExtent      = extent;
			}

			void CreateSyncObjects
			(

			)
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


			ShaderModule::Handle CreateTriShaderModule(LogicalDevice::Handle _logicalDevice, const IO::FileBuffer& code)
			{
				using ByteCode = uint32;

				ShaderModule::CreateInfo creationSpec{};

				creationSpec.SType     = EStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				creationSpec.Extension = NULL;
				creationSpec.CodeSize  = code.size();
				creationSpec.Code      = RCast<const ByteCode>(code.data());

				ShaderModule::Handle createdModule;

				EResult&& creationResult = ShaderModule_Create(_logicalDevice, creationSpec, nullptr, createdModule);

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
				return DebugCallback_Internal(_messageServerity, _messageType, _callbackData, _userData);
			}

			/*
			I had to make this to resolve an issue with a decltype function resolve error on EnforceConvention.
			*/
			Bool DebugCallback_Internal
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

			QueueFamilyIndices FindQueueFamilies
			(
				PhysicalDevice::Handle _deviceHandle ,
				Surface       ::Handle _surfaceHandle
			)
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

					Surface_CheckPhysicalDeviceSupport(_deviceHandle, index, _surfaceHandle, presentationSupport);

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

			void FramebufferSizeCallback(Window* _window, int width, int height)
			{
				//auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
				// Not necessary, this isn't object oriented.

				FramebufferResized = true;
			}

			ExtensionIdentifierList GetRequiredExtensions()
			{
				Stack
				(
					uint32    numExtensions      = 0; 
					CStrArray extensionsRequired    ;
				);

				extensionsRequired = GetRequiredVulkanAppExtensions(numExtensions);

				stack<ExtensionIdentifierList> extensions(extensionsRequired, extensionsRequired + numExtensions);

				if (Vulkan_ValidationLayersEnabled)
				{
					extensions.push_back(Vulkan::Extension_DebugUtility);
				}

				return extensions;
			}

			Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
			CStrArray> GetRequiredVulkanAppExtensions(uint32 _numExtensions)
			{
				return CStrArray(SAL::GLFW::GetRequiredVulkanAppExtensions(_numExtensions));
			}

			void InitializeVulkanApplication(RoCStr _appName, AppVersion _version)
			{
				CreateApplicationInstance(_appName, _version, ValidationLayerIdentifiers);
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

				_msngrCreateInfo.UserCallback = EnforceConvention(Vulkan::EnforcerID_Vulkan, DebugCallback_Internal);

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

				stack<Messenger::CreateInfo> msngrCreateSpec{};

				PopulateDebugMessengerCreateInfo(msngrCreateSpec);

				EResult&& creationResult = CreateMessenger(AppStateRef, msngrCreateSpec, nullptr, DebugMessenger_Handle);

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

					actualExtent.Width  = std::clamp (actualExtent.Width , _capabilities.MinImageExtent.Width , _capabilities.MaxImageExtent.Width );
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
		}
	}

#endif

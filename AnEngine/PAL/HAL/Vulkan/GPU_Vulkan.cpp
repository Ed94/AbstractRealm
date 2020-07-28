// Parent Header
#include "GPU_Vulkan.hpp"



// Includes



#include "Renderer/Shader/TriangleShader/TriangleShader.hpp"



#if VULCAN_INTERFACE == VAULTED_THERMALS_INTERFACE

	namespace HAL::GPU
	{
		namespace Platform_Vulkan
		{
			// Static Data

			//BSS
			//(
				AppInstance::Handle AppGPU;

				CommandBufferList CommandBuffers;
				VkCommandPool     CommandPool   ;   // TODO: Wrap

				//PhysicalDeviceList  PhysicalDevices      ;
				DebugMessenger::Handle     DebugMessenger_Handle;
				DebugMessenger::CreateInfo DebugMessenger_CreationSpec;

				LogicalDevice::Handle LogicalDevice    ;
				LogicalDevice::Queue::Handle         GraphicsQueue    ;
				LogicalDevice::Queue::Handle         PresentationQueue;

				Pipeline::Layout::Handle PipelineLayout;

				VkPipeline GraphicsPipeline;

				FenceList InFlightFences;
				FenceList ImagesInFlight;

				SemaphoreList ImageAvailable_Semaphores;
				SemaphoreList RenderFinished_Semaphores;

				RenderContextList RenderContextPool; 

				Surface::Handle SurfaceHandle;

				SwapChain::Handle SwapChain;

				Extent2D      SwapChain_Extent     ;
				ImageList     SwapChain_Images     ;
				EFormat  SwapChain_ImageFormat;
				ImageViewList SwapChain_ImageViews ;

				FrameBufferList SwapChain_Framebuffers;

				VkRenderPass RenderPass;   // TODO: Wrap.
			//);

			Data
			(
				
			);

			bool FramebufferResized = false;

			DataSize CurrentFrame = 0;

			sint32 MaxFramesInFlight = 2;

			ExtensionIdentifierList DeviceExtensions =
			{
				Swapchain_ExtensionName
			};

			PhysicalDevice::Handle PhysicalDevice = PhysicalDevice::NullHandle;

			ValidationLayerList ValidationLayerIdentifiers =
			{
				ValidationLayer_Khronos
			};


			// Forwards

			Bool DebugCallback_Internal
			(
				      MessageServerityFlags         _messageServerity, 
				      MessageTypeFlags              _messageType     ,
				const DebugMessenger::CallbackData  _callbackData    , 
				      ptr<void>                     _userData
			);


			// Functions

			bool CheckValidationLayerSupport(ValidationLayerList& _validationLayersSpecified)
			{
				using LayerPropertyList = std::vector<ValidationLayers::LayerProperties>;


				uInt32&& layerCount = ValidationLayers::GetNumberOfLayers();

				stack<LayerPropertyList> availableLayers(layerCount);

				ValidationLayers::GetAvailableLayers(availableLayers.data());


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
				LogicalDevice::Handle    _logicalDevice,
				Pipeline::Layout::Handle _pipelineLayout,
				VkPipeline               _graphicsPipeline,
				SwapChain::Handle        _swapChain,
				ImageViewList&           _swapChainImageViews,
				FrameBufferList&         _swapChainFramebuffers,
				VkRenderPass             _renderPass,
				VkCommandPool            _commandPool,
				CommandBufferList&       _commandBuffers
			)
			{
				for (DataSize index = 0; index < _swapChainFramebuffers.size(); index++)
				{
					Framebuffer::Destroy(_logicalDevice, _swapChainFramebuffers[index], nullptr);
				}

				CommandBuffer::Free(_logicalDevice, _commandPool, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());

				Pipeline::Destroy(_logicalDevice, _graphicsPipeline, nullptr);

				Pipeline::Layout::Destroy(_logicalDevice, _pipelineLayout, nullptr);

				RenderPass::Destroy(_logicalDevice, _renderPass, nullptr);

				for (DataSize index = 0; index < _swapChainImageViews.size(); index++)
				{
					ImageView::Destroy(_logicalDevice, _swapChainImageViews[index], nullptr);
				}

				SwapChain::Destroy(_logicalDevice, _swapChain, nullptr);
			}

			void CreateApplicationInstance
			(
				      RoCStr                          _appName                         , 
				      AppVersion&                     _version                         , 
				const ptr<ValidationLayerList>        _optionalValidationLayers        , 
				const ptr<DebugMessenger::CreateInfo> _optionalDebugMessengerCreateSpec,
				      AppInstance::Handle&            _applicationInstance
			)
			{
				using Meta::EEngineVersion;

				Stack
				(
					AppInstance::AppInfo    appSpec       {};
					AppInstance::CreateInfo appCreateSpec {};
				);

				if (Vulkan_EnableValidationLayers)
				{
					if (!CheckValidationLayerSupport(dref(_optionalValidationLayers)))
						throw std::runtime_error("Validation layers requested, but are not available!");
				}

				appSpec.SType         = AppInstance::AppInfo::STypeEnum                            ;
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

				appCreateSpec.SType   = AppInstance::CreateInfo::STypeEnum;
				appCreateSpec.AppInfo = getAddress(appSpec)               ;

				ExtensionIdentifierList extensions = GetRequiredExtensions();

				appCreateSpec.EnabledExtensionCount = SCast<uint32>(extensions.size());
				appCreateSpec.EnabledExtensionNames = extensions.data()               ;


				if (Vulkan_EnableValidationLayers)
				{
					appCreateSpec.EnabledLayerCount = SCast<uint32>(_optionalValidationLayers->size());

					appCreateSpec.EnabledLayerNames = _optionalValidationLayers->data();

					PopulateDebugMessengerCreateInfo(dref(_optionalDebugMessengerCreateSpec));

					appCreateSpec.Next = ptr<DebugMessenger::CreateInfo>(_optionalDebugMessengerCreateSpec);
				}
				else
				{
					appCreateSpec.EnabledLayerCount = 0;

					appCreateSpec.Next = nullptr;
				}

				EResult&& creationResult = AppInstance::Create(appCreateSpec, nullptr, _applicationInstance);

				if (creationResult != EResult::Success) 
					throw std::runtime_error("Triangle Test: Failed to create Vulkan app instance.");
			}

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

				CommandBuffer::AllocateInfo allocSpec {};

				allocSpec.SType       = CommandBuffer::AllocateInfo::STypeEnum  ;
				allocSpec.Pool        = _commandPool                            ;
				allocSpec.Level       = ECommandBufferLevel::Primary            ;
				allocSpec.BufferCount = uint32(_commandBufferContainer.size());

				if (CommandBuffer::Allocate(_logicalDevice, allocSpec, _commandBufferContainer.data()) != EResult::Success) 
				{
					throw std::runtime_error("failed to allocate command buffers!");
				}

				for (DataSize index = 0; index < _commandBufferContainer.size(); index++) 
				{
					CommandBuffer::BeginInfo beginInfo {};

					beginInfo.SType           = CommandBuffer::BeginInfo::STypeEnum;
					beginInfo.Flags           = 0                                  ;   // Optional
					beginInfo.InheritanceInfo = nullptr                            ;   // Optional

					if (CommandBuffer::BeginRecord(_commandBufferContainer[index], beginInfo) != EResult::Success) 
					{
						throw std::runtime_error("Failed to begin recording command buffer!");
					}

					RenderPass::BeginInfo renderPassInfo{};

					renderPassInfo.SType       = RenderPass::BeginInfo::STypeEnum;
					renderPassInfo.RenderPass  = _renderPass                     ;
					renderPassInfo.Framebuffer = _swapChain_FrameBuffers[index]  ;

					renderPassInfo.RenderArea.Offset.X = 0;
					renderPassInfo.RenderArea.Offset.Y = 0;

					renderPassInfo.RenderArea.Extent = _swapChain_Extent;

					ClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

					renderPassInfo.ClearValueCount = 1          ;
					renderPassInfo.ClearValues     = &clearColor;

					CommandBuffer::BeginRenderPass(_commandBufferContainer[index], renderPassInfo, ESubpassContents::Inline);

						CommandBuffer::BindPipeline(_commandBufferContainer[index], EPipelineBindPoint::Graphics, _graphicsPipeline);

						CommandBuffer::Draw
						(
							_commandBufferContainer[index], 
							0,
							3,
							0,
							1
						);

					CommandBuffer::EndRenderPass(_commandBufferContainer[index]);

					if (CommandBuffer::EndRecord(_commandBufferContainer[index]) != EResult::Success) 
					{
						throw std::runtime_error("Failed to record command buffer!");
					}	
				}
			}

			void CreateCommandPool
			(
				PhysicalDevice::Handle _physicalDevice, 
				LogicalDevice::Handle  _logicalDevice , 
				Surface::Handle        _surfaceHandle , 
				VkCommandPool&         _commandPool
			)
			{
				QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(_physicalDevice, _surfaceHandle);

				CommandPool::CreateInfo poolInfo {};

				poolInfo.SType            = CommandPool::CreateInfo::STypeEnum       ;
				poolInfo.QueueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();
				poolInfo.Flags            = CommandPool::CreateFlgas()               ;   // Optional
																					    
				if (CommandPool::Create(_logicalDevice, poolInfo, nullptr, _commandPool) != EResult::Success) 
				{
					throw std::runtime_error("failed to create command pool!");
				}
			}

			void CreateFrameBuffers
			(
				LogicalDevice::Handle _logicalDevice        ,
				VkRenderPass          _renderPass           ,
				Extent2D              _swapChainExtent      ,
				ImageViewList&        _swapChainImageViews  ,
				FrameBufferList&      _swapChainFrameBuffers
			)
			{
				_swapChainFrameBuffers.resize(_swapChainImageViews.size());

				for (DataSize index = 0; index < _swapChainImageViews.size(); index++) 
				{
					ImageView::Handle attachments[] = 
					{
						_swapChainImageViews[index]
					};

					Framebuffer::CreateInfo framebufferInfo {};

					framebufferInfo.SType           = Framebuffer::CreateInfo::STypeEnum;
					framebufferInfo.RenderPass      = _renderPass                       ;
					framebufferInfo.AttachmentCount = 1                                 ;
					framebufferInfo.Attachments     = attachments                       ;
					framebufferInfo.Width           = _swapChainExtent.Width            ;
					framebufferInfo.Height          = _swapChainExtent.Height           ;
					framebufferInfo.Layers          = 1                                 ;

					if (Framebuffer::Create(_logicalDevice, framebufferInfo, nullptr, _swapChainFrameBuffers[index]) != EResult::Success) 
					{
						throw std::runtime_error("Failed to create framebuffer!");
					}
				}
			}

			/*
			Creates a simple graphics pipeline.

			Note: This was enough for the Vulkan_TriangleTest, this will need modification most likely for higher level stuff.
			(At least shader wise)
			*/
			void CreateGraphicsPipeline
			(
				LogicalDevice::Handle     _logicalDevice   ,
				Extent2D                  _swapChainExtent ,
				VkRenderPass              _renderPass      ,
				Pipeline::Layout::Handle& _pipelineLayout  ,
				VkPipeline&               _graphicsPipeline   // Will be provided.
			)
			{
				using Bytecode_Buffer = DynamicArray<Bytecode>;

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

				triShaderStage_Vert_CreationSpec.SType = Pipeline::ShaderStage::CreateInfo::STypeEnum;
				triShaderStage_Vert_CreationSpec.Stage = EShaderStageFlag::Vertex                    ;

				triShaderStage_Vert_CreationSpec.Module = triShaderModule_Vert;
				triShaderStage_Vert_CreationSpec.Name   = "main"              ;

				triShaderStage_Frag_CreationSpec.SType = Pipeline::ShaderStage::CreateInfo::STypeEnum;
				triShaderStage_Frag_CreationSpec.Stage = EShaderStageFlag::Fragment                  ;

				triShaderStage_Frag_CreationSpec.Module = triShaderModule_Frag;
				triShaderStage_Frag_CreationSpec.Name   = "main"              ;

				Pipeline::ShaderStage::CreateInfo shaderStages[] = { triShaderStage_Vert_CreationSpec, triShaderStage_Frag_CreationSpec };

				// Fixed Function

				Pipeline::VertexInputState::CreateInfo vertexInputState_CreationSpec{};

				vertexInputState_CreationSpec.SType = Pipeline::VertexInputState::CreateInfo::STypeEnum;

				vertexInputState_CreationSpec.VertexBindingDescriptionCount = 0      ;
				vertexInputState_CreationSpec.BindingDescriptions           = nullptr;

				vertexInputState_CreationSpec.AttributeDescriptionCount = 0      ;
				vertexInputState_CreationSpec.AttributeDescription      = nullptr;

				Pipeline::InputAssemblyState::CreateInfo inputAssembly_CreationSpec{};

				inputAssembly_CreationSpec.SType = Pipeline::InputAssemblyState::CreateInfo::STypeEnum;

				inputAssembly_CreationSpec.Topology               = EPrimitiveTopology::TriangleList;
				inputAssembly_CreationSpec.PrimitiveRestartEnable = false                           ;

				Viewport viewport{};

				viewport.X        = 0.0f                          ;
				viewport.Y        = 0.0f                          ;
				viewport.Width    = float(_swapChainExtent.Width) ;
				viewport.Height   = float(_swapChainExtent.Height);
				viewport.MinDepth = 0.0f                          ;
				viewport.MaxDepth = 1.0f                          ;

				Rect2D scissor{};

				scissor.Offset.X = 0;
				scissor.Offset.Y = 0;

				scissor.Extent = _swapChainExtent;

				Pipeline::ViewportState::CreateInfo viewportState_CreationSpec{};

				viewportState_CreationSpec.SType         = Pipeline::ViewportState::CreateInfo::STypeEnum;
				viewportState_CreationSpec.ViewportCount = 1                                             ;
				viewportState_CreationSpec.Viewports     = &viewport                                     ;
				viewportState_CreationSpec.ScissorCount  = 1                                             ;
				viewportState_CreationSpec.Scissors      = &scissor                                      ;

				Pipeline::RasterizationState::CreateInfo rasterizer{};

				rasterizer.SType                   = Pipeline::RasterizationState::CreateInfo::STypeEnum;
				rasterizer.EnableDepthClamp        = EBool::False                                       ;
				rasterizer.EnableRasterizerDiscard = EBool::False                                       ;
				rasterizer.PolygonMode             = EPolygonMode::Fill                                 ;
				rasterizer.LineWidth               = 1.0f                                               ;

				rasterizer.CullMode.Set(ECullModeFlag::Back);

				rasterizer.FrontFace               = EFrontFace::Clockwise;
				rasterizer.EnableDepthBias         = EBool::False         ;
				rasterizer.DepthBiasConstantFactor = 0.0f                 ;
				rasterizer.DepthBiasClamp          = 0.0f                 ;
				rasterizer.DepthBiasSlopeFactor    = 0.0f                 ;

				Pipeline::MultisampleState::CreateInfo multisampling_CreationSpec{};

				multisampling_CreationSpec.SType                 = Pipeline::MultisampleState::CreateInfo::STypeEnum;
				multisampling_CreationSpec.EnableSampleShading   = EBool::False                                     ;
				multisampling_CreationSpec.RasterizationSamples  = ESampleCount::_1                                 ;
				multisampling_CreationSpec.MinSampleShading      = 1.0f                                             ;
				multisampling_CreationSpec.SampleMask            = nullptr                                          ;
				multisampling_CreationSpec.EnableAlphaToCoverage = EBool::False                                     ;
				multisampling_CreationSpec.EnableAlphaToOne      = EBool::False                                     ;

				Pipeline::ColorBlendState::AttachmentState colorBlend_Attachment{};

				colorBlend_Attachment.ColorWriteMask.Set
				(
					EColorComponentFlag::R, 
					EColorComponentFlag::G, 
					EColorComponentFlag::B, 
					EColorComponentFlag::A
				);

				colorBlend_Attachment.EnableBlend                  = EBool::False        ;
				colorBlend_Attachment.Source_ColorBlendFactor      = EBlendFactor::One   ;
				colorBlend_Attachment.Destination_ColorBlendFactor = EBlendFactor::Zero  ;
				colorBlend_Attachment.ColorOperation               = EBlendOperation::Add;
				colorBlend_Attachment.Source_AlphaBlendFactor      = EBlendFactor::One   ;
				colorBlend_Attachment.Destination_AlphaBlendFactor = EBlendFactor::Zero  ;
				colorBlend_Attachment.AlphaOperation               = EBlendOperation::Add;

				Pipeline::ColorBlendState::CreateInfo colorBlending_CreationSpec{};

				colorBlending_CreationSpec.SType                 = Pipeline::ColorBlendState::CreateInfo::STypeEnum;
				colorBlending_CreationSpec.EnableLogicOperations = EBool::False                                    ;
				colorBlending_CreationSpec.LogicOperation        = ELogicOperation::Copy                           ;
				colorBlending_CreationSpec.AttachmentCount       = 1                                               ;
				colorBlending_CreationSpec.Attachments           = &colorBlend_Attachment                          ;
				colorBlending_CreationSpec.BlendConstants[0]     = 0.0f                                            ;
				colorBlending_CreationSpec.BlendConstants[1]     = 0.0f                                            ;
				colorBlending_CreationSpec.BlendConstants[2]     = 0.0f                                            ;
				colorBlending_CreationSpec.BlendConstants[3]     = 0.0f                                            ;

				EDynamicState States[] =
				{
					EDynamicState::Viewport ,
					EDynamicState::LineWidth
				};

				Pipeline::DynamicState::CreateInfo dynamicState {};

				dynamicState.SType      = Pipeline::DynamicState::CreateInfo::STypeEnum;
				dynamicState.StateCount = 2                                            ;
				dynamicState.States     = States                                       ;

				Pipeline::Layout::CreateInfo pipelineLayout_CreationSpec {};

				pipelineLayout_CreationSpec.SType                  = Pipeline::Layout::CreateInfo::STypeEnum;
				pipelineLayout_CreationSpec.SetLayoutCount         = 0                                      ;
				pipelineLayout_CreationSpec.SetLayouts             = nullptr                                ;
				pipelineLayout_CreationSpec.PushConstantRangeCount = 0                                      ;
				pipelineLayout_CreationSpec.PushConstantRanges     = nullptr                                ;

				EResult&& piplineLayout_CreationResult = 
					Pipeline::Layout::Create(_logicalDevice, pipelineLayout_CreationSpec, nullptr, _pipelineLayout);

				if (piplineLayout_CreationResult != EResult::Success)
				{
					throw std::runtime_error("Failed to create pipeline layout!");
				}

				Pipeline::Graphics::CreateInfo pipelineInfo {};

				pipelineInfo.SType      = Pipeline::Graphics::CreateInfo::STypeEnum;
				pipelineInfo.StageCount = 2                                        ;
				pipelineInfo.Stages     = shaderStages                             ;

				pipelineInfo.VertexInputState   = &vertexInputState_CreationSpec;
				pipelineInfo.InputAssemblyState = &inputAssembly_CreationSpec   ;
				pipelineInfo.ViewportState      = &viewportState_CreationSpec   ;
				pipelineInfo.RasterizationState = &rasterizer                   ;
				pipelineInfo.MultisampleState   = &multisampling_CreationSpec   ;
				pipelineInfo.DepthStencilState  = nullptr                       ;   // Optional
				pipelineInfo.ColorBlendState    = &colorBlending_CreationSpec   ;
				pipelineInfo.DynamicState       = nullptr                       ;   // Optional

				pipelineInfo.Layout = _pipelineLayout;

				pipelineInfo.RenderPass = _renderPass;
				pipelineInfo.Subpass    = 0          ;

				pipelineInfo.BasePipelineHandle = VK_NULL_HANDLE;   // Optional
				pipelineInfo.BasePipelineIndex  = -1            ;   // Optional

				if (Pipeline::Graphics::Create(_logicalDevice, VK_NULL_HANDLE, 1, pipelineInfo, nullptr, _graphicsPipeline) != EResult::Success) 
				{
					throw std::runtime_error("Failed to create graphics pipeline!");
				}

				ShaderModule::Destory(_logicalDevice, triShaderModule_Vert, nullptr);
				ShaderModule::Destory(_logicalDevice, triShaderModule_Frag, nullptr);
			}	

			void CreateImageViews
			(
				LogicalDevice::Handle _logicalDevice       ,
				ImageList&            _swapChainImages     , 
				EFormat               _swapChainImageFormat,
				ImageViewList&        _imageViewContainer      // Will be populated.
			)
			{
				_imageViewContainer.resize(_swapChainImages.size());

				for (DataSize index = 0; index < _swapChainImages.size(); index++)
				{
					ImageView::CreateInfo creationSpec {};

					creationSpec.SType    = ImageView::CreateInfo::STypeEnum;
					creationSpec.Image    = _swapChainImages[index]         ;
					creationSpec.ViewType = EImageViewType::_2D             ;
					creationSpec.Format   = _swapChainImageFormat           ;

					creationSpec.Components.R = EComponentSwizzle::Identitity;
					creationSpec.Components.G = EComponentSwizzle::Identitity;
					creationSpec.Components.B = EComponentSwizzle::Identitity;
					creationSpec.Components.A = EComponentSwizzle::Identitity;

					creationSpec.SubresourceRange.AspectMask.Set(EImageAspect::Color);

					creationSpec.SubresourceRange.BaseMipLevel   = 0;
					creationSpec.SubresourceRange.LevelCount     = 1;
					creationSpec.SubresourceRange.BaseArrayLayer = 0;
					creationSpec.SubresourceRange.LayerCount     = 1;

					EResult&& creationResult = ImageView::Create(_logicalDevice, creationSpec, nullptr, &_imageViewContainer[index]);

					if (creationResult != EResult::Success)
					{
						throw std::runtime_error("Failed to create image views!");
					}
				}
			}

			void CreateLogicalDevice
			(
				PhysicalDevice::Handle        _physicalDevice          , 
				Surface::Handle               _surfaceHanle            ,
				ExtensionIdentifierList       _extensionsToSpecify     ,
				ptr<ValidationLayerList>      _optionalValidationLayers,
				LogicalDevice::Handle&        _logicalDevice           ,   // Will be provided.
				LogicalDevice::Queue::Handle& _graphicsQueue           ,   // Will be provided.
				LogicalDevice::Queue::Handle& _presentationQueue           // Will be provided.
			)	
			{
				QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice, _surfaceHanle);

				using LogicalDevice_QueueCreateInfoList = std::vector<LogicalDevice::Queue::CreateInfo>;

				LogicalDevice_QueueCreateInfoList queueCreateInfos;

				using IndiceSet = std::set<uint32>;

				IndiceSet queueFamiliesToCreate =
				{
					indices.GraphicsFamily    .value(),
					indices.PresentationFamily.value()
				};

				float32 queuePriority = 1.0f;

				for (auto queueFamily : queueFamiliesToCreate)
				{
					LogicalDevice::Queue::CreateInfo queueCreateInfo{};

					queueCreateInfo.SType            = LogicalDevice::Queue::CreateInfo::STypeEnum;
					queueCreateInfo.QueueFamilyIndex = queueFamily                                ;
					queueCreateInfo.QueueCount       = 1                                          ; 
					queueCreateInfo.QueuePriorities  = &queuePriority                             ;

					queueCreateInfos.push_back(queueCreateInfo);
				}

				//queueCreateInfo.QueuePriorities = &queuePriority;

				PhysicalDevice::Features physDeviceFeatures{};

				LogicalDevice::CreateInfo createInfo{};

				createInfo.SType                 = LogicalDevice::CreateInfo::STypeEnum;
				createInfo.QueueCreateInfoCount  = queueCreateInfos.size()             ;
				createInfo.QueueCreateInfos      = queueCreateInfos.data()             ;
				createInfo.EnabledFeatures       = &physDeviceFeatures                 ;
				createInfo.EnabledExtensionNames = _extensionsToSpecify.data()         ;
				createInfo.EnabledExtensionCount = _extensionsToSpecify.size()         ;

				if (Vulkan_EnableValidationLayers)
				{
					createInfo.EnabledLayerCount = SCast<uint32>(_optionalValidationLayers->size());
					createInfo.EnabledLayerNames = _optionalValidationLayers->data( )              ;
				}
				else
				{
					createInfo.EnabledLayerCount = 0;
				}

				EResult&& result = LogicalDevice::Create(_physicalDevice, createInfo, nullptr, _logicalDevice);

				if (result != EResult::Success)
				{
					throw std::runtime_error("Vulkan, TraingleTest: Failed to create logical device!");
				}

				LogicalDevice::Queue::Get(_logicalDevice, indices.GraphicsFamily    .value(), 0, _graphicsQueue    );
				LogicalDevice::Queue::Get(_logicalDevice, indices.PresentationFamily.value(), 0, _presentationQueue);
			}

			// TODO: Wrap.
			void CreateRenderPass
			(
				LogicalDevice::Handle _logicalDevice,
				EFormat               _imageFormat  ,
				VkRenderPass&         _renderPass       // Will be provided.
			)
			{
				RenderPass::AttachmentDescription colorAttachment {};

				colorAttachment.Format  = _imageFormat;
				colorAttachment.Samples = ESampleCount::_1;

				colorAttachment.LoadOp  = EAttachmentLoadOperation ::Clear;
				colorAttachment.StoreOp = EAttachmentStoreOperation::Store;

				colorAttachment.StencilLoadOp  = EAttachmentLoadOperation ::DontCare;
				colorAttachment.StencilStoreOp = EAttachmentStoreOperation::DontCare;

				colorAttachment.InitialLayout = EImageLayout::Undefined        ;
				colorAttachment.FinalLayout   = EImageLayout::PresentSource_KHR;

				RenderPass::AttachmentReference colorAttachmentRef{};

				colorAttachmentRef.Attachment = 0                                    ;
				colorAttachmentRef.Layout     = EImageLayout::Color_AttachmentOptimal;

				RenderPass::SubpassDescription subpass {};

				subpass.PipelineBindPoint    = EPipelineBindPoint::Graphics;
				subpass.ColorAttachmentCount = 1                           ;
				subpass.ColorAttachments     = &colorAttachmentRef         ;

				RenderPass::CreateInfo renderPassInfo{};

				renderPassInfo.SType           = RenderPass::CreateInfo::STypeEnum;
				renderPassInfo.AttachmentCount = 1                                ;
				renderPassInfo.Attachments     = &colorAttachment                 ;
				renderPassInfo.SubpassCount    = 1                                ;
				renderPassInfo.Subpasses       = &subpass                         ;


				RenderPass::SubpassDependency dependency{};

				dependency.SourceSubpass = VK_SUBPASS_EXTERNAL;
				dependency.DestinationSubpass = 0;

				dependency.SourceStageMask.Set(EPipelineStageFlag::ColorAttachmentOutput);

				dependency.SourceAccessMask = 0;

				dependency.DestinationStageMask .Set(EPipelineStageFlag::ColorAttachmentOutput);
				dependency.DestinationAccessMask.Set(EAccessFlag::ColorAttachmentWrite);

				renderPassInfo.DependencyCount = 1          ;
				renderPassInfo.Dependencies    = &dependency;


				if (RenderPass::Create(_logicalDevice, renderPassInfo, nullptr, _renderPass) != EResult::Success)
				{
					throw std::runtime_error("failed to create render pass!");
				}
			}

			void CreateSurface
			(
				AppInstance::Handle _applicationInstance,
				Window*             _window             , 
				Surface::Handle&    _surfaceHandle          // Will be provided.
			)
			{
				Surface::CreateInfo createInfo{};

				createInfo.SType       = Surface::CreateInfo::STypeEnum   ;
				createInfo.OSWinHandle = OSAL::GetOS_WindowHandle(_window);   // TODO: Use OSAL instead.
				createInfo.OSAppHandle = Surface::GetOS_AppHandle()       ;

				/*if (Vulkan::CreateSurface(_applicationInstance, createInfo, nullptr, _surfaceHandle) != EResult::Success) 
				{
					throw std::runtime_error("Vulkan, TriangleTest: Failed to create window surface!");
				}*/

				// TODO: Use OSAL if possible.
				if (EResult(SAL::GLFW::CreateWindowSurface(_applicationInstance, _window, nullptr, _surfaceHandle)) != EResult::Success)
				{
					throw std::runtime_error("Vulkan, TriangleTest: Failed to create window surface!");
				}
			}

			void CreateSwapChain
			(
				OSAL::Window*           _window             ,
				PhysicalDevice::Handle _physicalDevice      ,
				LogicalDevice::Handle  _logicalDevice       ,
				Surface::Handle        _surfaceHandle       ,
				SwapChain::Handle&     _swapChain           ,
				EFormat&               _swapChainImageFormat,
				Extent2D&              _swapChainExtent     ,
				ImageList&             _swapChain_Images
			)
			{
				SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_physicalDevice, _surfaceHandle);
				Surface::Format         surfaceFormat    = Surface_SwapChain_ChooseFormat(swapChainSupport.Formats);
				EPresentationMode       presentationMode = Surface_SwapChain_ChoosePresentationMode(swapChainSupport.PresentationModes);
				Extent2D                extent           = Surface_SwapChain_ChooseExtent(swapChainSupport.Capabilities,_window);
				uint32                  numImagesDesired = swapChainSupport.Capabilities.MinImageCount;

				numImagesDesired += 1;

				if (swapChainSupport.Capabilities.MaxImageCount > 0 && numImagesDesired > swapChainSupport.Capabilities.MaxImageCount)
				{
					numImagesDesired = swapChainSupport.Capabilities.MaxImageCount;
				}

				SwapChain::CreateInfo creationSpec {};

				creationSpec.SType            = SwapChain::CreateInfo::STypeEnum;
				creationSpec.Surface          = _surfaceHandle                  ;
				creationSpec.MinImageCount    = numImagesDesired                ;
				creationSpec.ImageFormat      = surfaceFormat.Format            ;
				creationSpec.ImageColorSpace  = surfaceFormat.ColorSpace        ;
				creationSpec.ImageExtent      = extent                          ;
				creationSpec.ImageArrayLayers = 1                               ;

				creationSpec.ImageUsage.Set(EImageUsage::Color_Attachment);

				QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice, _surfaceHandle);

				uint32 queueFamilyIndices[] = 
				{
					indices.GraphicsFamily    .value(), 
					indices.PresentationFamily.value() 
				};

				if (indices.GraphicsFamily != indices.PresentationFamily) 
				{
					creationSpec.ImageSharingMode      = ESharingMode::Concurrent;
					creationSpec.QueueFamilyIndexCount = 2                       ;
					creationSpec.QueueFamilyIndices    = queueFamilyIndices      ;
				}
				else 
				{
					creationSpec.ImageSharingMode      = ESharingMode::Excusive;
					creationSpec.QueueFamilyIndexCount = 0                     ; // Optional
					creationSpec.QueueFamilyIndices    = nullptr               ; // Optional
				}

				creationSpec.PreTransform     = swapChainSupport.Capabilities.CurrentTransform;
				creationSpec.CompositeAlpha   = ECompositeAlpha::Opaque                       ;
				creationSpec.PresentationMode = presentationMode                              ;
				creationSpec.Clipped          = true                                          ;
				creationSpec.OldSwapchain     = SwapChain::NullHandle                         ;

				EResult&& creationResult = SwapChain::Create(_logicalDevice, creationSpec, nullptr, _swapChain);

				if (creationResult != EResult::Success)
				{
					throw std::runtime_error("Failed to create the swap chain!");
				}

				_swapChain_Images.resize(Vault_02::SwapChain::GetImageCount(_logicalDevice, _swapChain));

				Vault_02::SwapChain::GetImages(_logicalDevice, _swapChain, _swapChain_Images.data());

				_swapChainImageFormat = surfaceFormat.Format;
				_swapChainExtent      = extent              ;
			}

			void CreateSyncObjects
			(
				LogicalDevice::Handle _logicalDevicce          ,
				sint32                _maxFramesInFlight       ,
				ImageList             _swapChainImages         ,
				SemaphoreList&        _imageAvailableSemaphores,
				SemaphoreList&        _renderFinishedSemaphores,
				FenceList&            _inFlightFences          ,
				FenceList&            _imagesInFlight
			)
			{
				_imageAvailableSemaphores.resize(_maxFramesInFlight);
				_renderFinishedSemaphores.resize(_maxFramesInFlight);

				_inFlightFences.resize(_maxFramesInFlight                      );
				_imagesInFlight.resize(_swapChainImages.size(), VK_NULL_HANDLE);

				Semaphore::CreateInfo semaphore_CreationSpec {};

				semaphore_CreationSpec.SType = Semaphore::CreateInfo::STypeEnum;

				Fence::CreateInfo fence_CreationSpec {};

				fence_CreationSpec.SType = Fence::CreateInfo::STypeEnum;

				fence_CreationSpec.Flags.Set(EFenceCreateFlag::Signaled);

				for (DataSize index = 0; index < _maxFramesInFlight; index++)
				{
					if
						(
							Semaphore::Create(_logicalDevicce, semaphore_CreationSpec, nullptr, _imageAvailableSemaphores[index]) != EResult::Success ||
							Semaphore::Create(_logicalDevicce, semaphore_CreationSpec, nullptr, _renderFinishedSemaphores[index]) != EResult::Success ||
							Fence    ::Create(_logicalDevicce, fence_CreationSpec    , nullptr, _inFlightFences          [index]) != EResult::Success
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

				creationSpec.SType     = ShaderModule::CreateInfo::STypeEnum;
				creationSpec.Extension = NULL                               ;
				creationSpec.CodeSize  = code.size()                        ;
				creationSpec.Code      = RCast<const ByteCode>(code.data()) ;

				ShaderModule::Handle createdModule;

				EResult&& creationResult = ShaderModule::Create(_logicalDevice, creationSpec, nullptr, createdModule);

				if (creationResult != EResult::Success)
				{
					throw RuntimeError("Failed to create TriShader module!");
				}

				return createdModule;
			}

			Bool DebugCallback
			(
					  MessageServerityFlags        _messageServerity, 
					  MessageTypeFlags             _messageType     ,
				const DebugMessenger::CallbackData _callbackData    , 
					  ptr<void>                    _userData
			)
			{
				return DebugCallback_Internal(_messageServerity, _messageType, _callbackData, _userData);
			}

			/*
			I had to make this to resolve an issue with a decltype function resolve error on EnforceConvention.
			*/
			Bool DebugCallback_Internal
			(
					  MessageServerityFlags        _messageServerity, 
					  MessageTypeFlags             _messageType     ,
				const DebugMessenger::CallbackData _callbackData    , 
					  ptr<void>                    _userData
			)
			{
				std::cerr << "Vulkan: Validation Layer: " << _callbackData.Message << std::endl;

				return EBool::True;
			}

			QueueFamilyIndices FindQueueFamilies
			(
				PhysicalDevice::Handle _deviceHandle ,
				Surface       ::Handle _surfaceHandle
			)
			{
				QueueFamilyIndices indices{};

				using QueueFamilyPropertiesListing = std::vector<Vault_02::PhysicalDevice::Properties::QueueFamily>;

				uint32 queueFamilyCount = Vault_02::PhysicalDevice::Properties::QueueFamily::GetCount(_deviceHandle);

				QueueFamilyPropertiesListing queueFamilies(queueFamilyCount);

				Vault_02::PhysicalDevice::Properties::QueueFamily::Get(_deviceHandle, queueFamilies.data());

				int index = 0;

				for (const auto& queueFamily : queueFamilies)
				{
					if (queueFamily.QueueFlags.Has(EQueueFlag::Graphics))
					{
						indices.GraphicsFamily = index;
					}

					Bool presentationSupport = EBool::False;

					Surface::CheckPhysicalDeviceSupport(_deviceHandle, index, _surfaceHandle, presentationSupport);

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
				Stack
				(
					uint32    numExtensions      = 0; 
					CStrArray extensionsRequired    ;
				);

				extensionsRequired = GetRequiredVulkanAppExtensions(numExtensions);

				stack<ExtensionIdentifierList> extensions(extensionsRequired, extensionsRequired + numExtensions);

				if (Vulkan_EnableValidationLayers)
				{
					extensions.push_back(Extension_DebugUtility);
				}

				return extensions;
			}

			Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
			CStrArray> GetRequiredVulkanAppExtensions(uint32& _numExtensions)
			{
				return CStrArray(SAL::GLFW::GetRequiredVulkanAppExtensions(_numExtensions));
			}

			bool IsDeviceSuitable(PhysicalDevice::Handle _physicalDevice, Surface::Handle _surface, ExtensionIdentifierList _extensionsSpecified)
			{
				stack<PhysicalDevice::Properties> deviceProperties;
				stack<PhysicalDevice::Features  > deviceFeatures  ;

				PhysicalDevice::GetProperties(_physicalDevice, deviceProperties);
				PhysicalDevice::GetFeatures  (_physicalDevice, deviceFeatures  );

				QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice, _surface);

				bool extensionsSupported = PhysicalDevice_CheckExtensionSupport(_physicalDevice, _extensionsSpecified);

				bool swapChainAdequate = false;

				if (extensionsSupported)
				{
					SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_physicalDevice, _surface);

					swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentationModes.empty();
				}

				bool&& result = 
					bool(deviceFeatures.GeometryShader) &&
					indices.IsComplete()                &&
					extensionsSupported                 &&
					swapChainAdequate                     ;

				return result;
			}

			bool PhysicalDevice_CheckExtensionSupport(PhysicalDevice::Handle _handle, ExtensionIdentifierList _extensionsSpecified)
			{
				using ExtensionPropertiesList = std::vector<ExtensionProperties>;

				ExtensionPropertiesList availableExtensions(Vault_02::PhysicalDevice::GetNumOfAvailableExtensions(_handle));

				Vault_02::PhysicalDevice::GetAvailableExtensions(_handle, availableExtensions.data());

				using ExtensionNameSet = std::set<std::string>;

				ExtensionNameSet requiredExtensions(_extensionsSpecified.begin(), _extensionsSpecified.end());

				for (const auto& extension : availableExtensions)
				{
					requiredExtensions.erase(extension.ExtensionName);
				}

				bool&& isSupported = requiredExtensions.empty();

				return isSupported;
			}

			void PickPhysicalDevice
			(
				AppInstance::Handle     _applicationInstance, 
				PhysicalDevice::Handle& _physicalDevice     , 
				Surface::Handle         _surface            , 
				ExtensionIdentifierList _extensionsSpecified
			)
			{
				stack<uint32> numDevices = Vault_02::PhysicalDevice::GetNumOfDevices(_applicationInstance);

				if (numDevices == 0)
					throw std::runtime_error("Physical device count 0. No GPUs found with Vulkan support.");

				using PhysicalDeviceList = std::vector<PhysicalDevice::Handle>;

				PhysicalDeviceList physicalDevices(numDevices);

				Vault_02::PhysicalDevice::GetAvailableDevices(_applicationInstance, physicalDevices.data());

				for (const auto& device : physicalDevices)
				{
					if (IsDeviceSuitable(device, _surface, _extensionsSpecified))
					{
						_physicalDevice = device;

						break;
					}
				}

				if (_physicalDevice == PhysicalDevice::NullHandle)
				{
					throw std::runtime_error("Not able to find suitable Vulkan supported GPU.");
				}
			}
			
			void PopulateDebugMessengerCreateInfo(DebugMessenger::CreateInfo& _msngrCreateInfo)
			{
				_msngrCreateInfo.SType = DebugMessenger::CreateInfo::STypeEnum;

				using EMaskS = decltype(_msngrCreateInfo.Serverity)::Enum;

				_msngrCreateInfo.Serverity.Set(EMaskS::Verbose, EMaskS::Warning, EMaskS::Error);

				using EMaskT = decltype(_msngrCreateInfo.Type)::Enum;

				_msngrCreateInfo.Type.Set(EMaskT::General, EMaskT::Validation, EMaskT::Performance);

				_msngrCreateInfo.UserCallback = EnforceConvention(EnforcerID_Vulkan, DebugCallback_Internal);

				_msngrCreateInfo.UserData = nullptr;
			}

			SwapChainSupportDetails QuerySwapChainSupport(PhysicalDevice::Handle _deviceHandle, Surface::Handle _surfaceHandle)
			{
				SwapChainSupportDetails details;

				Surface::GetPhysicalDeviceCapabilities(_deviceHandle, _surfaceHandle, details.Capabilities);

				uint32 formatCount;

				formatCount = Vault_02::Surface::GetNumOf_AvailableFormats(_deviceHandle, _surfaceHandle);

				if (formatCount > 0)
				{
					details.Formats.resize(formatCount);

					Vault_02::Surface::GetAvailableFormats(_deviceHandle, _surfaceHandle, details.Formats.data());
				}

				uint32 presentationModeCount;

				presentationModeCount = Vault_02::Surface::GetNumOf_SupportedPresentationModes(_deviceHandle, _surfaceHandle);

				if (presentationModeCount > 0)
				{
					details.PresentationModes.resize(presentationModeCount);

					Vault_02::Surface::GetSupportedPresentationModes(_deviceHandle, _surfaceHandle, details.PresentationModes.data());
				}

				return details;
			}

			int RateDeviceSuitability(PhysicalDevice::Handle _deviceHandle)
			{
				stack<PhysicalDevice::Properties> deviceProperties;

				PhysicalDevice::GetProperties(_deviceHandle, deviceProperties);

				stack<PhysicalDevice::Features> deviceFeatures;

				PhysicalDevice::GetFeatures(_deviceHandle, deviceFeatures);

				int score = 0;

				deviceProperties.Type == PhysicalDevice::EDeviceType::DiscreteGPU ? score += 1000 : NULL;

				score += deviceProperties.LimitsSpec.MaxImageDimension2D;

				if (!deviceFeatures.GeometryShader)
				{
					return 0;
				}

				return score;
			}

			void SetupDebugMessenger
			(
				AppInstance::Handle     _applicationInstance,
				DebugMessenger::Handle& _debugMessenger
			)
			{
				if (!Vulkan_EnableValidationLayers) return;

				stack<DebugMessenger::CreateInfo> msngrCreateSpec{};

				PopulateDebugMessengerCreateInfo(msngrCreateSpec);

				EResult&& creationResult = DebugMessenger::Create(_applicationInstance, msngrCreateSpec, nullptr, _debugMessenger);

				if (creationResult != EResult::Success) throw std::runtime_error("Failed to setup debug messenger!");
			}

			Extent2D Surface_SwapChain_ChooseExtent(const Surface::Capabilities& _capabilities, const ptr<Window> _window)
			{
				if (_capabilities.CurrentExtent.Width != UInt32Max)
				{
					return _capabilities.CurrentExtent;
				}
				else
				{
					stack<int> width, height;

					OSAL::FrameBufferDimensions frameBufferSize = OSAL::GetFramebufferDimensions(_window);

					Extent2D actualExtent;

					actualExtent.Width  = SCast<uint32>(frameBufferSize.Width );
					actualExtent.Height = SCast<uint32>(frameBufferSize.Height);

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
							availableFormat.Format     == EFormat    ::B8_G8_R8_A8_SRGB   &&
							availableFormat.ColorSpace == EColorSpace::KHR_SRGB_NonLinear         
						)
					{
						return availableFormat;
					}
				}

				// Just pick the first format...
				return _availableFormats[0];
			}

			EPresentationMode Surface_SwapChain_ChoosePresentationMode(const SurfacePresentationModeList& _surfacePresentationModes)
			{
				for (const auto& availablePresentationMode : _surfacePresentationModes)
				{
					if (availablePresentationMode == EPresentationMode::Mailbox)
					{
						return availablePresentationMode;
					}
				}

				return EPresentationMode::FIFO;
			}


			// GPU_HAL

			void Initialize_GPUComms(RoCStr _applicationName, AppVersion _applicationVersion)
			{
				CreateApplicationInstance
				(
					_applicationName, 
					_applicationVersion, 
					&ValidationLayerIdentifiers, 
					&DebugMessenger_CreationSpec,
					AppGPU
				);

				SetupDebugMessenger(AppGPU, DebugMessenger_Handle);
			}

			void Cease_GPUComms()
			{
				AppInstance::Destroy(AppGPU, nullptr);
			}

			void WaitFor_GPUIdle()
			{
				LogicalDevice::WaitUntilIdle(LogicalDevice);
			}
			
			ptr<ARenderContext> GetRenderContext(ptr<OSAL::Window> _window)
			{
				RenderContext renderContext {};

				// Fill this shit

				RenderContextPool.push_back(renderContext);

				return &RenderContextPool.back();
			}

			namespace Dirty
			{
				void GetRenderReady(ptr<Window> _window)
				{
					CreateSurface(AppGPU, _window, SurfaceHandle);

					PickPhysicalDevice(AppGPU, PhysicalDevice, SurfaceHandle, DeviceExtensions);

					CreateLogicalDevice(PhysicalDevice, SurfaceHandle, DeviceExtensions, &ValidationLayerIdentifiers, LogicalDevice, GraphicsQueue, PresentationQueue);

					CreateSwapChain(_window, PhysicalDevice, LogicalDevice, SurfaceHandle, SwapChain, SwapChain_ImageFormat, SwapChain_Extent, SwapChain_Images);

					CreateImageViews(LogicalDevice, SwapChain_Images, SwapChain_ImageFormat, SwapChain_ImageViews);

					CreateRenderPass(LogicalDevice, SwapChain_ImageFormat, RenderPass);

					CreateGraphicsPipeline(LogicalDevice, SwapChain_Extent, RenderPass, PipelineLayout, GraphicsPipeline);

					CreateFrameBuffers(LogicalDevice, RenderPass, SwapChain_Extent, SwapChain_ImageViews, SwapChain_Framebuffers);

					CreateCommandPool(PhysicalDevice, LogicalDevice, SurfaceHandle, CommandPool);

					CreateCommandBuffers(LogicalDevice, GraphicsPipeline, SwapChain_Framebuffers, SwapChain_Extent, RenderPass, CommandPool, CommandBuffers);;

					CreateSyncObjects(LogicalDevice, MaxFramesInFlight, SwapChain_Images, ImageAvailable_Semaphores, RenderFinished_Semaphores, InFlightFences, ImagesInFlight);
				}

				void RecreateSwapChain(ptr<Window> _window)
				{
					OSAL::FrameBufferDimensions dimensions;

					dimensions = OSAL::GetFramebufferDimensions(_window);

					while (dimensions.Width == 0 || dimensions.Height == 0)
					{
						OSAL::GetFramebufferDimensions(_window);

						SAL::GLFW::WaitForEvents();
					}

					LogicalDevice::WaitUntilIdle(LogicalDevice);

					CleanupSwapChain(LogicalDevice, PipelineLayout, GraphicsPipeline, SwapChain, SwapChain_ImageViews, SwapChain_Framebuffers, RenderPass, CommandPool, CommandBuffers);

					CreateSwapChain(_window, PhysicalDevice, LogicalDevice, SurfaceHandle, SwapChain, SwapChain_ImageFormat, SwapChain_Extent, SwapChain_Images);

					CreateImageViews(LogicalDevice, SwapChain_Images, SwapChain_ImageFormat, SwapChain_ImageViews);

					CreateRenderPass(LogicalDevice, SwapChain_ImageFormat, RenderPass);

					CreateGraphicsPipeline(LogicalDevice, SwapChain_Extent, RenderPass, PipelineLayout, GraphicsPipeline);

					CreateFrameBuffers(LogicalDevice, RenderPass, SwapChain_Extent, SwapChain_ImageViews, SwapChain_Framebuffers);

					CreateCommandBuffers(LogicalDevice, GraphicsPipeline, SwapChain_Framebuffers, SwapChain_Extent, RenderPass, CommandPool, CommandBuffers);
				}

				// TODO: Wrap
				void DrawFrame(ptr<Window> _window)
				{
					Fence::WaitForFences(LogicalDevice, 1, &InFlightFences[CurrentFrame], EBool::True, UInt64Max);

					uint32 imageIndex;

					EResult&& result = 
						SwapChain::AcquireNextImage
						(
							LogicalDevice                          , 
							SwapChain                              , 
							UInt64Max                              , 
							ImageAvailable_Semaphores[CurrentFrame], 
							Fence::NullHandle                      ,
							imageIndex
						);
						
					if (result == EResult::Error_OutOfDate_KHR) 
					{
						RecreateSwapChain(_window);

						return;
					}
					else if (result != EResult::Success && result != EResult::Suboptimal_KHR) 
					{
						throw std::runtime_error("Failed to acquire swap chain image!");
					}

					if (ImagesInFlight[imageIndex] != NullHandle) 
						Fence::WaitForFences(LogicalDevice, 1, &ImagesInFlight[imageIndex], EBool::True, UInt64Max);

					ImagesInFlight[imageIndex] = InFlightFences[CurrentFrame];


					CommandBuffer::SubmitInfo submitInfo {};

					submitInfo.SType = CommandBuffer::SubmitInfo::STypeEnum;


					Semaphore::Handle waitSemaphores[] = { ImageAvailable_Semaphores[CurrentFrame] };

					Pipeline::StageFlags waitStages[1] {};

					waitStages[0].Set(EPipelineStageFlag::ColorAttachmentOutput);

					submitInfo.WaitSemaphoreCount = 1             ;
					submitInfo.WaitSemaphores     = waitSemaphores;
					submitInfo.WaitDstStageMask   = waitStages    ;

					submitInfo.CommandBufferCount = 1;
					submitInfo.CommandBuffers     = &CommandBuffers[imageIndex];


					Semaphore::Handle signalSemaphores[] = { RenderFinished_Semaphores[CurrentFrame] };

					submitInfo.SignalSemaphoreCount = 1               ;
					submitInfo.SignalSemaphores     = signalSemaphores;


					Fence::Reset(LogicalDevice, InFlightFences[CurrentFrame], 1);

					if (CommandBuffer::SubmitToQueue(GraphicsQueue, 1, &submitInfo, InFlightFences[CurrentFrame]) != EResult::Success) 
						throw std::runtime_error("Failed to submit draw command buffer!");


					SwapChain::PresentationInfo presentInfo{};

					presentInfo.SType              = SwapChain::PresentationInfo::STypeEnum;
					presentInfo.WaitSemaphoreCount = 1                                     ;
					presentInfo.WaitSemaphores     = signalSemaphores                      ;


					SwapChain::Handle swapChains[] = { SwapChain };

					presentInfo.SwapchainCount = 1          ;
					presentInfo.Swapchains     = swapChains ;
					presentInfo.ImageIndices   = &imageIndex;

					presentInfo.Results = nullptr; // Optional


					result = SwapChain::QueuePresentation(PresentationQueue, presentInfo);

					if (result == EResult::Error_OutOfDate_KHR || result == EResult::Suboptimal_KHR || FramebufferResized) 
					{
						FramebufferResized = false;

						RecreateSwapChain(_window);
					}
					else if (result != EResult::Success) 
					{
						throw std::runtime_error("failed to present swap chain image!");
					}

					CurrentFrame = (CurrentFrame + 1) % MaxFramesInFlight;
				}

				void DeInitializeRenderReady(ptr<OSAL::Window> _window)
				{
					CleanupSwapChain
					(
						LogicalDevice         , 
						PipelineLayout        , 
						GraphicsPipeline      , 
						SwapChain             , 
						SwapChain_ImageViews  , 
						SwapChain_Framebuffers, 
						RenderPass            , 
						CommandPool           , 
						CommandBuffers
					);

					for (DataSize index = 0; index < MaxFramesInFlight; index++) 
					{
						Semaphore::Destroy(LogicalDevice, RenderFinished_Semaphores[index], nullptr);   // TODO: Wrap
						Semaphore::Destroy(LogicalDevice, ImageAvailable_Semaphores[index], nullptr);   // TODO: Wrap
						Fence::Destroy    (LogicalDevice, InFlightFences           [index], nullptr);   // TODO: Wrap
					}

					CommandPool::Destroy(LogicalDevice, CommandPool, nullptr);   // TODO: Wrap

					LogicalDevice::Destroy(LogicalDevice, nullptr);

					if (Vulkan_EnableValidationLayers) DebugMessenger::Destroy(AppGPU, DebugMessenger_Handle, nullptr);

					Surface::Destroy(AppGPU, SurfaceHandle, nullptr);
				}

				void ReinitializeRenderer(ptr<OSAL::Window> _window)
				{
					RecreateSwapChain(_window);
				}
			}
		}
	}

#endif
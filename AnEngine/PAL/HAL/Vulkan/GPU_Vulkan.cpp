// Parent Header
#include "GPU_Vulkan.hpp"



// Includes


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Renderer/Shader/TriangleShader/TriangleShader.hpp"
#include "Renderer/Shader/VKTut/VKTut.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <chrono>
#include <unordered_map>


#if VULCAN_INTERFACE == VAULTED_THERMALS_INTERFACE

	namespace HAL::GPU
	{
		namespace Platform_Vulkan
		{
			struct QueueFamilyIndices
			{
				std::optional<uint32> Graphics;
				std::optional<uint32> Compute;
				std::optional<uint32> Transfer;
				std::optional<uint32> SparseBinding;

				std::optional<uint32> PresentationSupported;

				std::set<uint32> GetSet()
				{
					std::set<uint32> queueIndices =
					{
						Graphics.value(),
						//Compute.value(),
						//Transfer.value(),
						//SparseBinding.value()
						PresentationSupported.value()
					};

					return queueIndices;
				}
			};


			// Static Data

			Vault_4::AppInstance AppGPU;

			Vault_4::PhysicalDevice PhysicalDevice;

			QueueFamilyIndices QueueIndices;

			Vault_4::LogicalDevice LogicalDevice;

			std::vector<Vault_4::LogicalDevice::Queue> DeviceQueues;

			Vault_4::LogicalDevice::Queue* GraphicsQueue    ;
			Vault_4::LogicalDevice::Queue* PresentationQueue;

			
				CommandBufferList   CommandBuffers;
				CommandPool::Handle CommandPool   ;   // TODO: Wrap

				//PhysicalDeviceList  PhysicalDevices      ;
				DebugMessenger::Handle     DebugMessenger_Handle;
				DebugMessenger::CreateInfo DebugMessenger_CreationSpec;

				Pipeline::Layout::DescriptorSet::Handle DescriptorSetLayout;

				Pipeline::Layout::Handle PipelineLayout;

				Pipeline::Handle GraphicsPipeline;

				FenceList InFlightFences;
				FenceList ImagesInFlight;

				SemaphoreList ImageAvailable_Semaphores;
				SemaphoreList RenderFinished_Semaphores;

				RenderContextList RenderContextPool; 

				Surface::Handle SurfaceHandle;

				SwapChain::Handle SwapChain;

				Extent2D      SwapChain_Extent     ;
				ImageList     SwapChain_Images     ;
				EFormat       SwapChain_ImageFormat;
				ImageViewList SwapChain_ImageViews ;

				FrameBufferList SwapChain_Framebuffers;

				RenderPass::Handle RenderPass;   

				Vault_4::Buffer VertexBuffer;
				Vault_4::Memory VertexBufferMemory;

				Vault_4::Buffer IndexBuffer;
				Vault_4::Memory IndexBufferMemory;

				DynamicArray<Vault_4::Buffer> UniformBuffers;
				DynamicArray<Vault_4::Memory> UniformBuffersMemory;

				Vault_4::DescriptorPool DescriptorPool;	

				DynamicArray<DescriptorSet::Handle> DescriptorSets;

				Vault_4::Image TextureImage;

				Vault_4::Memory TextureImageMemory;

				Vault_4::ImageView TextureImageView;

				Vault_4::Sampler TextureSampler;

				Vault_4::Image DepthImage;

				Vault_4::Memory DepthImageMemory;

				Vault_4::ImageView DepthImageView;


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


			ValidationLayerList ValidationLayerIdentifiers =
			{
				ValidationLayer_Khronos
			};


			/**
			* @brief Note: Need to define size.
			*/
			const Vault_2::Buffer::CreateInfo StagingBufferInfo = Vault_2::Buffer::CreateInfo(EBufferUsage::TransferSource, ESharingMode::Exclusive);


			

			#pragma region VKTut_V1

			struct UniformBufferObject
			{
				alignas(16) glm::mat4 ModelSpace;
				alignas(16) glm::mat4 Viewport  ;
				alignas(16) glm::mat4 Projection;
			};

				using AttributeDescription = Pipeline::VertexInputState::AttributeDescription;
				using BindingDescription   = Pipeline::VertexInputState::BindingDescription  ;

				using VertexAttributes = StaticArray<AttributeDescription, 3>;

				struct Vertex
				{
					struct
					{
						float32 X, Y, Z;
					} 
					
					Position;

					struct
					{
						float32 R,G,B;
					}
					
					Color;

					struct 
					{
						float32 X, Y;
					}
					
					TextureCoordinates;


					static constexpr VertexAttributes GetAttributeDescriptions()
					{
						VertexAttributes result{};

						// Position Attributes

						AttributeDescription& posAttrib = result.at(0);

						posAttrib.Binding = 0;
						posAttrib.Location = 0;
						posAttrib.Format = EFormat::R32_G32_B32_SFloat;
						posAttrib.Offset = offsetof(Vertex, Vertex::Position);

						// Color Attributes

						AttributeDescription& colorAttrib = result.at(1);

						colorAttrib.Binding = 0;
						colorAttrib.Location = 1;
						colorAttrib.Format = EFormat::R32_G32_B32_SFloat;
						colorAttrib.Offset = offsetof(Vertex, Vertex::Color);

						// Texture Coordinate Attributes

						AttributeDescription& texCoordAttrib = result.at(2);

						texCoordAttrib.Binding = 0;
						texCoordAttrib.Location = 2;
						texCoordAttrib.Format = EFormat::R32_G32_SFloat;
						texCoordAttrib.Offset = offsetof(Vertex, Vertex::TextureCoordinates);

						return result;
					}

					static constexpr BindingDescription GetBindingDescription()
					{
						BindingDescription result{};

						result.Binding = 0;
						result.Stride = sizeof(Vertex);
						result.InputRate = EVertexInputRate::Vertex;

						return result;
					}
				};

				const DynamicArray<Vertex> TriangleVerticies = 
				{
					{
						{ 0.0f, -0.5f      }, 
						{ 1.0f,  0.0f, 0.0f}
					},
					{
						{ 0.5f, 0.5f      }, 
						{ 0.0f, 1.0f, 0.0f}
					},
					{
						{-0.5f, 0.5f      }, 
						{ 0.0f, 0.0f, 1.0f}
					}
				};

				const DynamicArray<Vertex> SquareVerticies =
				{
					{
						{ -0.5f, -0.5f, 0.0f }, 
						{  1.0f,  0.0f, 0.0f }, 
						{  0.0f,  0.0f       }
					},
					{
						{ 0.5f, -0.5f, 0.0f }, 
						{ 0.0f,  1.0f, 0.0f }, 
						{ 1.0f,  0.0f       }
					},
					{
						{ 0.5f, 0.5f, 0.0f }, 
						{ 0.0f, 0.0f, 1.0f }, 
						{ 1.0f, 1.0f       }
					},
					{
						{ -0.5f, 0.5f, 0.0f }, 
						{  1.0f, 1.0f, 1.0f }, 
						{  0.0f, 1.0f       }
					},

					{ 
						{ -0.5f, -0.5f, -0.5f }, 
						{  1.0f,  0.0f,  0.0f }, 
						{  0.0f,  0.0f        }
					},
					{
						{ 0.5f, -0.5f, -0.5f }, 
						{ 0.0f,  1.0f,  0.0f }, 
						{ 1.0f,  0.0f        }
					},
					{
						{ 0.5f, 0.5f, -0.5f }, 
						{ 0.0f, 0.0f,  1.0f }, 
						{ 1.0f, 1.0f        }
					},
					{
						{ -0.5f, 0.5f, -0.5f }, 
						{  1.0f, 1.0f,  1.0f }, 
						{  0.0f, 1.0f        }
					}
				};

				const DynamicArray<uInt16> SquareIndices =
				{
					0, 1, 2, 2, 3, 0,
					4, 5, 6, 6, 7, 4
				};
				

				DynamicArray<Vertex> ModelVerticies;
				DynamicArray<uint32> ModelIndicies;

				const String VikingRoom_ModelPath   = "Engine/Data/Models/VikingRoom/viking_room.obj";
				const String VikingRoom_TexturePath = "Engine/Data/Models/VikingRoom/viking_room.png";

				const String EdsCryingCat_ModelPath = "Engine/Data/Models/EdsCryingCat/EdsCryingCat.obj";
				const String EdsCryingCat_TexturePath = "Engine/Data/Models/EdsCryingCat/EdsCryingCat.jpg";

				uint32 MipMapLevels;

				ESampleCount MSAA_Samples = ESampleCount::_1;

				Vault_4::Image ColorImage;
				Vault_4::Memory ColorImageMemory;
				Vault_4::ImageView ColorImageView;
				
				//Image::Handle TextureImage;


			#pragma endregion VKTut_V1


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


				uInt32&& layerCount = Vault_2::ValidationLayers::GetNumberOfLayers();

				stack<LayerPropertyList> availableLayers(layerCount);

				Vault_2::ValidationLayers::GetAvailableLayers(availableLayers.data());


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
				//ImageView::Destroy(LogicalDevice.GetHandle(), ColorImageView, nullptr);

				ColorImageView.Destroy();

				//Image::Destroy(LogicalDevice.GetHandle(), ColorImage, nullptr);

				ColorImage.Destroy();

				//Memory::Free(LogicalDevice.GetHandle(), ColorImageMemory, nullptr);

				ColorImageMemory.Free();

				//ImageView::Destroy(LogicalDevice.GetHandle(), DepthImageView, nullptr);

				DepthImageView.Destroy();

				//Image::Destroy(LogicalDevice.GetHandle(), DepthImage, nullptr);

				DepthImage.Destroy();

				//Memory::Free(LogicalDevice.GetHandle(), DepthImageMemory, nullptr);

				DepthImageMemory.Free();

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

					//_swapChainImageViews[index].Destroy();
				}

				SwapChain::Destroy(_logicalDevice, _swapChain, nullptr);

				for (DataSize index = 0; index < _swapChainImageViews.size(); index++)
				{
					//Buffer::Destroy(_logicalDevice, UniformBuffers[index], nullptr);
					//Memory::Free(_logicalDevice, UniformBuffersMemory[index], nullptr);

					UniformBuffers[index].Destroy();
					UniformBuffersMemory[index].Free();
				}
				
				//DescriptorPool::Destroy(_logicalDevice, DescriptorPool, nullptr);

				DescriptorPool.Destroy();
			}

			void CopyBufferToImage(Buffer::Handle _buffer, Image::Handle _image, uint32 _width, uint32 _height)
			{
				CommandBuffer::Handle commandBuffer = Vault_2::CommandBuffer::BeginSingleTimeCommands(LogicalDevice.GetHandle(), CommandPool);

				CommandBuffer::BufferImageRegion region {};

				region.BufferOffset = 0;
				region.BufferRowLength = 0;
				region.ImageSubresource.AspectMask.Set(EImageAspect::Color);
				region.ImageSubresource.MipLevel = 0;
				region.ImageSubresource.BaseArrayLayer = 0;
				region.ImageSubresource.LayerCount = 1;

				region.ImageOffset.Y = 0; 
				region.ImageOffset.Y = 0; 
				region.ImageOffset.Z = 0;

				region.ImageExtent.Width  = _width ;
				region.ImageExtent.Height = _height;
				region.ImageExtent.Depth  = 1      ;

				CommandBuffer::CopyBufferToImage
				(
					commandBuffer,
					_buffer,
					_image,
					EImageLayout::TransferDestination_Optimal,
					1,
					&region
				);

				Vault_2::CommandBuffer::EndSingleTimeCommands(commandBuffer, CommandPool, LogicalDevice.GetHandle(), GraphicsQueue->GetHandle());
			}

			void CreateApplicationInstance
			(
				      RoCStr                          _appName                         , 
				      AppVersion&                     _version                         , 
				const ptr<ValidationLayerList>        _optionalValidationLayers        , 
				const ptr<DebugMessenger::CreateInfo> _optionalDebugMessengerCreateSpec
			)
			{
				using Meta::EEngineVersion;

				Stack
				(
					Vault_4::AppInstance::AppInfo    appSpec       {};
					Vault_4::AppInstance::CreateInfo appCreateSpec {};
				);

				if (Vulkan_EnableValidationLayers)
				{
					if (!CheckValidationLayerSupport(dref(_optionalValidationLayers)))
						throw std::runtime_error("Validation layers requested, but are not available!");
				}

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

				appCreateSpec.AppInfo = getAddress(appSpec);

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

				EResult&& creationResult = AppGPU.Create(appSpec, appCreateSpec);   // AppInstance::Create(appCreateSpec, nullptr, _applicationInstance);

				if (creationResult != EResult::Success) 
					throw std::runtime_error("Triangle Test: Failed to create Vulkan app instance.");
			}

			void CreateCommandBuffers
			(
				LogicalDevice::Handle _logicalDevice         ,

				Pipeline::Handle            _graphicsPipeline      ,
				
				FrameBufferList&      _swapChain_FrameBuffers,
				Extent2D              _swapChain_Extent      ,
				RenderPass::Handle          _renderPass            ,

				CommandPool::Handle         _commandPool           ,
				CommandBufferList&    _commandBufferContainer
			)
			{
				_commandBufferContainer.resize(_swapChain_FrameBuffers.size());

				CommandBuffer::AllocateInfo allocSpec {};

				allocSpec.SType       = allocSpec.STypeEnum                   ;
				allocSpec.Pool        = _commandPool                          ;
				allocSpec.Level       = ECommandBufferLevel::Primary          ;
				allocSpec.BufferCount = uint32(_commandBufferContainer.size());

				if (CommandBuffer::Allocate(_logicalDevice, allocSpec, _commandBufferContainer.data()) != EResult::Success) 
				{
					throw std::runtime_error("failed to allocate command buffers!");
				}

				for (DataSize index = 0; index < _commandBufferContainer.size(); index++) 
				{
					CommandBuffer::BeginInfo beginInfo {};

					beginInfo.SType           = beginInfo.STypeEnum;
					beginInfo.Flags           = 0                  ;   // Optional
					beginInfo.InheritanceInfo = nullptr            ;   // Optional

					if (CommandBuffer::BeginRecord(_commandBufferContainer[index], beginInfo) != EResult::Success) 
						throw std::runtime_error("Failed to begin recording command buffer!");

						RenderPass::BeginInfo renderPassInfo{};

						renderPassInfo.SType       = renderPassInfo.STypeEnum      ;
						renderPassInfo.RenderPass  = _renderPass                   ;
						renderPassInfo.Framebuffer = _swapChain_FrameBuffers[index];

						renderPassInfo.RenderArea.Offset.X = 0;
						renderPassInfo.RenderArea.Offset.Y = 0;

						renderPassInfo.RenderArea.Extent = _swapChain_Extent;

						StaticArray<ClearValue, 2> clearValues {}; 

						clearValues[0].Color        = { 0.0f, 0.0f, 0.0f, 1.0f };
						clearValues[1].DepthStencil = { 1.0f, 0                };

						renderPassInfo.ClearValueCount = SCast<uint32>(clearValues.size());
						renderPassInfo.ClearValues     = clearValues.data()               ;

						CommandBuffer::BeginRenderPass(_commandBufferContainer[index], renderPassInfo, ESubpassContents::Inline);

							CommandBuffer::BindPipeline(_commandBufferContainer[index], EPipelineBindPoint::Graphics, _graphicsPipeline);

							Vault_4::Buffer::Handle vertexBuffers = VertexBuffer.GetHandle();

							DeviceSize offsets = 0;

							CommandBuffer::BindVertexBuffers(_commandBufferContainer[index], 0, 1, vertexBuffers, offsets);

							CommandBuffer::BindIndexBuffer(_commandBufferContainer[index], IndexBuffer.GetHandle(), 0, EIndexType::uInt32);

							CommandBuffer::BindDescriptorSets
							(
								_commandBufferContainer[index],
								EPipelineBindPoint::Graphics,
								PipelineLayout,
								0,
								1,
								&DescriptorSets[index],
								0,
								nullptr
							);

							CommandBuffer::DrawIndexed
							(
								_commandBufferContainer[index],
								SCast<uint32>(ModelIndicies.size()),
								1,
								0,
								0,
								0
							);

						CommandBuffer::EndRenderPass(_commandBufferContainer[index]);

					if (CommandBuffer::EndRecord(_commandBufferContainer[index]) != EResult::Success) 
						throw std::runtime_error("Failed to record command buffer!");
				}
			}

			void CreateColorResources()
			{
				EFormat colorFormat = SwapChain_ImageFormat;

				CreateImage_V4
				(
					SwapChain_Extent.Width, 
					SwapChain_Extent.Height, 
					1, 
					MSAA_Samples, 
					colorFormat, 
					EImageTiling::Optimal, 
					Image::UsageFlags(EImageUsage::Transient_Attachment, EImageUsage::Color_Attachment), 
					EMemoryPropertyFlag::DeviceLocal, 
					ColorImage, 
					ColorImageMemory
				);


				ColorImageView = CreateImageView(ColorImage, colorFormat, EImageAspect::Color, 1);
			}

			void CreateCommandPool
			(
				PhysicalDevice::Handle _physicalDevice, 
				LogicalDevice::Handle  _logicalDevice , 
				Surface::Handle        _surfaceHandle , 
				VkCommandPool&         _commandPool
			)
			{
				//QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(PhysicalDevice, _surfaceHandle);

				CommandPool::CreateInfo poolInfo {};

				poolInfo.SType            = poolInfo.STypeEnum                 ;
				poolInfo.QueueFamilyIndex = QueueIndices.Graphics.value();
				poolInfo.Flags            = CommandPool::CreateFlgas()         ;             // Optional
																					    
				if (CommandPool::Create(_logicalDevice, poolInfo, nullptr, _commandPool) != EResult::Success) 
				{
					throw std::runtime_error("failed to create command pool!");
				}
			}

			void CreateDepthResources()
			{
				EFormat depthFormat = FindDepthFormat();

				CreateImage_V4
				(
					SwapChain_Extent.Width, 
					SwapChain_Extent.Height, 
					1,
					MSAA_Samples,   // TODO: Change me.
					depthFormat, 
					EImageTiling::Optimal, 
					EImageUsage::DepthStencil_Attachment, 
					EMemoryPropertyFlag::DeviceLocal,
					DepthImage,
					DepthImageMemory
				);

				DepthImageView = CreateImageView(DepthImage, depthFormat, Image::AspectFlags(EImageAspect::Depth), 1);

				TransitionImageLayout(DepthImage.GetHandle(), depthFormat, EImageLayout::Undefined, EImageLayout::DepthStencil_AttachmentOptimal, 1);
			}

			void CreateDescriptorPool()
			{
				StaticArray<DescriptorPool::Size, 2> poolSizes {};

				poolSizes[0].Type = EDescriptorType::UniformBuffer;
				poolSizes[0].Count = SCast<uint32>(SwapChain_Images.size());

				poolSizes[1].Type = EDescriptorType::Sampler;
				poolSizes[1].Count = SCast<uint32>(SwapChain_Images.size());

				Vault_4::DescriptorPool::CreateInfo poolInfo {};

				poolInfo.SType         = poolInfo.STypeEnum             ;
				poolInfo.PoolSizeCount = SCast<uint32>(poolSizes.size());
				poolInfo.PoolSizes     = poolSizes.data()               ;

				poolInfo.MaxSets = SCast<uint32>(SwapChain_Images.size());

				//if (DescriptorPool::Create(LogicalDevice.GetHandle(), poolInfo, nullptr, DescriptorPool) != EResult::Success)
				if (DescriptorPool.Create(LogicalDevice, poolInfo, nullptr) != EResult::Success)
					throw RuntimeError("Failed to create descriptor pool!");
			}

			void CreateDescriptorSets()
			{
				DynamicArray<Pipeline::Layout::DescriptorSet::Handle> layouts(SwapChain_Images.size(), DescriptorSetLayout);

				DescriptorSet::AllocateInfo allocInfo{};

				allocInfo.SType              = allocInfo.STypeEnum;
				allocInfo.DescriptorPool     = DescriptorPool.GetHandle();
				allocInfo.DescriptorSetCount = SCast<uint32>(SwapChain_Images.size());
				allocInfo.SetLayouts         = layouts.data();

				DescriptorSets.resize(SwapChain_Images.size());

				if (DescriptorSet::Allocate(LogicalDevice.GetHandle(), allocInfo, DescriptorSets.data()) != EResult::Success)
					throw std::runtime_error("failed to allocate descriptor sets!");

				for (DataSize index = 0; index < SwapChain_Images.size(); index++)
				{
					DescriptorSet::BufferInfo bufferInfo{};

					bufferInfo.Buffer = UniformBuffers[index].GetHandle()      ;
					bufferInfo.Offset = 0                          ;
					bufferInfo.Range  = sizeof(UniformBufferObject);


					DescriptorSet::ImageInfo imageInfo{};

					imageInfo.ImageLayout = EImageLayout::Shader_ReadonlyOptimal;
					imageInfo.ImageView   = TextureImageView.GetHandle()                    ;
					imageInfo.Sampler     = TextureSampler.GetHandle()          ;


					StaticArray<DescriptorSet::Write, 2> descriptorWrites {};

					//DescriptorSet::Write descriptorWrite{};
					
					descriptorWrites[0].SType           = DescriptorSet::Write::STypeEnum;
					descriptorWrites[0].DstSet          = DescriptorSets[index]          ;
					descriptorWrites[0].DstBinding      = 0                              ;
					descriptorWrites[0].DstArrayElement = 0                              ;

					descriptorWrites[0].DescriptorType  = EDescriptorType::UniformBuffer;
					descriptorWrites[0].DescriptorCount = 1                             ;

					descriptorWrites[0].BufferInfo      = &bufferInfo;
					descriptorWrites[0].ImageInfo       = nullptr    ; // Optional
					descriptorWrites[0].TexelBufferView = nullptr    ; // Optional

					descriptorWrites[1].SType           = DescriptorSet::Write::STypeEnum;
					descriptorWrites[1].DstSet          = DescriptorSets[index]          ;
					descriptorWrites[1].DstBinding      = 1                              ;
					descriptorWrites[1].DstArrayElement = 0                              ;

					descriptorWrites[1].DescriptorType  = EDescriptorType::CombinedImageSampler;
					descriptorWrites[1].DescriptorCount = 1                                    ;

					descriptorWrites[1].BufferInfo      = nullptr   ;
					descriptorWrites[1].ImageInfo       = &imageInfo; // Optional
					descriptorWrites[1].TexelBufferView = nullptr   ; // Optional

					DescriptorSet::Update(LogicalDevice.GetHandle(), SCast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);	
				}
			}

			void CreateDescriptorSetLayout()
			{
				Pipeline::Layout::DescriptorSet::Binding uboLayoutBinding {};

				uboLayoutBinding.BindingID = 0;
				uboLayoutBinding.Type      = EDescriptorType::UniformBuffer;
				uboLayoutBinding.Count     = 1;

				uboLayoutBinding.StageFlags = EShaderStageFlag::Vertex;

				uboLayoutBinding.ImmutableSamplers = nullptr;

				Pipeline::Layout::DescriptorSet::Binding samplerLayoutBinding{};

				samplerLayoutBinding.BindingID = 1;
				samplerLayoutBinding.Count     = 1;
				samplerLayoutBinding.Type      = EDescriptorType::CombinedImageSampler;

				samplerLayoutBinding.ImmutableSamplers = nullptr;

				samplerLayoutBinding.StageFlags.Set(EShaderStageFlag::Fragment);

				StaticArray<Pipeline::Layout::DescriptorSet::Binding, 2> bindings =
				{ uboLayoutBinding, samplerLayoutBinding };
				
				Pipeline::Layout::DescriptorSet::CreateInfo layoutInfo {};

				layoutInfo.SType = layoutInfo.STypeEnum;
				layoutInfo.BindingCount = SCast<uint32>(bindings.size());
				layoutInfo.Bindings = bindings.data();

				if (Pipeline::Layout::DescriptorSet::Create(LogicalDevice.GetHandle(), layoutInfo, nullptr, DescriptorSetLayout) != EResult::Success)
					throw RuntimeError("Failed to create descriptor set layout!");
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
					StaticArray<ImageView::Handle, 3> attachments = 
					{
						ColorImageView.GetHandle(),
						DepthImageView.GetHandle(),
						_swapChainImageViews[index]
					};

					Framebuffer::CreateInfo framebufferInfo {};

					framebufferInfo.SType           = framebufferInfo.STypeEnum        ;
					framebufferInfo.RenderPass      = _renderPass                      ;
					framebufferInfo.AttachmentCount = SCast<uint32>(attachments.size());
					framebufferInfo.Attachments     = attachments.data()               ;
					framebufferInfo.Width           = _swapChainExtent.Width           ;
					framebufferInfo.Height          = _swapChainExtent.Height          ;
					framebufferInfo.Layers          = 1                                ;

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
				StaticArray<ShaderModule::Handle, 2> _shaders,
				Pipeline::Layout::Handle& _pipelineLayout  ,
				RenderPass::Handle        _renderPass      ,
				VkPipeline&               _graphicsPipeline   // Will be provided.
			)
			{
				Pipeline::ShaderStage::CreateInfo vertexShaderStage_CreeationSpec{};
				Pipeline::ShaderStage::CreateInfo fragShaderStage_CreationSpec{};

				vertexShaderStage_CreeationSpec.SType = vertexShaderStage_CreeationSpec.STypeEnum;
				vertexShaderStage_CreeationSpec.Stage = EShaderStageFlag::Vertex                 ;

				vertexShaderStage_CreeationSpec.Module = _shaders[0];
				vertexShaderStage_CreeationSpec.Name   = "main"     ;

				fragShaderStage_CreationSpec.SType = fragShaderStage_CreationSpec.STypeEnum;
				fragShaderStage_CreationSpec.Stage = EShaderStageFlag::Fragment            ;

				fragShaderStage_CreationSpec.Module = _shaders[1];
				fragShaderStage_CreationSpec.Name   = "main"     ;

				Pipeline::ShaderStage::CreateInfo shaderStages[] = { vertexShaderStage_CreeationSpec, fragShaderStage_CreationSpec };

				// Fixed Function

				Pipeline::VertexInputState::CreateInfo vertexInputState_CreationSpec{};

				vertexInputState_CreationSpec.SType = vertexInputState_CreationSpec.STypeEnum;

				// TODO: Need to figure out how the GPU is going to be fed shader/vertex information etc.

				// VKTut_V1/2

				auto binding = Vertex::GetBindingDescription();
				auto attributes = Vertex::GetAttributeDescriptions();

				vertexInputState_CreationSpec.VertexBindingDescriptionCount = 1;
				vertexInputState_CreationSpec.AttributeDescriptionCount = SCast<uint32>(attributes.size());

				vertexInputState_CreationSpec.BindingDescriptions = &binding;
				vertexInputState_CreationSpec.AttributeDescription = attributes.data();

				Pipeline::InputAssemblyState::CreateInfo inputAssembly_CreationSpec{};

				inputAssembly_CreationSpec.SType = inputAssembly_CreationSpec.STypeEnum;

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

				viewportState_CreationSpec.SType         = viewportState_CreationSpec.STypeEnum;
				viewportState_CreationSpec.ViewportCount = 1                                   ;
				viewportState_CreationSpec.Viewports     = &viewport                           ;
				viewportState_CreationSpec.ScissorCount  = 1                                   ;
				viewportState_CreationSpec.Scissors      = &scissor                            ;

				Pipeline::RasterizationState::CreateInfo rasterizer{};

				rasterizer.SType                   = rasterizer.STypeEnum;
				rasterizer.EnableDepthClamp        = EBool::False        ;
				rasterizer.EnableRasterizerDiscard = EBool::False        ;
				rasterizer.PolygonMode             = EPolygonMode::Fill  ;
				rasterizer.LineWidth               = 1.0f                ;

				rasterizer.CullMode.Set(ECullModeFlag::Back);

				rasterizer.FrontFace               = EFrontFace::CounterClockwise;
				rasterizer.EnableDepthBias         = EBool::False                ;
				rasterizer.DepthBiasConstantFactor = 0.0f                        ;
				rasterizer.DepthBiasClamp          = 0.0f                        ;
				rasterizer.DepthBiasSlopeFactor    = 0.0f                        ;

				Pipeline::MultisampleState::CreateInfo multisampling_CreationSpec{};

				multisampling_CreationSpec.SType                 = multisampling_CreationSpec.STypeEnum;
				multisampling_CreationSpec.EnableSampleShading   = EBool::False                        ;
				multisampling_CreationSpec.RasterizationSamples  = MSAA_Samples                        ;
				multisampling_CreationSpec.MinSampleShading      = 1.0f                                ;
				multisampling_CreationSpec.SampleMask            = nullptr                             ;
				multisampling_CreationSpec.EnableAlphaToCoverage = EBool::False                        ;
				multisampling_CreationSpec.EnableAlphaToOne      = EBool::False                        ;

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

				colorBlending_CreationSpec.SType                 = colorBlending_CreationSpec.STypeEnum;
				colorBlending_CreationSpec.EnableLogicOperations = EBool::False                        ;
				colorBlending_CreationSpec.LogicOperation        = ELogicOperation::Copy               ;
				colorBlending_CreationSpec.AttachmentCount       = 1                                   ;
				colorBlending_CreationSpec.Attachments           = &colorBlend_Attachment              ;
				colorBlending_CreationSpec.BlendConstants[0]     = 0.0f                                ;
				colorBlending_CreationSpec.BlendConstants[1]     = 0.0f                                ;
				colorBlending_CreationSpec.BlendConstants[2]     = 0.0f                                ;
				colorBlending_CreationSpec.BlendConstants[3]     = 0.0f                                ;

				Pipeline::DepthStencilState::CreateInfo depthStencil_Info {};

				depthStencil_Info.SType = depthStencil_Info.STypeEnum;
				depthStencil_Info.DepthTestEnable = EBool::True;
				depthStencil_Info.DepthWriteEnable = EBool::True;
				
				depthStencil_Info.DepthCompareOp = ECompareOperation::Less;
				
				depthStencil_Info.DepthBoundsTestEnable = EBool::False;

				depthStencil_Info.MinDepthBounds = 0.0f;
				depthStencil_Info.MaxDepthBounds = 1.0f;

				depthStencil_Info.StencilTestEnable = EBool::False;
				depthStencil_Info.Front;   // Optional
				depthStencil_Info.Back ;   // Optional



				EDynamicState States[] =
				{
					EDynamicState::Viewport ,
					EDynamicState::LineWidth
				};

				Pipeline::DynamicState::CreateInfo dynamicState {};

				dynamicState.SType      = dynamicState.STypeEnum;
				dynamicState.StateCount = 2                     ;
				dynamicState.States     = States                ;

				Pipeline::Layout::CreateInfo pipelineLayout_CreationSpec {};

				pipelineLayout_CreationSpec.SType                  = pipelineLayout_CreationSpec.STypeEnum;
				pipelineLayout_CreationSpec.SetLayoutCount         = 1                                    ;
				pipelineLayout_CreationSpec.SetLayouts             = &DescriptorSetLayout                 ;
				pipelineLayout_CreationSpec.PushConstantRangeCount = 0                                    ;
				pipelineLayout_CreationSpec.PushConstantRanges     = nullptr                              ;

				EResult&& piplineLayout_CreationResult = 
					Pipeline::Layout::Create(_logicalDevice, pipelineLayout_CreationSpec, nullptr, _pipelineLayout);

				if (piplineLayout_CreationResult != EResult::Success)
				{
					throw std::runtime_error("Failed to create pipeline layout!");
				}

				Pipeline::Graphics::CreateInfo pipelineInfo {};

				pipelineInfo.SType      = pipelineInfo.STypeEnum;
				pipelineInfo.StageCount = 2                     ;
				pipelineInfo.Stages     = shaderStages          ;

				pipelineInfo.VertexInputState   = &vertexInputState_CreationSpec;
				pipelineInfo.InputAssemblyState = &inputAssembly_CreationSpec   ;
				pipelineInfo.ViewportState      = &viewportState_CreationSpec   ;
				pipelineInfo.RasterizationState = &rasterizer                   ;
				pipelineInfo.MultisampleState   = &multisampling_CreationSpec   ;
				pipelineInfo.DepthStencilState  = &depthStencil_Info            ;   
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
			}	

			void CreateImage
			(
				uint32                _width      , 
				uint32                _height     , 
				uint32                _mipLevels  ,
				ESampleCount          _numSamples ,
				EFormat               _format     , 
				EImageTiling          _tiling     , 
				Image::UsageFlags     _usage      , 
				Memory::PropertyFlags _properties , 
				Image::Handle&        _image      , 
				Memory::Handle&       _imageMemory
			)
			{
				Image::CreateInfo imageInfo {};

				imageInfo.SType         = imageInfo.STypeEnum   ;
				imageInfo.ImageType     = EImageType::_2D       ;
				imageInfo.Extent.Width  = SCast<uint32>(_width );
				imageInfo.Extent.Height = SCast<uint32>(_height);
				imageInfo.Extent.Depth  = 1                     ;
				imageInfo.MipmapLevels  = _mipLevels            ;
				imageInfo.ArrayLayers   = 1                     ;

				imageInfo.Format       = _format                ;
				imageInfo.Tiling       = _tiling                ;
				imageInfo.InitalLayout = EImageLayout::Undefined;
				imageInfo.Usage        = _usage                 ;
				imageInfo.SharingMode  = ESharingMode::Exclusive ;
				imageInfo.Samples      = _numSamples            ;
				imageInfo.Flags        = 0                      ;

				if (Image::Create(LogicalDevice.GetHandle(), imageInfo, nullptr, _image) != EResult::Success)
					throw RuntimeError("Failed to create image!");

				Memory::Requirements memReq;

				Image::GetMemoryRequirements(LogicalDevice.GetHandle(), _image, memReq);

				Memory::AllocateInfo allocationInfo {};

				allocationInfo.SType = allocationInfo.STypeEnum;
				allocationInfo.AllocationSize = memReq.Size;
				allocationInfo.MemoryTypeIndex = PhysicalDevice.FindMemoryType(memReq.MemoryTypeBits, _properties);

				if (Memory::Allocate(LogicalDevice.GetHandle(), allocationInfo, nullptr, _imageMemory) != EResult::Success)
					throw RuntimeError("Failed to allocate image memory!");

				Image::BindMemory(LogicalDevice.GetHandle(), _image, _imageMemory, 0);
			}

			void CreateImage_V4
			(
				uint32                _width      , 
				uint32                _height     , 
				uint32                _mipLevels  ,
				ESampleCount          _numSamples ,
				EFormat               _format     , 
				EImageTiling          _tiling     , 
				Image::UsageFlags     _usage      , 
				Memory::PropertyFlags _properties , 
				Vault_4::Image&        _image      , 
				Vault_4::Memory&       _imageMemory
			)
			{
				Vault_4::Image::CreateInfo imageInfo;

				imageInfo.ImageType     = EImageType::_2D       ;
				imageInfo.Extent.Width  = SCast<uint32>(_width );
				imageInfo.Extent.Height = SCast<uint32>(_height);
				imageInfo.Extent.Depth  = 1                     ;
				imageInfo.MipmapLevels  = _mipLevels            ;
				imageInfo.ArrayLayers   = 1                     ;

				imageInfo.Format       = _format                ;
				imageInfo.Tiling       = _tiling                ;
				imageInfo.InitalLayout = EImageLayout::Undefined;
				imageInfo.Usage        = _usage                 ;
				imageInfo.SharingMode  = ESharingMode::Exclusive ;
				imageInfo.Samples      = _numSamples            ;
				imageInfo.Flags        = 0                      ;

				//if (Image::Create(LogicalDevice.GetHandle(), imageInfo, nullptr, _image) != EResult::Success)
				if (_image.Create(LogicalDevice, imageInfo, Memory::DefaultAllocator) != EResult::Success)
					throw RuntimeError("Failed to create image!");

				//Memory::Requirements memReq;

				//Image::GetMemoryRequirements(LogicalDevice.GetHandle(), _image, memReq);

				Vault_4::Memory::AllocateInfo allocationInfo;

				//allocationInfo.SType = allocationInfo.STypeEnum;
				allocationInfo.AllocationSize = _image.GetMemoryRequirements().Size;
				allocationInfo.MemoryTypeIndex = PhysicalDevice.FindMemoryType(_image.GetMemoryRequirements().MemoryTypeBits, _properties);

				//if (Memory::Allocate(LogicalDevice.GetHandle(), allocationInfo, nullptr, _imageMemory) != EResult::Success)
				if (_imageMemory.Allocate(LogicalDevice, allocationInfo, Memory::DefaultAllocator) != EResult::Success)
					throw RuntimeError("Failed to allocate image memory!");

				//Image::BindMemory(LogicalDevice.GetHandle(), _image, _imageMemory, 0);

				_image.BindMemory(_imageMemory, Memory::ZeroOffset);
			}

			Vault_4::ImageView CreateImageView(Vault_4::Image _image, EFormat _format, Image::AspectFlags _aspectFlags, uint32 _miplevels)
			{
				Vault_4::ImageView::CreateInfo viewInfo;

				//viewInfo.SType    = viewInfo.STypeEnum       ;
				viewInfo.Image    = _image.GetHandle()       ;
				viewInfo.ViewType = ImageView::EViewType::_2D;
				viewInfo.Format   = _format                  ;

				viewInfo.Components.R = EComponentSwizzle::Identitity;
				viewInfo.Components.G = EComponentSwizzle::Identitity;
				viewInfo.Components.B = EComponentSwizzle::Identitity;
				viewInfo.Components.A = EComponentSwizzle::Identitity;

				viewInfo.SubresourceRange.AspectMask     = _aspectFlags;
				viewInfo.SubresourceRange.BaseMipLevel   = 0           ;
				viewInfo.SubresourceRange.LevelCount     = _miplevels  ;
				viewInfo.SubresourceRange.BaseArrayLayer = 0           ;
				viewInfo.SubresourceRange.LayerCount     = 1           ;

				Vault_4::ImageView result;

				//if (ImageView::Create(LogicalDevice.GetHandle(), viewInfo, nullptr, result) != EResult::Success )
				if (result.Create(LogicalDevice, viewInfo, Memory::DefaultAllocator) != EResult::Success )
					throw RuntimeError("Failed to create texture image view!");

				return result;
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
					Vault_4::ImageView::CreateInfo creationSpec;

					//creationSpec.SType    = creationSpec.STypeEnum ;
					creationSpec.Image    = _swapChainImages[index];
					creationSpec.ViewType = EImageViewType::_2D    ;
					creationSpec.Format   = _swapChainImageFormat  ;

					creationSpec.Components.R = EComponentSwizzle::Identitity;
					creationSpec.Components.G = EComponentSwizzle::Identitity;
					creationSpec.Components.B = EComponentSwizzle::Identitity;
					creationSpec.Components.A = EComponentSwizzle::Identitity;

					creationSpec.SubresourceRange.AspectMask.Set(EImageAspect::Color);

					creationSpec.SubresourceRange.BaseMipLevel   = 0;
					creationSpec.SubresourceRange.LevelCount     = 1;
					creationSpec.SubresourceRange.BaseArrayLayer = 0;
					creationSpec.SubresourceRange.LayerCount     = 1;

					EResult&& creationResult = 
						//_imageViewContainer[index].Create(LogicalDevice, creationSpec, Memory::DefaultAllocator);
						ImageView::Create(_logicalDevice, creationSpec, nullptr, _imageViewContainer[index]);

					if (creationResult != EResult::Success)
					{
						throw std::runtime_error("Failed to create image views!");
					}
				}
			}

			void CreateIndexBuffer()
			{
				DeviceSize bufferSize = sizeof(ModelIndicies[0]) * ModelIndicies.size();

				Buffer::Handle     stagingBuffer;
				Vault_2::Buffer::CreateInfo stagingBufferInfo {};
				Memory::Handle     stagingBufferMemory;

				stagingBufferInfo.SharingMode = ESharingMode::Exclusive;
				stagingBufferInfo.Size        = bufferSize            ; 

				stagingBufferInfo.Usage.Set(EBufferUsage::TransferSource);

				Vault_2::Buffer::CreateAndBind
				(
					PhysicalDevice.GetHandle(), 
					LogicalDevice.GetHandle(),
					stagingBufferInfo, 
					stagingBuffer, 
					Memory::PropertyFlags(EMemoryPropertyFlag::HostVisible, EMemoryPropertyFlag::HostCoherent), 
					stagingBufferMemory, 
					nullptr
				);

				// Hard coded vertex data...

				VoidPtr&& address = ModelIndicies.data();

				Vault_2::Memory::WriteToGPU(LogicalDevice.GetHandle(), stagingBufferMemory, 0, bufferSize, 0, address);

				Vault_2::Buffer::CreateInfo indexBufferInfo {};

				indexBufferInfo.SharingMode = ESharingMode::Exclusive;
				indexBufferInfo.Size        = bufferSize;

				indexBufferInfo.Usage.Set(EBufferUsage::TransferDestination, EBufferUsage::IndexBuffer);

				IndexBuffer.CreateAndBind
				(
					PhysicalDevice, 
					LogicalDevice, 
					indexBufferInfo, 
					Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal), 
					IndexBufferMemory, 
					Memory::DefaultAllocator
				);

				Buffer::CopyInfo copyInfo {}; copyInfo.DestinationOffset = 0; copyInfo.SourceOffset = 0; copyInfo.Size = bufferSize;

				Vault_2::CommandBuffer::CopyBuffer(stagingBuffer, IndexBuffer.GetHandle(), copyInfo, LogicalDevice.GetHandle(), CommandPool, GraphicsQueue->GetHandle());

				Buffer::Destroy(LogicalDevice.GetHandle(), stagingBuffer, Memory::DefaultAllocator);

				Memory::Free(LogicalDevice.GetHandle(), stagingBufferMemory, Memory::DefaultAllocator);
			}

			void CreateLogicalDevice
			(
				PhysicalDevice::Handle        _physicalDevice          , 
				Surface::Handle               _surfaceHanle            ,
				ExtensionIdentifierList       _extensionsToSpecify     ,
				ptr<ValidationLayerList>      _optionalValidationLayers
			)	
			{
				std::vector<Vault_4::LogicalDevice::Queue::CreateInfo> queueCreateInfos;

				float32 queuePriority = 1.0f;

				for (auto queueFamily : QueueIndices.GetSet())
				{
					Vault_4::LogicalDevice::Queue::CreateInfo createInfo{};

					createInfo.QueueFamilyIndex = queueFamily;
					createInfo.QueueCount       = 1;
					createInfo.QueuePriorities  = &queuePriority;

					if (GraphicsQueue->FamilySpecified() && GraphicsQueue->GetFamilyIndex() == queueFamily)
					{
						GraphicsQueue->Assign(LogicalDevice, createInfo);
					}

					if (PresentationQueue->FamilySpecified() && PresentationQueue->GetFamilyIndex() == queueFamily && PresentationQueue != GraphicsQueue)
					{
						PresentationQueue->Assign(LogicalDevice, createInfo);
					}

					queueCreateInfos.push_back(createInfo);
				}

				PhysicalDevice::Features physDeviceFeatures{};

				physDeviceFeatures.SamplerAnisotropy = EBool::True;	

				Vault_4::LogicalDevice::CreateInfo createInfo{};

				createInfo.QueueCreateInfoCount  = queueCreateInfos.size()    ;
				createInfo.QueueCreateInfos      = queueCreateInfos.data()    ;
				createInfo.EnabledFeatures       = &physDeviceFeatures        ;
				createInfo.EnabledExtensionNames = _extensionsToSpecify.data();
				createInfo.EnabledExtensionCount = _extensionsToSpecify.size();

				if (Vulkan_EnableValidationLayers)
				{
					createInfo.EnabledLayerCount = SCast<uint32>(_optionalValidationLayers->size());
					createInfo.EnabledLayerNames = _optionalValidationLayers->data( )              ;
				}
				else
				{
					createInfo.EnabledLayerCount = 0;
				}

				EResult&& result = LogicalDevice.Create(PhysicalDevice, createInfo, nullptr);

				if (result != EResult::Success)
				{
					throw std::runtime_error("Vulkan: Failed to create logical device!");
				}

				GraphicsQueue->Retrieve();

				if (PresentationQueue->GetHandle() == NULL)
				{
					PresentationQueue->Retrieve();
				}
			}

			void CreateRenderPass
			(
				LogicalDevice::Handle _logicalDevice,
				EFormat               _imageFormat  ,
				VkRenderPass&         _renderPass       // Will be provided.
			)
			{
				RenderPass::AttachmentDescription colorAttachment {};

				colorAttachment.Format  = _imageFormat;
				colorAttachment.Samples = MSAA_Samples;

				colorAttachment.LoadOp  = EAttachmentLoadOperation ::Clear;
				colorAttachment.StoreOp = EAttachmentStoreOperation::Store;

				colorAttachment.StencilLoadOp  = EAttachmentLoadOperation ::DontCare;
				colorAttachment.StencilStoreOp = EAttachmentStoreOperation::DontCare;

				colorAttachment.InitialLayout = EImageLayout::Undefined        ;
				colorAttachment.FinalLayout   = EImageLayout::Color_AttachmentOptimal;

				RenderPass::AttachmentReference colorAttachmentRef {};

				colorAttachmentRef.Attachment = 0                                    ;
				colorAttachmentRef.Layout     = EImageLayout::Color_AttachmentOptimal;

				RenderPass::AttachmentDescription depthAttachment {};

				depthAttachment.Format = FindDepthFormat();
				depthAttachment.Samples = MSAA_Samples;

				depthAttachment.LoadOp = EAttachmentLoadOperation::Clear;
				depthAttachment.StoreOp = EAttachmentStoreOperation::DontCare;

				depthAttachment.StencilLoadOp = EAttachmentLoadOperation::DontCare;
				depthAttachment.StencilStoreOp = EAttachmentStoreOperation::DontCare;

				depthAttachment.InitialLayout = EImageLayout::Undefined;
				depthAttachment.FinalLayout = EImageLayout::DepthStencil_AttachmentOptimal;

				RenderPass::AttachmentReference depthAttachmentRef {};

				depthAttachmentRef.Attachment = 1;
				depthAttachmentRef.Layout = EImageLayout::DepthStencil_AttachmentOptimal;

				RenderPass::AttachmentDescription colorAttachmentResolve {};

				colorAttachmentResolve.Format = SwapChain_ImageFormat;

				colorAttachmentResolve.Samples = ESampleCount::_1;

				colorAttachmentResolve.LoadOp  = EAttachmentLoadOperation ::DontCare;
				colorAttachmentResolve.StoreOp = EAttachmentStoreOperation::Store   ;

				colorAttachmentResolve.StencilLoadOp  = EAttachmentLoadOperation ::DontCare;
				colorAttachmentResolve.StencilStoreOp = EAttachmentStoreOperation::DontCare;

				colorAttachmentResolve.InitialLayout = EImageLayout::Undefined        ;
				colorAttachmentResolve.FinalLayout   = EImageLayout::PresentSource_KHR;

				RenderPass::AttachmentReference colorAttachmentResolveRef {};

				colorAttachmentResolveRef.Attachment = 2;

				colorAttachmentResolveRef.Layout = EImageLayout::Color_AttachmentOptimal;

				RenderPass::SubpassDescription subpass {};

				subpass.PipelineBindPoint      = EPipelineBindPoint::Graphics;
				subpass.ColorAttachmentCount   = 1                           ;
				subpass.ColorAttachments       = &colorAttachmentRef         ;
				subpass.DepthStencilAttachment = &depthAttachmentRef         ;
				subpass.ResolveAttachments     = &colorAttachmentResolveRef  ;

				StaticArray<RenderPass::AttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };

				RenderPass::CreateInfo renderPassInfo{};

				renderPassInfo.SType           = renderPassInfo.STypeEnum         ;
				renderPassInfo.AttachmentCount = SCast<uint32>(attachments.size());
				renderPassInfo.Attachments     = attachments.data()               ;
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
				Window*             _window             , 
				Surface::Handle&    _surfaceHandle          // Will be provided.
			)
			{
				Surface::CreateInfo createInfo{};

				createInfo.SType       = createInfo.STypeEnum             ;
				createInfo.OSWinHandle = OSAL::GetOS_WindowHandle(_window);   // TODO: Use OSAL instead.
				createInfo.OSAppHandle = Surface::GetOS_AppHandle()       ;

				if (Vault_1::Surface::Create(AppGPU.GetHandle(), createInfo, nullptr, _surfaceHandle) != EResult::Success) 
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

				creationSpec.SType            = creationSpec.STypeEnum  ;
				creationSpec.Surface          = _surfaceHandle          ;
				creationSpec.MinImageCount    = numImagesDesired        ;
				creationSpec.ImageFormat      = surfaceFormat.Format    ;
				creationSpec.ImageColorSpace  = surfaceFormat.ColorSpace;
				creationSpec.ImageExtent      = extent                  ;
				creationSpec.ImageArrayLayers = 1                       ;

				creationSpec.ImageUsage.Set(EImageUsage::Color_Attachment);

				//QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice, _surfaceHandle);

				uint32 queueFamilyIndices[] = 
				{
					QueueIndices.Graphics.value()
				};

				if (QueueIndices.Graphics != QueueIndices.PresentationSupported) 
				{
					creationSpec.ImageSharingMode      = ESharingMode::Concurrent;
					creationSpec.QueueFamilyIndexCount = 2                       ;
					creationSpec.QueueFamilyIndices    = queueFamilyIndices      ;
				}
				else 
				{
					creationSpec.ImageSharingMode      = ESharingMode::Exclusive;
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

				_swapChain_Images.resize(Vault_2::SwapChain::GetImageCount(_logicalDevice, _swapChain));

				Vault_2::SwapChain::GetImages(_logicalDevice, _swapChain, _swapChain_Images.data());

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

				semaphore_CreationSpec.SType = semaphore_CreationSpec.STypeEnum;

				Fence::CreateInfo fence_CreationSpec {};

				fence_CreationSpec.SType = fence_CreationSpec.STypeEnum;

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

			void CreateTextureImage(RoCStr _filePath)
			{
				int textureWidth, textureHeight, textureChannels;

				using ImageBytes = stbi_uc;

				ImageBytes* imageData = stbi_load(_filePath, &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);

				DeviceSize imageSize = textureWidth * textureHeight	 * 4;

				MipMapLevels = SCast<uint32>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1;

				if (!imageData)
				{
					throw RuntimeError("Failed to load image data!");
				}

				Buffer::Handle stagingBuffer;

				Memory::Handle stagingBufferMemory;

				Vault_2::Buffer::CreateInfo stagingBufferInfo {};

				stagingBufferInfo.Size = imageSize;
				stagingBufferInfo.SharingMode = ESharingMode::Exclusive;

				stagingBufferInfo.Usage.Set(EImageUsage::TransferSource);

				Vault_2::Buffer::CreateAndBind
				(
					PhysicalDevice.GetHandle(),
					LogicalDevice.GetHandle(),
					stagingBufferInfo,
					stagingBuffer,
					Memory::PropertyFlags(EMemoryPropertyFlag::HostVisible, EMemoryPropertyFlag::HostCoherent),
					stagingBufferMemory,
					nullptr
				);

				VoidPtr address = imageData;

				Vault_2::Memory::WriteToGPU(LogicalDevice.GetHandle(), stagingBufferMemory, 0, imageSize, 0, address);

				stbi_image_free(imageData);

				CreateImage_V4
				(
					textureWidth, 
					textureHeight,  
					MipMapLevels,
					ESampleCount::_1,   // TODO: Change me.
					EFormat::R8_G8_B8_A8_SRGB, 
					EImageTiling::Optimal, 
					Image::UsageFlags(EImageUsage::TransferDestination, EImageUsage::Sampled, EImageUsage::TransferSource), 
					Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal),
					TextureImage, 
					TextureImageMemory
				);

				TransitionImageLayout(TextureImage.GetHandle(), EFormat::R8_G8_B8_A8_SRGB, EImageLayout::Undefined, EImageLayout::TransferDestination_Optimal, MipMapLevels);

				CopyBufferToImage(stagingBuffer, TextureImage.GetHandle(), SCast<uint32>(textureWidth), SCast<uint32>(textureHeight));

				GenerateMipMaps(TextureImage.GetHandle(), EFormat::R8_G8_B8_A8_SRGB, textureWidth, textureHeight, MipMapLevels);

				Buffer::Destroy(LogicalDevice.GetHandle(), stagingBuffer, nullptr);

				Memory::Free(LogicalDevice.GetHandle(), stagingBufferMemory, nullptr);
			}

			void CreateTextureImageView()
			{
				TextureImageView = CreateImageView(TextureImage, EFormat::R8_G8_B8_A8_SRGB, Image::AspectFlags(EImageAspect::Color), MipMapLevels);
			}

			void CreateTextureSampler()
			{
				Vault_4::Sampler::CreateInfo samplerInfo {};

				samplerInfo.SType = samplerInfo.STypeEnum;

				samplerInfo.MagnificationFilter = EFilter::Linear;
				samplerInfo.MinimumFilter = EFilter::Linear;

				samplerInfo.AddressModeU = ESamplerAddressMode::Repeat;
				samplerInfo.AddressModeV = ESamplerAddressMode::Repeat;
				samplerInfo.AddressModeW = ESamplerAddressMode::Repeat;

				samplerInfo.AnisotropyEnable = EBool::True;
				samplerInfo.MaxAnisotropy    = 16.0f      ;
				samplerInfo.BorderColor = EBorderColor::Int_OpaqueBlack;

				samplerInfo.UnnormalizedCoordinates = EBool::False;

				samplerInfo.CompareEnable = EBool::False;
				samplerInfo.CompareOperation = ECompareOperation::Always;

				samplerInfo.MipmapMode = ESamplerMipmapMode::Linear;
				samplerInfo.MipLodBias = 0.0f                      ;
				samplerInfo.MinimumLod = 0.0f                      ;
				//samplerInfo.MinimumLod = SCast<float32>(MipMapLevels / 2);
				samplerInfo.MaxLod     = SCast<float32>(MipMapLevels);

				//if (Sampler::Create(LogicalDevice.GetHandle(), samplerInfo, nullptr, TextureSampler) != EResult::Success)
				if (TextureSampler.Create(LogicalDevice, samplerInfo, nullptr) != EResult::Success)
					throw RuntimeError("Failed to create texture sampler!");
			}

			StaticArray<ShaderModule::Handle, 2> CreateTriShaders(LogicalDevice::Handle _logicalDevice)
			{
				using Bytecode_Buffer = DynamicArray<Bytecode>;

				// Shader setup

				using namespace Renderer::Shader;

				auto triShader_VertCode = IO::BufferFile(String(Paths::TriangleShader) + "TriangleShader_Vert.spv");
				auto triShader_FragCode = IO::BufferFile(String(Paths::TriangleShader) + "TriangleShader_Frag.spv");

				//TODO: FIGURE THIS OUT.
				Bytecode_Buffer triShader_Vert_Bytecode(triShader_VertCode.begin(), triShader_VertCode.end());
				Bytecode_Buffer triShader_Frag_Bytecode(triShader_FragCode.begin(), triShader_FragCode.end());

				ShaderModule::Handle triShaderModule_Vert; 

				Vault_2::ShaderModule::Create(_logicalDevice, Vault_2::ShaderModule::CreateInfo(triShader_VertCode.data(), triShader_VertCode.size()), nullptr, triShaderModule_Vert);

				ShaderModule::Handle triShaderModule_Frag;

				Vault_2::ShaderModule::Create(_logicalDevice, Vault_2::ShaderModule::CreateInfo(triShader_FragCode.data(), triShader_FragCode.size()), nullptr, triShaderModule_Frag);

				StaticArray<ShaderModule::Handle, 2> result = { triShaderModule_Vert, triShaderModule_Frag };

				return result;
			}

			void CreateUniformBuffers()
			{
				DeviceSize bufferSize = sizeof(UniformBufferObject);

				UniformBuffers      .resize(SwapChain_Images.size());
				UniformBuffersMemory.resize(SwapChain_Images.size());

				Vault_2::Buffer::CreateInfo uniformBufferInfo {};

				uniformBufferInfo.Size = bufferSize;
				uniformBufferInfo.Usage.Set(EBufferUsage::UniformBuffer);
				uniformBufferInfo.SharingMode = ESharingMode::Exclusive;

				for (DataSize index = 0; index < SwapChain_Images.size(); index++)
				{
					UniformBuffers[index].CreateAndBind
					(
						PhysicalDevice,
						LogicalDevice,
						uniformBufferInfo,
						Memory::PropertyFlags(EMemoryPropertyFlag::HostVisible, EMemoryPropertyFlag::HostCoherent),
						UniformBuffersMemory[index],
						Memory::DefaultAllocator
					);
				}

			}

			StaticArray<ShaderModule::Handle, 2> Create_VKTut_Shaders(LogicalDevice::Handle _logicalDevice)
			{
				using Bytecode_Buffer = DynamicArray<Bytecode>;

				// Shader setup

				using namespace Renderer::Shader;

				auto vertCode = IO::BufferFile(String(Paths::VKTut) + "VKTut_V5_Vert.spv");
				auto fragCode = IO::BufferFile(String(Paths::VKTut) + "VKTut_V5_Frag.spv");

				//TODO: FIGURE THIS OUT.
				Bytecode_Buffer vertBytecode(vertCode.begin(), vertCode.end());
				Bytecode_Buffer fragBytecode(fragCode.begin(), fragCode.end());

				ShaderModule::Handle vertShaderModule;
				
				Vault_2::ShaderModule::Create(_logicalDevice, Vault_2::ShaderModule::CreateInfo(vertCode.data(), vertCode.size()), nullptr, vertShaderModule);
				
				ShaderModule::Handle fragShaderModule;
				
				Vault_2::ShaderModule::Create(_logicalDevice, Vault_2::ShaderModule::CreateInfo(fragCode.data(), fragCode.size()), nullptr, fragShaderModule);

				StaticArray<ShaderModule::Handle, 2> result = { vertShaderModule, fragShaderModule };

				return result;
			}

			void CreateVertexBuffers(PhysicalDevice::Handle _physicalDevice, LogicalDevice::Handle _device, Vault_4::Buffer& _vertexBuffer, Vault_4::Memory&_vertexBufferMemory)
			{
				DeviceSize bufferSize = sizeof(ModelVerticies[0]) * ModelVerticies.size();

				Buffer::Handle stagingBuffer;

				Memory::Handle stagingBufferMemory;

				Vault_2::Buffer::CreateInfo stagingBufferInfo {};

				stagingBufferInfo.SharingMode = ESharingMode::Exclusive;
				stagingBufferInfo.Size        = bufferSize;

				stagingBufferInfo.Usage.Set(EBufferUsage::TransferSource);

				Vault_2::Buffer::CreateAndBind
				(
					PhysicalDevice.GetHandle(),
					LogicalDevice.GetHandle(),
					stagingBufferInfo,
					stagingBuffer, 
					Memory::PropertyFlags(EMemoryPropertyFlag::HostVisible, EMemoryPropertyFlag::HostCoherent), 
					stagingBufferMemory,
					Memory::DefaultAllocator
				);

				// Hard coded vertex data...

				VoidPtr vertexData = ModelVerticies.data();

				Memory::MapFlags mapflags;

				Vault_2::Memory::WriteToGPU(_device, stagingBufferMemory, 0, bufferSize, mapflags, vertexData);

				Vault_2::Buffer::CreateInfo vertexBufferInfo {};

				vertexBufferInfo.Size = bufferSize;
				vertexBufferInfo.SharingMode = ESharingMode::Exclusive;
				vertexBufferInfo.Usage.Set(EBufferUsage::TransferDestination, EBufferUsage::VertexBuffer);

				VertexBuffer.CreateAndBind
				(
					PhysicalDevice, 
					LogicalDevice, 
					vertexBufferInfo, 
					Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal), 
					VertexBufferMemory, 
					Memory::DefaultAllocator
				);

				Buffer::CopyInfo copyInfo {}; copyInfo.DestinationOffset = 0; copyInfo.SourceOffset = 0; copyInfo.Size = bufferSize;

				Vault_2::CommandBuffer::CopyBuffer(stagingBuffer, _vertexBuffer.GetHandle(), copyInfo, LogicalDevice.GetHandle(), CommandPool, GraphicsQueue->GetHandle());

				Buffer::Destroy(LogicalDevice.GetHandle(), stagingBuffer, Memory::DefaultAllocator);

				Memory::Free(LogicalDevice.GetHandle(), stagingBufferMemory, Memory::DefaultAllocator);
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

			EFormat FindDepthFormat()
			{
				return	FindSupportedFormat
				( 
					{ EFormat::D32_SFloat, EFormat::D32_SFloat_S8_UInt, EFormat::D24_UNormalized_S8_UInt }, 
					EImageTiling::Optimal, 
					EFormatFeatureFlag::DepthStencilAttachment 
				);
			}

			QueueFamilyIndices FindQueueFamilies
			(
				Vault_4::PhysicalDevice _physicalDevice,
				Surface::Handle _surfaceHandle
			)
			{
				QueueFamilyIndices indices {};

				auto&& queueFamilies = _physicalDevice.GetAvailableQueueFamilies();

				int index = 0;

				for (const auto& queueFamily : queueFamilies)
				{
					if (queueFamily.QueueFlags.Has(EQueueFlag::Graphics))
					{
						indices.Graphics = index;

						DeviceQueues.push_back(Vault_4::LogicalDevice::Queue());

						GraphicsQueue = &DeviceQueues.back();

						GraphicsQueue->SpecifyFamily(index, Vault_4::LogicalDevice::Queue::EType::Graphics);	
					}

					if (queueFamily.QueueFlags.Has(EQueueFlag::Compute))
					{
						indices.Compute = index;
					}

					if (queueFamily.QueueFlags.Has(EQueueFlag::Transfer))
					{
						indices.Transfer = index;
					}

					if (queueFamily.QueueFlags.Has(EQueueFlag::SparseBinding))
					{
						indices.SparseBinding = index;
					}

					
					if 
					(
						indices.Graphics.has_value()
					)
					{
						break;
					}

					index++;

					if (index == queueFamilies.size())
					{
						break;
					}
				}

				return indices;
			}

			EFormat FindSupportedFormat(const DynamicArray<EFormat>& _canidates, EImageTiling _tiling, FormatFeatureFlags _features)
			{
				for (EFormat format : _canidates)
				{
					FormatProperties properties;

					PhysicalDevice::GetFormatProperties(PhysicalDevice.GetHandle(), format, properties);

					if (_tiling == EImageTiling::Linear && (properties.LinearTilingFeatures & _features) == _features)
					{
						return format;
					}
					else if (_tiling == EImageTiling::Optimal && (properties.OptimalTilingFeatures & _features) == _features)
					{
						return format;
					}
				}

				throw RuntimeError("Failed to find supported format!");
			}

			void GenerateMipMaps(Image::Handle _image, EFormat _format, uint32 _textureWidth, uint32 _textureHeight, uint32 _mipLevels)
			{
				// Check if image format supports linear blitting
				FormatProperties formatProperties;
				
				PhysicalDevice::GetFormatProperties(PhysicalDevice.GetHandle(), _format, formatProperties);

				if (!(formatProperties.OptimalTilingFeatures.Has(EFormatFeatureFlag::SampledImageFilterLinear)))
				{
					throw std::runtime_error("Texture image format does not support linear blitting!");
				}

				CommandBuffer::Handle commandBuffer = Vault_2::CommandBuffer::BeginSingleTimeCommands(LogicalDevice.GetHandle(), CommandPool);

				Image::Memory_Barrier barrier{};

				barrier.SType = barrier.STypeEnum;
				barrier.Image = _image;
				barrier.SrcQueueFamilyIndex = QueueFamily_Ignored;
				barrier.DstQueueFamilyIndex = QueueFamily_Ignored;
				barrier.SubresourceRange.AspectMask.Set(EImageAspect::Color);
				barrier.SubresourceRange.BaseArrayLayer = 0;
				barrier.SubresourceRange.LayerCount = 1;
				barrier.SubresourceRange.LevelCount = 1;

				sint32 mipWidth = _textureWidth;
				sint32 mipHeight = _textureHeight;

				for (uint32_t index = 1; index < _mipLevels; index++)
				{
					barrier.SubresourceRange.BaseMipLevel = index - 1;

					barrier.OldLayout = EImageLayout::TransferDestination_Optimal;
					barrier.NewLayout = EImageLayout::TransferSource_Optimal;

					barrier.SrcAccessMask.Set(EAccessFlag::TransferWrite);
					barrier.DstAccessMask.Set(EAccessFlag::TransferRead);

					CommandBuffer::SubmitPipelineBarrier
					(
						commandBuffer,
						EPipelineStageFlag::Transfer, EPipelineStageFlag::Transfer, 0,
						0, nullptr,
						0, nullptr,
						1, &barrier
					);

					Image::Blit blit{};

					blit.SrcOffsets[0].X = 0;
					blit.SrcOffsets[0].Y = 0;
					blit.SrcOffsets[0].Z = 0;

					blit.SrcOffsets[1].X = mipWidth;
					blit.SrcOffsets[1].Y = mipHeight;
					blit.SrcOffsets[1].Z = 1;

					blit.SrcSubresource.AspectMask.Set(EImageAspect::Color);
					blit.SrcSubresource.MipLevel = index - 1;
					blit.SrcSubresource.BaseArrayLayer = 0;
					blit.SrcSubresource.LayerCount = 1;

					blit.DstOffsets[0].X = 0;
					blit.DstOffsets[0].Y = 0; 
					blit.DstOffsets[0].Z = 0;

					blit.DstOffsets[1].X = mipWidth > 1 ? mipWidth / 2 : 1;
					blit.DstOffsets[1].Y = mipHeight > 1 ? mipHeight / 2 : 1; 
					blit.DstOffsets[1].Z = 1;

					blit.DstSubresource.AspectMask.Set(EImageAspect::Color);
					blit.DstSubresource.MipLevel = index;
					blit.DstSubresource.BaseArrayLayer = 0;
					blit.DstSubresource.LayerCount = 1;

					CommandBuffer::BlitImage
					(
						commandBuffer,
						_image, EImageLayout::TransferSource_Optimal,
						_image, EImageLayout::TransferDestination_Optimal,
						1, &blit,
						EFilter::Linear
					);

					barrier.OldLayout = EImageLayout::TransferSource_Optimal;
					barrier.NewLayout = EImageLayout::Shader_ReadonlyOptimal;

					barrier.SrcAccessMask.Set(EAccessFlag::TransferRead);
					barrier.DstAccessMask.Set(EAccessFlag::ShaderRead  );

					CommandBuffer::SubmitPipelineBarrier
					(
						commandBuffer,
						EPipelineStageFlag::Transfer, EPipelineStageFlag::FragementShader, 0,
						0, nullptr,
						0, nullptr,
						1, &barrier
					);

					if (mipWidth > 1) mipWidth /= 2;
					if (mipHeight > 1) mipHeight /= 2;
				}

				barrier.SubresourceRange.BaseMipLevel = _mipLevels - 1;

				barrier.OldLayout = EImageLayout::TransferDestination_Optimal;
				barrier.NewLayout = EImageLayout::Shader_ReadonlyOptimal     ;

				barrier.SrcAccessMask.Set(EAccessFlag::TransferWrite);
				barrier.DstAccessMask.Set(EAccessFlag::ShaderRead   );

				CommandBuffer::SubmitPipelineBarrier
				(
					commandBuffer,
					EPipelineStageFlag::Transfer, EPipelineStageFlag::FragementShader, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier
				);

				Vault_2::CommandBuffer::EndSingleTimeCommands(commandBuffer, CommandPool, LogicalDevice.GetHandle(), GraphicsQueue->GetHandle());
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

			// TODO: Offload
			bool CheckQueuesFor_SurfacePresentationSupport(Vault_4::PhysicalDevice _physicalDevice)
			{
				for (auto& queue : DeviceQueues)
				{
					if (Vault_2::Surface::CheckForPresentationSupport(_physicalDevice.GetHandle(), SurfaceHandle, queue.GetFamilyIndex()))
					{
						QueueIndices.PresentationSupported = queue.GetFamilyIndex();

						PresentationQueue = &queue;

						return true;
					}
				}

				return false;
			}

			bool HasStencilComponent(EFormat _format)
			{
				return _format == EFormat::D32_SFloat_S8_UInt || _format == EFormat::D24_UNormalized_S8_UInt;
			}

			bool IsDeviceSuitable(Vault_4::PhysicalDevice _physicalDevice, Surface::Handle _surface, ExtensionIdentifierList _extensionsSpecified)
			{
				auto& deviceProperties = _physicalDevice.GetProperties();
				auto& deviceFeatures   = _physicalDevice.GetFeatures  ();

				QueueIndices = FindQueueFamilies(_physicalDevice, _surface);

				bool presentationSupport = CheckQueuesFor_SurfacePresentationSupport(_physicalDevice);

				bool extensionsSupported = _physicalDevice.CheckExtensionSupport(_extensionsSpecified);

				bool swapChainAdequate = false;

				if (extensionsSupported)
				{
					SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_physicalDevice.GetHandle(), _surface);

					swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentationModes.empty();
				}

				bool&& result = 
					bool(deviceFeatures.GeometryShader) &&
					QueueIndices.Graphics.has_value() &&
					presentationSupport &&
					extensionsSupported                 &&
					swapChainAdequate                   && 
					deviceFeatures.SamplerAnisotropy      ;

				return result;
			}

			void LoadModel(String _modelPath)
			{
				tinyobj::attrib_t attrib;

				DynamicArray<tinyobj::shape_t> shapes;

				DynamicArray<tinyobj::material_t> materials;

				String warning, error;

				if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, _modelPath.c_str()))
					throw RuntimeError(warning + error);

				for (const auto& shape : shapes)
				{
					for (const auto& index : shape.mesh.indices)
					{
						Vertex vertex {};

						vertex.Position = 
						{
							attrib.vertices[3 * index.vertex_index + 0],
							attrib.vertices[3 * index.vertex_index + 1],
							attrib.vertices[3 * index.vertex_index + 2]
						};

						vertex.TextureCoordinates = 
						{
							attrib.texcoords[2 * index.texcoord_index + 0],
							1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
						};

						vertex.Color = { 1.0f, 1.0f, 1.0f };

						ModelVerticies.push_back(vertex);
						ModelIndicies.push_back(ModelIndicies.size());
					}
				}
			}

			void PickPhysicalDevice
			(
				Surface::Handle         _surface            , 
				ExtensionIdentifierList _extensionsSpecified
			)
			{
				Vault_4::PhysicalDevice::List physicalDevices;
					
				//Vault_2::AppInstance::GetAvailablePhysicalDevices(AppGPU.GetHandle(), physicalDevices);

				AppGPU.GetAvailablePhysicalDevices(physicalDevices);

				auto&& size = physicalDevices.size();

				if (size == 0)
					throw std::runtime_error("Physical device count 0. No GPUs found with Vulkan support.");

				using PhysicalDeviceList = std::vector<PhysicalDevice::Handle>;

				for (DeviceSize index = 0; index < size; index++)
				{
					//PhysDevice.AssignHandle(physicalDevices[index]);
					if (IsDeviceSuitable(physicalDevices[index], _surface, _extensionsSpecified))
					{
						PhysicalDevice = physicalDevices[index];

						MSAA_Samples = PhysicalDevice.GetMaxSampleCount_ColorAndDepth();

						break;
					}
				}

				if (PhysicalDevice.GetHandle() == PhysicalDevice::NullHandle)
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

				formatCount = Vault_2::Surface::GetNumOf_AvailableFormats(_deviceHandle, _surfaceHandle);

				if (formatCount > 0)
				{
					details.Formats.resize(formatCount);

					Vault_2::Surface::GetAvailableFormats(_deviceHandle, _surfaceHandle, details.Formats.data());
				}

				uint32 presentationModeCount;

				presentationModeCount = Vault_2::Surface::GetNumOf_SupportedPresentationModes(_deviceHandle, _surfaceHandle);

				if (presentationModeCount > 0)
				{
					details.PresentationModes.resize(presentationModeCount);

					Vault_2::Surface::GetSupportedPresentationModes(_deviceHandle, _surfaceHandle, details.PresentationModes.data());
				}

				return details;
			}

			int RateDeviceSuitability(PhysicalDevice::Handle _deviceHandle)
			{
				auto& deviceProperties = PhysicalDevice.GetProperties();

				auto& deviceFeatures = PhysicalDevice.GetFeatures();

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
				DebugMessenger::Handle& _debugMessenger
			)
			{
				if (!Vulkan_EnableValidationLayers) return;

				stack<DebugMessenger::CreateInfo> msngrCreateSpec{};

				PopulateDebugMessengerCreateInfo(msngrCreateSpec);

				EResult&& creationResult = DebugMessenger::Create(AppGPU.GetHandle(), msngrCreateSpec, nullptr, _debugMessenger);

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

			void TransitionImageLayout(Image::Handle _image, EFormat _format, EImageLayout _oldLayout, EImageLayout _newLayout, uint32 _mipMapLevels)
			{
				CommandBuffer::Handle commandBuffer = Vault_2::CommandBuffer::BeginSingleTimeCommands(LogicalDevice.GetHandle(), CommandPool);   

				Image::Memory_Barrier barrier {};

				barrier.SType = barrier.STypeEnum;

				barrier.OldLayout = _oldLayout;
				barrier.NewLayout = _newLayout;

				barrier.SrcQueueFamilyIndex = QueueFamily_Ignored;
				barrier.DstQueueFamilyIndex = QueueFamily_Ignored;

				barrier.Image = _image;

				barrier.SubresourceRange.AspectMask.Set(EImageAspect::Color);

				if (_newLayout == EImageLayout::DepthStencil_AttachmentOptimal)
				{
					barrier.SubresourceRange.AspectMask.Set(EImageAspect::Depth);

					if (HasStencilComponent(_format))
					{
						barrier.SubresourceRange.AspectMask.Add(EImageAspect::Stencil);
					}
				}
				else
				{
					barrier.SubresourceRange.AspectMask.Set(EImageAspect::Color);
				}

				barrier.SubresourceRange.BaseMipLevel   = 0            ;
				barrier.SubresourceRange.LevelCount     = _mipMapLevels;
				barrier.SubresourceRange.BaseArrayLayer = 0            ;
				barrier.SubresourceRange.LayerCount     = 1            ;

				Pipeline::StageFlags sourceStage     ;
				Pipeline::StageFlags destinationStage;

				if (_oldLayout == EImageLayout::Undefined && _newLayout == EImageLayout::TransferDestination_Optimal)
				{
					barrier.SrcAccessMask = 0;

					barrier.DstAccessMask.Set(EAccessFlag::TransferWrite);

					sourceStage     .Set(EPipelineStageFlag::TopOfPipe);
					destinationStage.Set(EPipelineStageFlag::Transfer );
				}
				else if (_oldLayout == EImageLayout::TransferDestination_Optimal && _newLayout == EImageLayout::Shader_ReadonlyOptimal)
				{
					barrier.SrcAccessMask.Set(EAccessFlag::TransferWrite);
					barrier.DstAccessMask.Set(EAccessFlag::ShaderRead   );

					sourceStage     .Set(EPipelineStageFlag::Transfer       );
					destinationStage.Set(EPipelineStageFlag::FragementShader);
				}
				else if (_oldLayout == EImageLayout::Undefined && _newLayout == EImageLayout::DepthStencil_AttachmentOptimal)
				{
					barrier.SrcAccessMask = 0;

					barrier.DstAccessMask.Set(EAccessFlag::DepthStencilAttachmentRead, EAccessFlag::DepthStencilAttachmentWrite);

					sourceStage     .Set(EPipelineStageFlag::TopOfPipe         );
					destinationStage.Set(EPipelineStageFlag::EarlyFragmentTests);
				}
				else
				{
					throw std::invalid_argument("unsupported layout transition!");
				}

				CommandBuffer::SubmitPipelineBarrier
				(
					commandBuffer,
					sourceStage, destinationStage,   // TODO
					0, 
					0, nullptr,
					0, nullptr,
					1, &barrier
				);
				
				Vault_2::CommandBuffer::EndSingleTimeCommands(commandBuffer, CommandPool, LogicalDevice.GetHandle(), GraphicsQueue->GetHandle());
			}

			void UpdateUniformBuffers(uint32 _currentImage)
			{
				static auto startTime = std::chrono::high_resolution_clock::now();

				auto currentTime = std::chrono::high_resolution_clock::now();

				float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

				UniformBufferObject ubo {};

				ubo.ModelSpace = glm::rotate(glm::mat4(1.0f), time * glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 0.5f));

				ubo.Viewport = glm::lookAt(glm::vec3(1.7f, 1.7f, 1.7f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));   // The default
				//ubo.Viewport = glm::lookAt(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); crying cat

				ubo.Projection = glm::perspective(glm::radians(45.0f), SwapChain_Extent.Width / (float) SwapChain_Extent.Height, 0.1f, 10.0f);

				ubo.Projection[1][1] *= -1;

				void* address = &ubo;

				Vault_2::Memory::WriteToGPU(LogicalDevice.GetHandle(), UniformBuffersMemory[_currentImage], 0, sizeof(ubo), 0, address);
			}

			// GPU_HAL

			void Initialize_GPUComms(RoCStr _applicationName, AppVersion _applicationVersion)
			{
				CreateApplicationInstance
				(
					_applicationName, 
					_applicationVersion, 
					&ValidationLayerIdentifiers, 
					&DebugMessenger_CreationSpec
				);

				SetupDebugMessenger(DebugMessenger_Handle);
			}

			void Cease_GPUComms()
			{
				AppGPU.Destroy();
			}

			void WaitFor_GPUIdle()
			{
				LogicalDevice::WaitUntilIdle(LogicalDevice.GetHandle());
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
					CreateSurface(_window, SurfaceHandle);

					PickPhysicalDevice(SurfaceHandle, DeviceExtensions);

					CreateLogicalDevice(PhysicalDevice.GetHandle(), SurfaceHandle, DeviceExtensions, &ValidationLayerIdentifiers);

					CreateSwapChain(_window, PhysicalDevice.GetHandle(), LogicalDevice.GetHandle(), SurfaceHandle, SwapChain, SwapChain_ImageFormat, SwapChain_Extent, SwapChain_Images);

					CreateImageViews(LogicalDevice.GetHandle(), SwapChain_Images, SwapChain_ImageFormat, SwapChain_ImageViews);

					CreateRenderPass(LogicalDevice.GetHandle(), SwapChain_ImageFormat, RenderPass);

					StaticArray<ShaderModule::Handle, 2> shaders = Create_VKTut_Shaders(LogicalDevice.GetHandle());

					CreateDescriptorSetLayout();

					CreateGraphicsPipeline(LogicalDevice.GetHandle(), SwapChain_Extent, shaders, PipelineLayout, RenderPass, GraphicsPipeline);

					ShaderModule::Destory(LogicalDevice.GetHandle(), shaders[0], nullptr);
					ShaderModule::Destory(LogicalDevice.GetHandle(), shaders[1], nullptr);

					CreateCommandPool(PhysicalDevice.GetHandle(), LogicalDevice.GetHandle(), SurfaceHandle, CommandPool);

					LoadModel(VikingRoom_ModelPath);

					CreateVertexBuffers(PhysicalDevice.GetHandle(), LogicalDevice.GetHandle(), VertexBuffer, VertexBufferMemory);

					CreateIndexBuffer();

					CreateUniformBuffers();

					CreateDescriptorPool();

					CreateColorResources();

					CreateDepthResources();

					CreateFrameBuffers(LogicalDevice.GetHandle(), RenderPass, SwapChain_Extent, SwapChain_ImageViews, SwapChain_Framebuffers);

					//CreateTextureImage("Engine/Data/Textures/DaFace.png");   // Single face thing;
					CreateTextureImage(VikingRoom_TexturePath.c_str());   // Viking model related.

					CreateTextureImageView();

					CreateTextureSampler();

					CreateDescriptorSets();

					CreateCommandBuffers(LogicalDevice.GetHandle(), GraphicsPipeline, SwapChain_Framebuffers, SwapChain_Extent, RenderPass, CommandPool, CommandBuffers);;

					CreateSyncObjects(LogicalDevice.GetHandle(), MaxFramesInFlight, SwapChain_Images, ImageAvailable_Semaphores, RenderFinished_Semaphores, InFlightFences, ImagesInFlight);
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

					LogicalDevice.WaitUntilIdle();

					CleanupSwapChain(LogicalDevice.GetHandle(), PipelineLayout, GraphicsPipeline, SwapChain, SwapChain_ImageViews, SwapChain_Framebuffers, RenderPass, CommandPool, CommandBuffers);

					CreateSwapChain(_window, PhysicalDevice.GetHandle(), LogicalDevice.GetHandle(), SurfaceHandle, SwapChain, SwapChain_ImageFormat, SwapChain_Extent, SwapChain_Images);

					CreateImageViews(LogicalDevice.GetHandle(), SwapChain_Images, SwapChain_ImageFormat, SwapChain_ImageViews);

					CreateRenderPass(LogicalDevice.GetHandle(), SwapChain_ImageFormat, RenderPass);

					StaticArray<ShaderModule::Handle, 2> shaders = Create_VKTut_Shaders(LogicalDevice.GetHandle());

					CreateGraphicsPipeline(LogicalDevice.GetHandle(), SwapChain_Extent, shaders, PipelineLayout, RenderPass, GraphicsPipeline);

					ShaderModule::Destory(LogicalDevice.GetHandle(), shaders[0], nullptr);
					ShaderModule::Destory(LogicalDevice.GetHandle(), shaders[1], nullptr);

					CreateColorResources();

					CreateDepthResources();

					CreateFrameBuffers(LogicalDevice.GetHandle(), RenderPass, SwapChain_Extent, SwapChain_ImageViews, SwapChain_Framebuffers);

					CreateUniformBuffers();

					CreateDescriptorPool();

					CreateDescriptorSets();

					CreateCommandBuffers(LogicalDevice.GetHandle(), GraphicsPipeline, SwapChain_Framebuffers, SwapChain_Extent, RenderPass, CommandPool, CommandBuffers);
				}

				void DrawFrame(ptr<Window> _window)
				{
					Fence::WaitForFences(LogicalDevice.GetHandle(), 1, &InFlightFences[CurrentFrame], EBool::True, UInt64Max);

					uint32 imageIndex;

					EResult&& result = 
						SwapChain::AcquireNextImage
						(
						    LogicalDevice.GetHandle()              , 
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
						Fence::WaitForFences(LogicalDevice.GetHandle(), 1, &ImagesInFlight[imageIndex], EBool::True, UInt64Max);

					ImagesInFlight[imageIndex] = InFlightFences[CurrentFrame];

					UpdateUniformBuffers(imageIndex);

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


					Fence::Reset(LogicalDevice.GetHandle(), &InFlightFences[CurrentFrame], 1);

					if (CommandBuffer::SubmitToQueue(GraphicsQueue->GetHandle(), 1, &submitInfo, InFlightFences[CurrentFrame]) != EResult::Success) 
						throw std::runtime_error("Failed to submit draw command buffer!");


					SwapChain::PresentationInfo presentInfo{};

					presentInfo.SType              = presentInfo.STypeEnum;
					presentInfo.WaitSemaphoreCount = 1                    ;
					presentInfo.WaitSemaphores     = signalSemaphores     ;


					SwapChain::Handle swapChains[] = { SwapChain };

					presentInfo.SwapchainCount = 1          ;
					presentInfo.Swapchains     = swapChains ;
					presentInfo.ImageIndices   = &imageIndex;

					presentInfo.Results = nullptr; // Optional


					result = SwapChain::QueuePresentation(PresentationQueue->GetHandle(), presentInfo);

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
						LogicalDevice.GetHandle()         , 
						PipelineLayout        , 
						GraphicsPipeline      , 
						SwapChain             , 
						SwapChain_ImageViews  , 
						SwapChain_Framebuffers, 
						RenderPass            , 
						CommandPool           , 
						CommandBuffers
					);

					TextureSampler.Destroy();

					//ImageView::Destroy(LogicalDevice.GetHandle(), TextureImageView, nullptr);

					TextureImageView.Destroy();

					//Image::Destroy(LogicalDevice.GetHandle(), TextureImage, nullptr);

					TextureImage.Destroy();

					//Memory::Free(LogicalDevice.GetHandle(), TextureImageMemory, nullptr);

					TextureImageMemory.Free();

					Pipeline::Layout::DescriptorSet::Destroy(LogicalDevice.GetHandle(), DescriptorSetLayout, nullptr);

					//Buffer::Destroy(LogicalDevice.GetHandle(), IndexBuffer, nullptr);
					IndexBuffer.Destroy();

					//Memory::Free(LogicalDevice.GetHandle(), IndexBufferMemory, nullptr);

					IndexBufferMemory.Free();

					//Buffer::Destroy(LogicalDevice.GetHandle(), VertexBuffer, nullptr);
					VertexBuffer.Destroy();

					//Memory::Free(LogicalDevice.GetHandle(), VertexBufferMemory, nullptr);

					VertexBufferMemory.Free();

					for (DataSize index = 0; index < MaxFramesInFlight; index++) 
					{
						Semaphore::Destroy(LogicalDevice.GetHandle(), RenderFinished_Semaphores[index], nullptr);   // TODO: Wrap
						Semaphore::Destroy(LogicalDevice.GetHandle(), ImageAvailable_Semaphores[index], nullptr);   // TODO: Wrap
						Fence::Destroy    (LogicalDevice.GetHandle(), InFlightFences           [index], nullptr);   // TODO: Wrap
					}

					CommandPool::Destroy(LogicalDevice.GetHandle(), CommandPool, nullptr);   // TODO: Wrap

					LogicalDevice.Destroy();

					if (Vulkan_EnableValidationLayers) DebugMessenger::Destroy(AppGPU.GetHandle(), DebugMessenger_Handle, nullptr);

					Surface::Destroy(AppGPU.GetHandle(), SurfaceHandle, nullptr);
				}

				void ReinitializeRenderer(ptr<OSAL::Window> _window)
				{
					RecreateSwapChain(_window);
				}
			}
		}
	}

#endif
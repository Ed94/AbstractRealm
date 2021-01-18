// Parent Header
#include "GPU_Vulkan.hpp"



// TUtorial: This needs to be removed from GPU_Vulkan
// This can be done by making the design of the GPU HAL agnostic to what the tutorial requires.
#include "_TutorialRelated.hpp"

// Engine
#include "HAL_Backend.hpp"
#include "GPUVK_Comms.hpp"
#include "GPUVK_PayloadDeck.hpp"
#include "GPUVK_Rendering.hpp"


#include "Dev/Console.hpp"

#if VulkanAPI_Interface == VaultedVulkan_Interface

	namespace HAL::GPU
	{
		namespace Vulkan
		{
			StaticData()

				// Command

				CommandPool                         SingleTimeCommandPool;
				DynamicArray<CommandPool          > CommandPools_Old     ;   
				DynamicArray<CommandBuffer        > CommandBuffers_Old   ;
				DynamicArray<CommandBuffer::Handle> CommandBufferHandles ;

				// Model V1 Rendering Pipeline

				V3::GraphicsPipeline GraphicsPipeline_Old;

				V3::RenderPass RenderPass_Old;   

				Pipeline::Cache PipelineCache_Old;   // Implement.

				Pipeline::Layout::DescriptorSet DescriptorSetLayout_Old;
				Pipeline::Layout                PipelineLayout;

				V3::DescriptorPool DescriptorPool;

				DynamicArray<DescriptorSet        > DescriptorSets      ;
				DynamicArray<DescriptorSet::Handle> DescriptorSetHandles;

				ui32         MipMapLevels                   ;
				ESampleCount MSAA_Samples = ESampleCount::_1;

				// Basic Rendering Synch (Upgrade this later)			

				DynamicArray<Fence>      InFlightFences;
				DynamicArray<ptr<Fence>> ImagesInFlight;

				DynamicArray<Semaphore> ImageAvailable_Semaphores;
				DynamicArray<Semaphore> RenderFinished_Semaphores;

				// Surface Context

				ptr<Surface> Surface_Old;

				// Swapchain for surface

				Swapchain* SwapChain_Old;

				DynamicArray<Framebuffer> SwapChain_Framebuffers;

				ui32 Swap_MinImageCount;

				// Related to Rendering State Tracking

				bool     FramebufferResized = false;
				WordSize CurrentFrame       = 0;
				si32     MaxFramesInFlight  = 2;
				
				// Not Sure

				Image     DepthImage      ;
				Memory    DepthImageMemory;
				ImageView DepthImageView  ;

				Image     ColorImage      ;
				Memory    ColorImageMemory;
				ImageView ColorImageView  ;

				RawRenderContext RenderContext_Default;   // Should this still be used?



			void CreateImage
			(
				ui32                  _width,
				ui32                  _height,
				ui32                  _mipLevels,
				ESampleCount          _numSamples,
				EFormat               _format,
				EImageTiling          _tiling,
				Image::UsageFlags     _usage,
				Memory::PropertyFlags _properties,
				Image&                _image,
				Memory&               _imageMemory
			);

			ImageView CreateImageView(Image& _image, EFormat _format, Image::AspectFlags _aspectFlags, ui32 /*_miplevels*/);

			ptr<Surface>       TriangleDemo_Surface;
			ptr<Swapchain>     TriangleDemo_Swap;
			ptr<RenderContext> TriangleDemo_Context;

			BasicShader ModelWTxtur_Shader;

			BasicShader TriangleDemo_Shader;

			ptr<ARenderable> TriangleDemo_Renderable;
			ptr<ARenderable> ModelWTexur_Renderable;

			using ImageBytes = stbi_uc;

			ImageBytes* ModelWTxtur_TxtImage;

			int Model_TxtWidth, Model_TxtHeight, Model_TxtChannels;

			void AddTestCallback();

			void Start_TriangleDemo(ptr<OSAL::Window> _window)
			{
				TriangleDemo_Surface = getAddress(Rendering::Request_Surface(_window));

				Surface::Format format;

				format.Format     = EFormat::B8_G8_R8_A8_UNormalized;
				format.ColorSpace = EColorSpace::SRGB_NonLinear;

				TriangleDemo_Swap = getAddress(Rendering::Request_SwapChain(*TriangleDemo_Surface, format));

				TriangleDemo_Context = getAddress(Rendering::Request_RenderContext(*TriangleDemo_Swap));

				TriangleDemo_Shader.Create
				(
					String(Renderer::Shader::Paths::TriangleShader) + "TriangleShader.vert",
					String(Renderer::Shader::Paths::TriangleShader) + "TriangleShader.frag"
				);

				ModelWTxtur_Shader.Create
				(
					String(Renderer::Shader::Paths::VKTut) + "VertexShaderV5.vert",
					String(Renderer::Shader::Paths::VKTut) + "FragmentShaderV5.frag"
				);

				TriangleDemo_Renderable = GPU_Resources::Request_Renderable(TriangleVerticies, getAddress(TriangleDemo_Shader));

				LoadModel(VikingRoom_ModelPath);

				ModelWTxtur_TxtImage = stbi_load(VikingRoom_TexturePath.c_str(), &Model_TxtWidth, &Model_TxtHeight, &Model_TxtChannels, STBI_rgb_alpha);

				ModelWTexur_Renderable = GPU_Resources::Request_Renderable
				(
					ModelVerticies, 
					ModelIndicies, 
					ModelWTxtur_TxtImage, Model_TxtWidth, Model_TxtHeight,
					getAddress(ModelWTxtur_Shader)
				);

				TriangleDemo_Context->AddRenderable(ModelWTexur_Renderable);

				//TriangleDemo_Context->AddRenderable(TriangleDemo_Renderable);

				//AddTestCallback();
			}

			void Render()
			{
				Rendering::Update();
			}

			void Present()
			{
				Rendering::Present();
			}

			void Stop_TriangleDemo()
			{
				CLog("Stopping Clear Color Demo...");




				stbi_image_free(ModelWTxtur_TxtImage);

				Rendering::Retire_RenderContext(TriangleDemo_Context);
				Rendering::Retire_SwapChain    (TriangleDemo_Swap   );
				Rendering::Retire_Surface      (TriangleDemo_Surface);
			}

		#pragma region Staying

			ui32 GetMinimumFramebufferCount()
			{
				return 2;
			}

			ui32 GetNumberOfFramebuffers()
			{
				return SCast<ui32>(SwapChain_Old->GetImages().size());
			}

			const CommandBuffer& RequestSingleTimeBuffer()
			{
				return SingleTimeCommandPool.BeginSingleTimeCommands();
			}

			void EndSingleTimeBuffer(const CommandBuffer& _buffer)
			{
				SingleTimeCommandPool.EndSingleTimeCommands(_buffer);
			}

			EResult RequestDescriptorPool(V3::DescriptorPool& _pool, V3::DescriptorPool::CreateInfo _info)
			{
				EResult returnCode =  _pool.Create(GPU_Comms::GetEngagedDevice(), _info);

				return returnCode;
			}

		#pragma endregion Staying

			// Functions

			void CleanupSwapChain()
			{
					ColorImageView  .Destroy(); 
					ColorImage      .Destroy(); 
					ColorImageMemory.Free   (); 

					DepthImageView  .Destroy(); 
					DepthImage      .Destroy(); 
					DepthImageMemory.Free   (); 

					for (WordSize index = 0; index < SwapChain_Framebuffers.size(); index++)
					{
						SwapChain_Framebuffers[index].Destroy(); 
					}

					GraphicsPipeline_Old.Destroy(); 
					PipelineLayout      .Destroy(); 
					RenderPass_Old      .Destroy(); 

					for (WordSize index = 0; index < SwapChain_Old->GetImageViews().size(); index++)
					{
						UniformBuffers      [index].Destroy(); 
						UniformBuffersMemory[index].Free   (); 
					}

					DescriptorPool.Destroy(); 
			}

			void CopyBufferToImage(Buffer& _buffer, Image& _image, ui32 _width, ui32 _height)
			{
				CommandBuffer commandBuffer = SingleTimeCommandPool.BeginSingleTimeCommands();

				CommandBuffer::BufferImageRegion region {};

				region.BufferOffset    = 0;
				region.BufferRowLength = 0;

				region.ImageSubresource.AspectMask.Set(EImageAspect::Color);

				region.ImageSubresource.MipLevel       = 0;
				region.ImageSubresource.BaseArrayLayer = 0;
				region.ImageSubresource.LayerCount     = 1;

				region.ImageOffset.X = 0; 
				region.ImageOffset.Y = 0; 
				region.ImageOffset.Z = 0;

				region.ImageExtent.Width  = _width ;
				region.ImageExtent.Height = _height;
				region.ImageExtent.Depth  = 1      ;

				commandBuffer.CopyBufferToImage(_buffer, _image, EImageLayout::TransferDestination_Optimal, 1, &region);
				
				SingleTimeCommandPool.EndSingleTimeCommands(commandBuffer);
			}

			DynamicArray<RenderCallback> RenderCallbacks;

			void AddRenderCallback(RenderCallback _renderCallback)
			{
				RenderCallbacks.push_back(_renderCallback);
			}

			void RecordToBuffers(int index)
			{
				CommandBuffer::BeginInfo beginInfo;

				beginInfo.Flags           = 0                  ;   // Optional
				beginInfo.InheritanceInfo = nullptr            ;   // Optional

				if (CommandBuffers_Old[index].BeginRecord(beginInfo) != EResult::Success) 
					throw std::runtime_error("Failed to begin recording command buffer!");

				RenderPass::BeginInfo renderPassInfo{};

				renderPassInfo.RenderPass  = RenderPass_Old               ;
				renderPassInfo.Framebuffer = SwapChain_Framebuffers[index];

				renderPassInfo.RenderArea.Offset.X = 0;
				renderPassInfo.RenderArea.Offset.Y = 0;

				renderPassInfo.RenderArea.Extent = SwapChain_Old->GetExtent();

				StaticArray<ClearValue, 2> clearValues {}; 

				clearValues[0].Color        = { 0.0f, 0.0f, 0.0f, 0.1f };
				clearValues[1].DepthStencil = { 1.0f, 0                };

				renderPassInfo.ClearValueCount = SCast<ui32>(clearValues.size());
				renderPassInfo.ClearValues     = clearValues.data()               ;

				CommandBuffers_Old[index].BeginRenderPass(renderPassInfo, ESubpassContents::Inline);

				CommandBuffers_Old[index].BindPipeline(EPipelineBindPoint::Graphics, GraphicsPipeline_Old);

				Buffer::Handle vertexBuffers = VertexBuffer_Old;

				DeviceSize offsets = 0;

				CommandBuffers_Old[index].BindVertexBuffers(0, 1, &vertexBuffers, &offsets);

				CommandBuffers_Old[index].BindIndexBuffer(IndexBuffer_Old, 0, EIndexType::uInt32);

				CommandBuffers_Old[index].BindDescriptorSets
				(
					EPipelineBindPoint::Graphics,
					PipelineLayout,
					0,
					1,
					DescriptorSets[index]
				);

				CommandBuffers_Old[index].DrawIndexed
				(
					SCast<ui32>(ModelIndicies.size()),
					1,
					0,
					0,
					0
				);

				for (auto renderCallback : RenderCallbacks)
				{
					renderCallback(CommandBuffers_Old[index], index);
				}
				
				CommandBuffers_Old[index].EndRenderPass();

				if (CommandBuffers_Old[index].EndRecord() != EResult::Success) 
					throw std::runtime_error("Failed to record command buffer!");
			}

			void RecordToBuffers_LMAO
			(
				int index, 
				const CommandBuffer& _buffer, 
				Framebuffer::Handle _frameBuffer, 
				ptr<const Swapchain> _swap,
				RenderPass::BeginInfo _beginInfo
			)
			{
				//CommandBuffer::BeginInfo beginInfo;

				//beginInfo.Flags           = 0                  ;   // Optional
				//beginInfo.InheritanceInfo = nullptr            ;   // Optional

				//if (_buffer.BeginRecord(beginInfo) != EResult::Success) 
				//	throw std::runtime_error("Failed to begin recording command buffer!");

				/*RenderPass::BeginInfo renderPassInfo{};

				renderPassInfo.RenderPass  = RenderPass_Old               ;
				renderPassInfo.Framebuffer = _frameBuffer;

				renderPassInfo.RenderArea.Offset.X = 0;
				renderPassInfo.RenderArea.Offset.Y = 0;

				renderPassInfo.RenderArea.Extent = _swap->GetExtent();

				StaticArray<ClearValue, 2> clearValues {};

				clearValues[0].Color        = { 0.0f, 0.0f, 0.0f, 0.1f };
				clearValues[1].DepthStencil = { 1.0f, 0                };

				renderPassInfo.ClearValueCount = SCast<ui32>(clearValues.size());
				renderPassInfo.ClearValues     = clearValues.data()               ;

				_buffer.BeginRenderPass(renderPassInfo, ESubpassContents::Inline);*/

				_buffer.BindPipeline(EPipelineBindPoint::Graphics, GraphicsPipeline_Old);

				Buffer::Handle vertexBuffers = VertexBuffer_Old;

				DeviceSize offsets = 0;

				_buffer.BindVertexBuffers(0, 1, &vertexBuffers, &offsets);

				_buffer.BindIndexBuffer(IndexBuffer_Old, 0, EIndexType::uInt32);

				_buffer.BindDescriptorSets
				(
					EPipelineBindPoint::Graphics,
					PipelineLayout,
					0,
					1,
					DescriptorSets[index]
				);

				_buffer.DrawIndexed
				(
					SCast<ui32>(ModelIndicies.size()),
					1,
					0,
					0,
					0
				);

				for (auto renderCallback : RenderCallbacks)
				{
					renderCallback(_buffer, index);
				}

				//_buffer.EndRenderPass();

				/*if (_buffer.EndRecord() != EResult::Success)
					throw std::runtime_error("Failed to record command buffer!");*/
			}

			void AddTestCallback()
			{
				SetTestCallback(RecordToBuffers_LMAO);
			}

			void CreateColorResources()
			{
				EFormat colorFormat = SwapChain_Old->GetFormat();

				auto swapExtent = SwapChain_Old->GetExtent();

				CreateImage
				(
					swapExtent.Width, 
					swapExtent.Height, 
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

			void CreateFrameObjects()
			{
				CommandPool::CreateInfo poolInfo;

				poolInfo.QueueFamilyIndex = GPU_Comms::GetEngagedDevice().GetGraphicsQueue().GetFamilyIndex();
				poolInfo.Flags            = CommandPool::CreateFlgas()                                       ;   // Optional

				SingleTimeCommandPool.Create(GPU_Comms::GetEngagedDevice(), poolInfo);

				CommandPools_Old    .resize(SwapChain_Old->GetImages().size());
				CommandBuffers_Old  .resize(SwapChain_Old->GetImages().size());
				CommandBufferHandles.resize(SwapChain_Old->GetImages().size());

				for (DeviceSize index = 0; index < SwapChain_Old->GetImages().size(); index++)
				{
					if (CommandPools_Old[index].Create(GPU_Comms::GetEngagedDevice(), poolInfo) != EResult::Success)
					{
						throw std::runtime_error("failed to create command pool!");
					}

					CommandPool::AllocateInfo info;

					info.BufferCount = 1;
					info.Pool        = CommandPools_Old[index];
					info.Level       = ECommandBufferLevel::Primary;

					if (CommandPools_Old[index].Allocate(info,  CommandBuffers_Old[index]) != EResult::Success)
						throw std::runtime_error("failed to allocate command buffers!");

					// Allocations of buffers here are not tracked. They are freed automatically by the command pool on its destruction.

					CommandBufferHandles[index] = CommandBuffers_Old[index];
				}
			}

			void CreateDepthResources()
			{
				EFormat depthFormat = FindDepthFormat();

				auto swapExtent = SwapChain_Old->GetExtent();

				CreateImage
				(
					swapExtent.Width, 
					swapExtent.Height, 
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

				DepthImage.TransitionLayout(EImageLayout::Undefined, EImageLayout::DepthStencil_AttachmentOptimal);
			}

			void CreateDescriptorPool()
			{
				StaticArray<DescriptorPool::Size, 2> poolSizes {};

				poolSizes[0].Type = EDescriptorType::UniformBuffer;
				poolSizes[0].Count = SCast<ui32>(SwapChain_Old->GetImages().size());

				poolSizes[1].Type = EDescriptorType::Sampler;
				poolSizes[1].Count = SCast<ui32>(SwapChain_Old->GetImages().size());

				DescriptorPool::CreateInfo poolInfo {};

				poolInfo.PoolSizeCount = SCast<ui32>(poolSizes.size());
				poolInfo.PoolSizes     = poolSizes.data()               ;

				poolInfo.MaxSets = SCast<ui32>(SwapChain_Old->GetImages().size());

				if (DescriptorPool.Create(GPU_Comms::GetEngagedDevice(), poolInfo) != EResult::Success)
					throw RuntimeError("Failed to create descriptor pool!");
			}

			void CreateDescriptorSets()
			{
				DynamicArray<Pipeline::Layout::DescriptorSet::Handle> layouts(SwapChain_Old->GetImages().size(), Pipeline::Layout::DescriptorSet::Handle(DescriptorSetLayout_Old));

				DescriptorPool::AllocateInfo allocInfo{};

				allocInfo.DescriptorPool     = DescriptorPool;
				allocInfo.DescriptorSetCount = SCast<ui32>(SwapChain_Old->GetImages().size());
				allocInfo.SetLayouts         = layouts.data();

				if (DescriptorPool.Allocate(allocInfo, DescriptorSets, DescriptorSetHandles) != EResult::Success)
					throw std::runtime_error("failed to allocate descriptor sets!");

				// Allocations of sets here are not tracked. They are freed automatically by the descriptor pool on its destruction.

				for (WordSize index = 0; index < SwapChain_Old->GetImages().size(); index++)
				{
					DescriptorSet::BufferInfo bufferInfo{};

					bufferInfo.Buffer = UniformBuffers[index]      ;
					bufferInfo.Offset = 0                          ;
					bufferInfo.Range  = sizeof(UniformBufferObject);


					DescriptorSet::ImageInfo imageInfo{};

					imageInfo.ImageLayout = EImageLayout::Shader_ReadonlyOptimal;
					imageInfo.ImageView   = TextureImageView        ;
					imageInfo.Sampler     = TextureSampler;


					StaticArray<DescriptorSet::Write, 2> descriptorWrites;

					descriptorWrites[0].DstSet          = DescriptorSets[index];
					descriptorWrites[0].DstBinding      = 0                    ;
					descriptorWrites[0].DstArrayElement = 0                    ;

					descriptorWrites[0].DescriptorType  = EDescriptorType::UniformBuffer;
					descriptorWrites[0].DescriptorCount = 1                             ;

					descriptorWrites[0].BufferInfo      = &bufferInfo;
					descriptorWrites[0].ImageInfo       = nullptr    ; // Optional
					descriptorWrites[0].TexelBufferView = nullptr    ; // Optional

					descriptorWrites[1].DstSet          = DescriptorSets[index]          ;
					descriptorWrites[1].DstBinding      = 1                              ;
					descriptorWrites[1].DstArrayElement = 0                              ;

					descriptorWrites[1].DescriptorType  = EDescriptorType::CombinedImageSampler;
					descriptorWrites[1].DescriptorCount = 1                                    ;

					descriptorWrites[1].BufferInfo      = nullptr   ;
					descriptorWrites[1].ImageInfo       = &imageInfo; // Optional
					descriptorWrites[1].TexelBufferView = nullptr   ; // Optional

					DescriptorSets[0].Update(SCast<ui32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);	
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
				
				Pipeline::Layout::DescriptorSet::CreateInfo layoutInfo;

				layoutInfo.BindingCount = SCast<ui32>(bindings.size());
				layoutInfo.Bindings = bindings.data();

				//DescriptorSetLayout_Old.Assign(GPU_Comms::GetEngagedDevice(), layoutInfo);

				if (DescriptorSetLayout_Old.Create(GPU_Comms::GetEngagedDevice(), layoutInfo) != EResult::Success)
					throw RuntimeError("Failed to create descriptor set layout!");
			}

			void CreateFrameBuffers()
			{
				SwapChain_Framebuffers.resize(SwapChain_Old->GetImages().size());

				for (WordSize index = 0; index < SwapChain_Old->GetImages().size(); index++) 
				{
					StaticArray<ImageView::Handle, 2> attachments = 
					{
						SwapChain_Old->GetImageViews()[index],
						DepthImageView
						//ColorImageView.GetHandle(), // Sampler image.
					};

					Framebuffer::CreateInfo framebufferInfo {};

					auto swapExtent = SwapChain_Old->GetExtent();

					framebufferInfo.RenderPass      = RenderPass_Old       ;
					framebufferInfo.AttachmentCount = SCast<ui32>(attachments.size());
					framebufferInfo.Attachments     = attachments.data()               ;
					framebufferInfo.Width           = swapExtent.Width           ;
					framebufferInfo.Height          = swapExtent.Height          ;
					framebufferInfo.Layers          = 1                                ;

					if (SwapChain_Framebuffers[index].Create(GPU_Comms::GetEngagedDevice(), framebufferInfo) != EResult::Success) 
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
			void CreateGraphicsPipeline(StaticArray<V3::ShaderModule, 2>& _shaders)
			{
				Pipeline::ShaderStage::CreateInfo vertexShaderStage_CreeationSpec{};
				Pipeline::ShaderStage::CreateInfo fragShaderStage_CreationSpec{};

				vertexShaderStage_CreeationSpec.Stage = EShaderStageFlag::Vertex;
				vertexShaderStage_CreeationSpec.Module = _shaders[0]            ;
				vertexShaderStage_CreeationSpec.Name   = "main"     ;

				fragShaderStage_CreationSpec.Stage = EShaderStageFlag::Fragment;
				fragShaderStage_CreationSpec.Module = _shaders[1];
				fragShaderStage_CreationSpec.Name   = "main"     ;

				Pipeline::ShaderStage::CreateInfo shaderStages[] = { vertexShaderStage_CreeationSpec, fragShaderStage_CreationSpec };

				// Fixed Function

				Pipeline::VertexInputState::CreateInfo vertexInputState_CreationSpec{};

				// TODO: Need to figure out how the GPU is going to be fed shader/vertex information etc.

				// VKTut_V1/2

				auto binding = Vertex_WTexture::GetBindingDescription();
				auto attributes = Vertex_WTexture::GetAttributeDescriptions();

				vertexInputState_CreationSpec.BindingDescriptionCount = 1;
				vertexInputState_CreationSpec.AttributeDescriptionCount = SCast<ui32>(attributes.size());

				vertexInputState_CreationSpec.BindingDescriptions = &binding;
				vertexInputState_CreationSpec.AttributeDescriptions = attributes.data();

				Pipeline::InputAssemblyState::CreateInfo inputAssembly_CreationSpec{};

				inputAssembly_CreationSpec.Topology               = EPrimitiveTopology::TriangleList;
				inputAssembly_CreationSpec.PrimitiveRestartEnable = false                           ;

				Viewport viewport{};

				auto swapExtent = SwapChain_Old->GetExtent();

				viewport.X        = 0.0f                    ;
				viewport.Y        = 0.0f                    ;
				viewport.Width    = float(swapExtent.Width) ;
				viewport.Height   = float(swapExtent.Height);
				viewport.MinDepth = 0.0f                    ;
				viewport.MaxDepth = 1.0f                    ;

				Rect2D scissor{};

				scissor.Offset.X = 0;
				scissor.Offset.Y = 0;

				scissor.Extent = swapExtent;

				Pipeline::ViewportState::CreateInfo viewportState_CreationSpec{};

				viewportState_CreationSpec.ViewportCount = 1        ;
				viewportState_CreationSpec.Viewports     = &viewport;
				viewportState_CreationSpec.ScissorCount  = 1        ;
				viewportState_CreationSpec.Scissors      = &scissor ;

				Pipeline::RasterizationState::CreateInfo rasterizer{};

				rasterizer.EnableDepthClamp        = EBool::False      ;
				rasterizer.EnableRasterizerDiscard = EBool::False      ;
				rasterizer.PolygonMode             = EPolygonMode::Fill;
				rasterizer.LineWidth               = 1.0f              ;

				rasterizer.CullMode.Set(ECullModeFlag::Back);

				rasterizer.FrontFace               = EFrontFace::CounterClockwise;
				rasterizer.EnableDepthBias         = EBool::False                ;
				rasterizer.DepthBiasConstantFactor = 0.0f                        ;
				rasterizer.DepthBiasClamp          = 0.0f                        ;
				rasterizer.DepthBiasSlopeFactor    = 0.0f                        ;

				Pipeline::MultiSampleState::CreateInfo multisampling_CreationSpec;

				multisampling_CreationSpec.EnableSampleShading   = EBool::False;
				multisampling_CreationSpec.RasterizationSamples  = MSAA_Samples;
				multisampling_CreationSpec.MinSampleShading      = 1.0f        ;
				multisampling_CreationSpec.SampleMask            = nullptr     ;
				multisampling_CreationSpec.EnableAlphaToCoverage = EBool::False;
				multisampling_CreationSpec.EnableAlphaToOne      = EBool::False;

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
				colorBlend_Attachment.ColorOperation              = EBlendOperation::Add;
				colorBlend_Attachment.Source_AlphaBlendFactor      = EBlendFactor::One   ;
				colorBlend_Attachment.Destination_AlphaBlendFactor = EBlendFactor::Zero  ;
				colorBlend_Attachment.AlphaOperation               = EBlendOperation::Add;

				Pipeline::ColorBlendState::CreateInfo colorBlending_CreationSpec{};

				colorBlending_CreationSpec.EnableLogicOperations = EBool::False          ;
				colorBlending_CreationSpec.LogicOperation        = ELogicOperation::Copy ;
				colorBlending_CreationSpec.AttachmentCount       = 1                     ;
				colorBlending_CreationSpec.Attachments           = &colorBlend_Attachment;
				colorBlending_CreationSpec.BlendConstants[0]     = 0.0f                  ;
				colorBlending_CreationSpec.BlendConstants[1]     = 0.0f                  ;
				colorBlending_CreationSpec.BlendConstants[2]     = 0.0f                  ;
				colorBlending_CreationSpec.BlendConstants[3]     = 0.0f                  ;

				Pipeline::DepthStencilState::CreateInfo depthStencil_Info {};

				depthStencil_Info.DepthTestEnable  = EBool::True;
				depthStencil_Info.DepthWriteEnable = EBool::True;
				
				depthStencil_Info.DepthCompareOp = ECompareOperation::Less;
				
				depthStencil_Info.DepthBoundsTestEnable = EBool::False;

				depthStencil_Info.MinDepthBounds = 0.0f;
				depthStencil_Info.MaxDepthBounds = 1.0f;

				depthStencil_Info.StencilTestEnable = EBool::False;

				StencilOperationState nullStencil {};

				depthStencil_Info.Front = nullStencil;   // Optional
				depthStencil_Info.Back  = nullStencil;   // Optional

				EDynamicState States[] =
				{
					EDynamicState::Viewport ,
					EDynamicState::LineWidth
				};	

				Pipeline::DynamicState::CreateInfo dynamicState {};

				dynamicState.StateCount = 2     ;
				dynamicState.States     = States;

				Pipeline::Layout::CreateInfo pipelineLayout_CreationSpec {};

				pipelineLayout_CreationSpec.SetLayoutCount         = 1                  ;
				pipelineLayout_CreationSpec.SetLayouts             = DescriptorSetLayout_Old;
				pipelineLayout_CreationSpec.PushConstantRangeCount = 0                  ;
				pipelineLayout_CreationSpec.PushConstantRanges     = nullptr            ;

				EResult piplineLayout_CreationResult = 
					PipelineLayout.Create(GPU_Comms::GetEngagedDevice(), pipelineLayout_CreationSpec);

				if (piplineLayout_CreationResult != EResult::Success)
				{
					throw std::runtime_error("Failed to create pipeline layout!");
				}

				Pipeline::Graphics::CreateInfo pipelineInfo {};

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
				pipelineInfo.TessellationState = nullptr;

				pipelineInfo.Layout = PipelineLayout;

				pipelineInfo.RenderPass = RenderPass_Old;
				pipelineInfo.Subpass    = 0          ;

				pipelineInfo.BasePipelineHandle = VK_NULL_HANDLE;   // Optional
				pipelineInfo.BasePipelineIndex  = -1            ;   // Optional

				EResult returnCode = GraphicsPipeline_Old.Create(GPU_Comms::GetEngagedDevice(), pipelineInfo);

				if (returnCode != EResult::Success) 
					throw std::runtime_error("Failed to create graphics pipeline!");
			}	

			void CreateImage
			(
				ui32                _width      , 
				ui32                _height     , 
				ui32                _mipLevels  ,
				ESampleCount          _numSamples ,
				EFormat               _format     , 
				EImageTiling          _tiling     , 
				Image::UsageFlags     _usage      , 
				Memory::PropertyFlags _properties , 
				Image&        _image      , 
				Memory&       _imageMemory
			)
			{
				Image::CreateInfo imageInfo {};

				imageInfo.ImageType     = EImageType::_2D       ;
				imageInfo.Extent.Width  = SCast<ui32>(_width );
				imageInfo.Extent.Height = SCast<ui32>(_height);
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

				if (_image.Create(GPU_Comms::GetEngagedDevice(), imageInfo) != EResult::Success)
					throw RuntimeError("Failed to create image!");

				Memory::AllocateInfo allocationInfo {};

				auto& gpu = GPU_Comms::GetEngagedDevice().GetPhysicalDevice();

				allocationInfo.AllocationSize = _image.GetMemoryRequirements().Size;
				allocationInfo.MemoryTypeIndex = gpu.FindMemoryType(_image.GetMemoryRequirements().MemoryTypeBits, _properties);

				if (_imageMemory.Allocate(_image.GetMemoryRequirements(), _properties) != EResult::Success)
					throw RuntimeError("Failed to allocate image memory!");

				_image.BindMemory(_imageMemory, 0);
			}

			ImageView CreateImageView(Image& _image, EFormat _format, Image::AspectFlags _aspectFlags, ui32 /*_miplevels*/)
			{
				ImageView::CreateInfo viewInfo;

				viewInfo.Image    = _image                   ;
				viewInfo.ViewType = ImageView::EViewType::_2D;
				viewInfo.Format   = _format                  ;

				viewInfo.Components.R = EComponentSwizzle::Identitity;
				viewInfo.Components.G = EComponentSwizzle::Identitity;
				viewInfo.Components.B = EComponentSwizzle::Identitity;
				viewInfo.Components.A = EComponentSwizzle::Identitity;

				viewInfo.SubresourceRange.AspectMask     = _aspectFlags;
				viewInfo.SubresourceRange.BaseMipLevel   = 0           ;
				viewInfo.SubresourceRange.LevelCount     = 1 ;  //_miplevels  ;
				viewInfo.SubresourceRange.BaseArrayLayer = 0           ;
				viewInfo.SubresourceRange.LayerCount     = 1           ;

				ImageView result;

				if (result.Create(GPU_Comms::GetEngagedDevice(), viewInfo) != EResult::Success )
					throw RuntimeError("Failed to create texture image view!");

				return result;
			}

			void CreateIndexBuffer()
			{
				DeviceSize bufferSize = sizeof(ModelIndicies[0]) * ModelIndicies.size();

				Buffer             stagingBuffer      ;
				Buffer::CreateInfo stagingBufferInfo  ;
				Memory             stagingBufferMemory;

				stagingBufferInfo.SharingMode = ESharingMode::Exclusive;
				stagingBufferInfo.Size        = bufferSize            ; 

				stagingBufferInfo.Usage.Set(EBufferUsage::TransferSource);

				stagingBuffer.Create(GPU_Comms::GetEngagedDevice(), stagingBufferInfo);

				Memory::AllocateInfo allocInfo;

				auto& memReq = stagingBuffer.GetMemoryRequirements();

				allocInfo.AllocationSize = memReq.Size;

				allocInfo.MemoryTypeIndex =
					stagingBuffer.GetDevice().GetPhysicalDevice().FindMemoryType
					(memReq.MemoryTypeBits, Memory::PropertyFlags(EMemoryPropertyFlag::HostVisible, EMemoryPropertyFlag::HostCoherent));

				stagingBufferMemory.Allocate(GPU_Comms::GetEngagedDevice(), allocInfo);

				stagingBuffer.BindMemory(stagingBufferMemory, Memory::ZeroOffset);

				// Hard coded vertex data...

				RoVoidPtr address = ModelIndicies.data();

				stagingBufferMemory.WriteToGPU(0, bufferSize, Memory::MapFlags(), address);

				Buffer::CreateInfo indexBufferInfo {};

				indexBufferInfo.SharingMode = ESharingMode::Exclusive;
				indexBufferInfo.Size        = bufferSize;

				indexBufferInfo.Usage.Set(EBufferUsage::TransferDestination, EBufferUsage::IndexBuffer);

				IndexBuffer_Old.Create(GPU_Comms::GetEngagedDevice(), indexBufferInfo);

				auto& memReqIndex = IndexBuffer_Old.GetMemoryRequirements();

				allocInfo.AllocationSize = memReqIndex.Size;

				allocInfo.MemoryTypeIndex = 
					IndexBuffer_Old.GetDevice().GetPhysicalDevice().FindMemoryType
					(memReqIndex.MemoryTypeBits, Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal));

				IndexBufferMemory.Allocate(GPU_Comms::GetEngagedDevice(),allocInfo);

				IndexBuffer_Old.BindMemory(IndexBufferMemory, Memory::ZeroOffset);

				Buffer::CopyInfo copyInfo {}; copyInfo.DestinationOffset = 0; copyInfo.SourceOffset = 0; copyInfo.Size = bufferSize;

				SingleTimeCommandPool.CopyBuffer(stagingBuffer, IndexBuffer_Old, copyInfo, GPU_Comms::GetEngagedDevice().GetGraphicsQueue());

				stagingBuffer.Destroy();
				stagingBufferMemory.Free();
				// TODO: WHY was it set to not free...
			}

			void CreateRenderPass()
			{
				RenderPass::AttachmentDescription colorAttachment {};

				colorAttachment.Format  = SwapChain_Old->GetFormat();
				colorAttachment.Samples = MSAA_Samples        ;

				colorAttachment.LoadOp  = EAttachmentLoadOperation ::Clear;
				colorAttachment.StoreOp = EAttachmentStoreOperation::Store;

				colorAttachment.StencilLoadOp  = EAttachmentLoadOperation ::DontCare;
				colorAttachment.StencilStoreOp = EAttachmentStoreOperation::DontCare;

				colorAttachment.InitialLayout = EImageLayout::Undefined        ;
				//colorAttachment.FinalLayout   = EImageLayout::Color_AttachmentOptimal;
				colorAttachment.FinalLayout   = EImageLayout::PresentSource_KHR;

				RenderPass::AttachmentReference colorAttachmentRef {};

				colorAttachmentRef.Attachment = 0                                    ;
				//colorAttachmentRef.Layout     = EImageLayout::Color_AttachmentOptimal;
				colorAttachmentRef.Layout   = EImageLayout::Color_AttachmentOptimal;

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

				colorAttachmentResolve.Format = SwapChain_Old->GetFormat();

				colorAttachmentResolve.Samples = ESampleCount::_1;

				colorAttachmentResolve.LoadOp  = EAttachmentLoadOperation ::DontCare;
				colorAttachmentResolve.StoreOp = EAttachmentStoreOperation::Store   ;

				colorAttachmentResolve.StencilLoadOp  = EAttachmentLoadOperation ::DontCare;
				colorAttachmentResolve.StencilStoreOp = EAttachmentStoreOperation::DontCare;

				colorAttachmentResolve.InitialLayout = EImageLayout::Undefined        ;
				colorAttachmentResolve.FinalLayout   = EImageLayout::PresentSource_KHR;

				RenderPass::AttachmentReference colorAttachmentResolveRef {};

				colorAttachmentResolveRef.Attachment = 2;

				colorAttachmentResolveRef.Layout = EImageLayout::PresentSource_KHR;

				RenderPass::SubpassDescription subpass {};

				subpass.PipelineBindPoint      = EPipelineBindPoint::Graphics;
				subpass.ColorAttachmentCount   = 1                           ;
				subpass.ColorAttachments       = &colorAttachmentRef         ;
				subpass.DepthStencilAttachment = &depthAttachmentRef         ;

				StaticArray<RenderPass::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

				RenderPass::CreateInfo renderPassInfo;

				renderPassInfo.AttachmentCount = SCast<ui32>(attachments.size());
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

				if (RenderPass_Old.Create(GPU_Comms::GetEngagedDevice(), renderPassInfo) != EResult::Success)
				{
					throw std::runtime_error("failed to create render pass!");
				}
			}

			void CreateSyncObjects()
			{
				ImageAvailable_Semaphores.resize(MaxFramesInFlight);
				RenderFinished_Semaphores.resize(MaxFramesInFlight);

				InFlightFences.resize(MaxFramesInFlight      );
				ImagesInFlight.resize(SwapChain_Old->GetImages().size());

				Semaphore::CreateInfo semaphore_CreationSpec;

				Fence::CreateInfo fence_CreationSpec;

				fence_CreationSpec.Flags.Set(EFenceCreateFlag::Signaled);

				for (WordSize index = 0; index < WordSize(MaxFramesInFlight); index++)
				{
					EResult
					result = ImageAvailable_Semaphores[index].Create(GPU_Comms::GetEngagedDevice(), semaphore_CreationSpec);
					result = RenderFinished_Semaphores[index].Create(GPU_Comms::GetEngagedDevice(), semaphore_CreationSpec);
					result = InFlightFences           [index].Create(GPU_Comms::GetEngagedDevice(), fence_CreationSpec    );

					if (result != EResult::Success)
						throw std::runtime_error("Failed to create synchronization objects for a frame!");
				}
			}

			void CreateTextureImage(RoCStr _filePath)
			{
				int textureWidth, textureHeight, textureChannels;

				using ImageBytes = stbi_uc;

				ImageBytes* imageData = stbi_load(_filePath, &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);

				DeviceSize imageSize = textureWidth * textureHeight	 * 4;

				MipMapLevels = SCast<ui32>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1;

				if (!imageData)
				{
					throw RuntimeError("Failed to load image data!");
				}

				Buffer stagingBuffer;

				Memory stagingBufferMemory;

				Buffer::CreateInfo stagingBufferInfo {};

				stagingBufferInfo.Size = imageSize;
				stagingBufferInfo.SharingMode = ESharingMode::Exclusive;

				stagingBufferInfo.Usage.Set(EImageUsage::TransferSource);

				stagingBuffer.Create(GPU_Comms::GetEngagedDevice(), stagingBufferInfo);

				Memory::AllocateInfo allocInfo;

				auto& memReq = stagingBuffer.GetMemoryRequirements();

				allocInfo.AllocationSize = memReq.Size;

				allocInfo.MemoryTypeIndex =
					stagingBuffer.GetDevice().GetPhysicalDevice().FindMemoryType
					(memReq.MemoryTypeBits, Memory::PropertyFlags(EMemoryPropertyFlag::HostVisible, EMemoryPropertyFlag::HostCoherent));

				stagingBufferMemory.Allocate(GPU_Comms::GetEngagedDevice(), allocInfo);

				stagingBuffer.BindMemory(stagingBufferMemory, Memory::ZeroOffset);

				RoVoidPtr address = imageData;

				stagingBufferMemory.WriteToGPU(0, imageSize, 0, address);

				stbi_image_free(imageData);

				CreateImage
				(
					textureWidth, 
					textureHeight,  
					MipMapLevels,
					ESampleCount::_1,   // TODO: Change me.
					//EFormat::R8_G8_B8_A8_SRGB, 
					EFormat::R8_G8_B8_A8_UNormalized, 
					EImageTiling::Optimal, 
					Image::UsageFlags(EImageUsage::TransferDestination, EImageUsage::Sampled, EImageUsage::TransferSource), 
					Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal),
					TextureImage, 
					TextureImageMemory
				);

				//TransitionImageLayout(TextureImage, EFormat::R8_G8_B8_A8_UNormalized, EImageLayout::Undefined, EImageLayout::TransferDestination_Optimal, MipMapLevels);

				TextureImage.TransitionLayout(EImageLayout::Undefined, EImageLayout::TransferDestination_Optimal);

				CopyBufferToImage(stagingBuffer, TextureImage, SCast<ui32>(textureWidth), SCast<ui32>(textureHeight));

				GenerateMipMaps(TextureImage, EFormat::R8_G8_B8_A8_UNormalized, textureWidth, textureHeight, MipMapLevels);

				stagingBuffer.Destroy(); 
				stagingBufferMemory.Free();
			}

			void CreateTextureSampler()
			{
				Sampler::CreateInfo samplerInfo {};

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
				samplerInfo.MaxLod     = SCast<f32>(MipMapLevels);

				if (TextureSampler.Create(GPU_Comms::GetEngagedDevice(), samplerInfo) != EResult::Success)
					throw RuntimeError("Failed to create texture sampler!");
			}

			StaticArray<V3::ShaderModule, 2> CreateTriShaders()
			{
				using Bytecode_Buffer = DynamicArray<Bytecode>;

				using ShaderModule = V3::ShaderModule;

				// Shader setup

				auto triShader_VertCode = Core::IO::BufferFile(String(Renderer::Shader::Paths::TriangleShader) + "TriangleShader_Vert.spv");
				auto triShader_FragCode = Core::IO::BufferFile(String(Renderer::Shader::Paths::TriangleShader) + "TriangleShader_Frag.spv");

				//TODO: FIGURE THIS OUT.
				Bytecode_Buffer triShader_Vert_Bytecode(triShader_VertCode.begin(), triShader_VertCode.end());
				Bytecode_Buffer triShader_Frag_Bytecode(triShader_FragCode.begin(), triShader_FragCode.end());

				ShaderModule::CreateInfo vertInfo(triShader_VertCode.data(), triShader_VertCode.size());
				ShaderModule::CreateInfo fragInfo(triShader_FragCode.data(), triShader_FragCode.size());

				ShaderModule triShaderModule_Vert; triShaderModule_Vert.Create(GPU_Comms::GetEngagedDevice(), vertInfo);
				ShaderModule triShaderModule_Frag; triShaderModule_Frag.Create(GPU_Comms::GetEngagedDevice(), fragInfo);

				StaticArray<ShaderModule, 2> result = { move(triShaderModule_Vert), move(triShaderModule_Frag) };

				return result;
			}

			void CreateUniformBuffers()
			{
				DeviceSize bufferSize = sizeof(UniformBufferObject);

				UniformBuffers      .resize(SwapChain_Old->GetImages().size());
				UniformBuffersMemory.resize(SwapChain_Old->GetImages().size());

				Buffer::CreateInfo uniformBufferInfo {};

				uniformBufferInfo.Size = bufferSize;

				uniformBufferInfo.Usage.Set(EBufferUsage::UniformBuffer);

				uniformBufferInfo.SharingMode = ESharingMode::Exclusive;

				for (WordSize index = 0; index < SwapChain_Old->GetImages().size(); index++)
				{
					UniformBuffers[index].Create(GPU_Comms::GetEngagedDevice(), uniformBufferInfo);

					Memory::AllocateInfo allocInfo;

					auto& memReq = UniformBuffers[index].GetMemoryRequirements();

					allocInfo.AllocationSize = memReq.Size;

					allocInfo.MemoryTypeIndex =
						UniformBuffers[index].GetDevice().GetPhysicalDevice().FindMemoryType
						(memReq.MemoryTypeBits, Memory::PropertyFlags(EMemoryPropertyFlag::HostVisible, EMemoryPropertyFlag::HostCoherent));

					UniformBuffersMemory[index].Allocate(GPU_Comms::GetEngagedDevice(), allocInfo);

					UniformBuffers[index].BindMemory(UniformBuffersMemory[index], Memory::ZeroOffset);
				}
			}

			StaticArray<V3::ShaderModule, 2> Create_VKTut_Shaders()
			{
				using ShaderModule = V3::ShaderModule;

				// Shader setup

				auto vertCode = Core::IO::BufferFile(String(Renderer::Shader::Paths::VKTut) + "VKTut_V5_Vert.spv");
				auto fragCode = Core::IO::BufferFile(String(Renderer::Shader::Paths::VKTut) + "VKTut_V5_Frag.spv");

				//TODO: FIGURE THIS OUT.
				Bytecode_Buffer vertBytecode(vertCode.begin(), vertCode.end());
				Bytecode_Buffer fragBytecode(fragCode.begin(), fragCode.end());

				ShaderModule vertShaderModule; ShaderModule::CreateInfo vertShaderInfo(vertCode.data(), vertCode.size());
				ShaderModule fragShaderModule; ShaderModule::CreateInfo fragShaderInfo(fragCode.data(), fragCode.size());

				vertShaderModule.Create(GPU_Comms::GetEngagedDevice(), vertShaderInfo);
				fragShaderModule.Create(GPU_Comms::GetEngagedDevice(), fragShaderInfo);
				
				StaticArray<ShaderModule, 2> result = { move(vertShaderModule), move(fragShaderModule) };

				return result;
			}

			void CreateVertexBuffers(Buffer& _vertexBuffer, Memory& _vertexBufferMemory)
			{
				DeviceSize bufferSize = sizeof(ModelVerticies[0]) * ModelVerticies.size();

				Buffer stagingBuffer;

				Memory stagingBufferMemory;

				Buffer::CreateInfo stagingBufferInfo {};

				stagingBufferInfo.SharingMode = ESharingMode::Exclusive;
				stagingBufferInfo.Size        = bufferSize;

				stagingBufferInfo.Usage.Set(EBufferUsage::TransferSource);

				stagingBuffer.Create(GPU_Comms::GetEngagedDevice(), stagingBufferInfo);

				Memory::AllocateInfo allocInfo;

				auto& memReq = stagingBuffer.GetMemoryRequirements();

				allocInfo.AllocationSize = memReq.Size;

				allocInfo.MemoryTypeIndex =
					stagingBuffer.GetDevice().GetPhysicalDevice().FindMemoryType
					(memReq.MemoryTypeBits, Memory::PropertyFlags(EMemoryPropertyFlag::HostVisible, EMemoryPropertyFlag::HostCoherent));

				stagingBufferMemory.Allocate(GPU_Comms::GetEngagedDevice(), allocInfo);

				stagingBuffer.BindMemory(stagingBufferMemory, Memory::ZeroOffset);

				// Hard coded vertex data...

				RoVoidPtr vertexData = ModelVerticies.data();

				Memory::MapFlags mapflags;

				stagingBufferMemory.WriteToGPU(0, bufferSize, mapflags, vertexData);

				Buffer::CreateInfo vertexBufferInfo {};

				vertexBufferInfo.Size        = bufferSize;
				vertexBufferInfo.SharingMode = ESharingMode::Exclusive;

				vertexBufferInfo.Usage.Set(EBufferUsage::TransferDestination, EBufferUsage::VertexBuffer);

				_vertexBuffer.Create(GPU_Comms::GetEngagedDevice(), vertexBufferInfo);

				auto& memReqVert = _vertexBuffer.GetMemoryRequirements();

				allocInfo.AllocationSize = memReqVert.Size;

				allocInfo.MemoryTypeIndex = 
					_vertexBuffer.GetDevice().GetPhysicalDevice().FindMemoryType
					(memReqVert.MemoryTypeBits, Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal));

				_vertexBufferMemory.Allocate(GPU_Comms::GetEngagedDevice(), allocInfo);

				_vertexBuffer.BindMemory(_vertexBufferMemory, Memory::ZeroOffset);

				Buffer::CopyInfo copyInfo {}; copyInfo.DestinationOffset = 0; copyInfo.SourceOffset = 0; copyInfo.Size = bufferSize;

				SingleTimeCommandPool.CopyBuffer(stagingBuffer, _vertexBuffer, copyInfo, GPU_Comms::GetEngagedDevice().GetGraphicsQueue());

				stagingBuffer      .Destroy();
				stagingBufferMemory.Free();
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

			EFormat FindSupportedFormat(const DynamicArray<EFormat>& _canidates, EImageTiling _tiling, FormatFeatureFlags _features)
			{
				for (EFormat format : _canidates)
				{
					FormatProperties properties = GPU_Comms::GetEngagedDevice().GetPhysicalDevice().GetFormatProperties(format);

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

			void GenerateMipMaps(V3::Image& _image, EFormat _format, ui32 _textureWidth, ui32 _textureHeight, ui32 _mipLevels)
			{
				// Check if image format supports linear blitting
				FormatProperties formatProperties = GPU_Comms::GetEngagedDevice().GetPhysicalDevice().GetFormatProperties(_format);

				if (!(formatProperties.OptimalTilingFeatures.HasFlag(EFormatFeatureFlag::SampledImageFilterLinear)))
				{
					throw std::runtime_error("Texture image format does not support linear blitting!");
				}

				CommandBuffer commandBuffer = SingleTimeCommandPool.BeginSingleTimeCommands();

				Image::Memory_Barrier barrier{};

				barrier.Image               = _image             ;
				barrier.SrcQueueFamilyIndex = QueueFamily_Ignored;
				barrier.DstQueueFamilyIndex = QueueFamily_Ignored;

				barrier.SubresourceRange.AspectMask.Set(EImageAspect::Color);

				barrier.SubresourceRange.BaseArrayLayer = 0;
				barrier.SubresourceRange.LayerCount     = 1;
				barrier.SubresourceRange.LevelCount     = 1;

				si32 mipWidth = _textureWidth;
				si32 mipHeight = _textureHeight;

				for (uint32_t index = 1; index < _mipLevels; index++)
				{
					barrier.SubresourceRange.BaseMipLevel = index - 1;

					barrier.OldLayout = EImageLayout::TransferDestination_Optimal;
					barrier.NewLayout = EImageLayout::TransferSource_Optimal;

					barrier.SrcAccessMask.Set(EAccessFlag::TransferWrite);
					barrier.DstAccessMask.Set(EAccessFlag::TransferRead);

					commandBuffer.SubmitPipelineBarrier
					(
						EPipelineStageFlag::Transfer, EPipelineStageFlag::Transfer, 0,
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

					commandBuffer.BlitImage
					(
						_image, EImageLayout::TransferSource_Optimal,
						_image, EImageLayout::TransferDestination_Optimal,
						1, &blit,
						EFilter::Linear
					);

					barrier.OldLayout = EImageLayout::TransferSource_Optimal;
					barrier.NewLayout = EImageLayout::Shader_ReadonlyOptimal;

					barrier.SrcAccessMask.Set(EAccessFlag::TransferRead);
					barrier.DstAccessMask.Set(EAccessFlag::ShaderRead  );

					commandBuffer.SubmitPipelineBarrier
					(
						EPipelineStageFlag::Transfer, EPipelineStageFlag::FragementShader, 0,
						1, &barrier
					);

					if (mipWidth  > 1) mipWidth  /= 2;
					if (mipHeight > 1) mipHeight /= 2;
				}

				barrier.SubresourceRange.BaseMipLevel = _mipLevels - 1;

				barrier.OldLayout = EImageLayout::TransferDestination_Optimal;
				barrier.NewLayout = EImageLayout::Shader_ReadonlyOptimal     ;

				barrier.SrcAccessMask.Set(EAccessFlag::TransferWrite);
				barrier.DstAccessMask.Set(EAccessFlag::ShaderRead   );

				commandBuffer.SubmitPipelineBarrier
				(
					EPipelineStageFlag::Transfer, EPipelineStageFlag::FragementShader, 0,
					1, &barrier
				);

				SingleTimeCommandPool.EndSingleTimeCommands(commandBuffer);
			}

			void UpdateUniformBuffers(ui32 _currentImage)
			{
				static auto startTime = std::chrono::high_resolution_clock::now();

				auto currentTime = std::chrono::high_resolution_clock::now();

				float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

				UniformBufferObject ubo {};

				ubo.ModelSpace = glm::rotate(glm::mat4(1.0f), time * glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 0.5f));

				ubo.Viewport = glm::lookAt(glm::vec3(1.7f, 1.7f, 1.7f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));   // The default
				//ubo.Viewport = glm::lookAt(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); crying cat

				ubo.Projection = glm::perspective(glm::radians(45.0f), SwapChain_Old->GetExtent().Width / (float) SwapChain_Old->GetExtent().Height, 0.1f, 10.0f);

				ubo.Projection[1][1] *= -1;

				const void* address = &ubo;

				UniformBuffersMemory[_currentImage].WriteToGPU(0, sizeof(ubo), 0, address);
			}

			// GPU_HAL

			void Start_GPUComms(RoCStr _applicationName, AppVersion _applicationVersion)
			{
				CLog("Initializing GPU Communication");

				GPU_Comms::Initialize(_applicationName, _applicationVersion);

				GPU_Comms::AcquirePhysicalDevices();

				GPU_Comms::GenerateLogicalDevices();

				GPU_Comms::EngageMostSuitableDevice();

				Deck::Prepare();
			}

			void Cease_GPUComms()
			{
				Deck::Wipe();

				GPU_Comms::Cease();	
			}

			void WaitFor_GPUIdle()
			{
				GPU_Comms::GetEngagedDevice().WaitUntilIdle();
			}
			
			ptr<ARenderContext> GetRenderContext(ptr<OSAL::Window> /* _window */)
			{
				return &RenderContext_Default;
			}

			void SetRenderContext()
			{
				RenderContext_Default.ApplicationInstance = GPU_Comms::GetAppInstance();
				RenderContext_Default.PhysicalDevice      = GPU_Comms::GetEngagedDevice().GetPhysicalDevice();
				RenderContext_Default.LogicalDevice       = GPU_Comms::GetEngagedDevice()          ;
				RenderContext_Default.Queue               = GPU_Comms::GetEngagedDevice().GetGraphicsQueue();
				RenderContext_Default.PipelineCache       = PipelineCache_Old           ;
				RenderContext_Default.ImageFormat         = SwapChain_Old->GetFormat()   ;
				RenderContext_Default.FrameSize           = SwapChain_Old->GetExtent()        ;
				RenderContext_Default.Allocator           = Memory::DefaultAllocator;
				RenderContext_Default.RenderPass          = RenderPass_Old              ;
				RenderContext_Default.MinimumFrameBuffers = SwapChain_Old->GetMinimumImageCount();
				RenderContext_Default.FrameBufferCount    = SCast<ui32>(SwapChain_Old->GetImages().size());
				RenderContext_Default.MSAA_Samples        = MSAA_Samples            ;
			}

			void Default_InitializeRenderer(ptr<Window> _window)
			{
				CLog("Doing dirty renderer initialization, clean this up!");

				Surface_Old = &Rendering::Request_Surface(_window);

				Surface::Format format;

				format.Format     = EFormat::B8_G8_R8_A8_UNormalized;
				format.ColorSpace = EColorSpace::SRGB_NonLinear;

				auto& swapchain = Rendering::Request_SwapChain(*Surface_Old, format); 

				SwapChain_Old = &swapchain;

				// Ported
				CreateRenderPass();

				CreateDescriptorSetLayout();

				// Ported
				StaticArray<V3::ShaderModule, 2> shaders = Create_VKTut_Shaders();

				//
				CreateGraphicsPipeline(shaders);

				for (auto& shader : shaders)
				{
					shader.Destroy();
				}

				// Ported
				CreateFrameObjects();

				// Ported
				CreateColorResources();

				// Ported
				CreateDepthResources();

				// Ported
				CreateFrameBuffers();

				// Ported
				CreateSyncObjects();

				// Hard coded model setup

				//LoadModel(VikingRoom_ModelPath);

				// Ported
				CreateVertexBuffers(VertexBuffer_Old, VertexBufferMemory);

				// Ported
				CreateIndexBuffer();

				CreateUniformBuffers();

				// Ported
				CreateTextureImage(VikingRoom_TexturePath.c_str()); 

				// Ported
				TextureImageView = CreateImageView(TextureImage, EFormat::R8_G8_B8_A8_UNormalized, Image::AspectFlags(EImageAspect::Color), MipMapLevels);

				// Ported
				CreateTextureSampler();

				CreateDescriptorPool();

				CreateDescriptorSets();

				// Ported
				SetRenderContext();
			}

			void Default_ReinitializeRenderer(ptr<Window> _window)
			{
				OSAL::FrameBufferDimensions dimensions;

				dimensions = OSAL::GetFramebufferDimensions(_window);

				while (dimensions.Width == 0 || dimensions.Height == 0)
				{
					OSAL::GetFramebufferDimensions(_window);

					SAL::GLFW::WaitForEvents();
				}

				GPU_Comms::GetEngagedDevice().GetGraphicsQueue().WaitUntilIdle();

				// Ported
				CleanupSwapChain();

				SwapChain_Old->QuerySurfaceChanges();

				Surface::Format format;

				format.Format = EFormat::B8_G8_R8_A8_UNormalized;
				format.ColorSpace = EColorSpace::SRGB_NonLinear;

				// Ported
				CreateRenderPass();

				// Ported
				StaticArray<V3::ShaderModule, 2> shaders = Create_VKTut_Shaders();

				CreateGraphicsPipeline(shaders);

				for (auto& shader : shaders)
				{
					shader.Destroy();
				}

				CreateColorResources();

				CreateDepthResources();

				// Ported
				CreateFrameBuffers();

				CreateUniformBuffers();

				CreateDescriptorPool();

				CreateDescriptorSets();
			}

			DynamicArray<CommandBuffer::Handle> CommandBuffersToSubmit;

			void Default_DrawFrame(ptr<Window> _window)
			{
				InFlightFences[CurrentFrame].WaitFor(UInt64Max);


				ui32 imageIndex;

				EResult result = 
					SwapChain_Old->AcquireNextImage
					(
						UInt64Max                              , 
						ImageAvailable_Semaphores[CurrentFrame], 
						Null<Fence::Handle>                     ,
						imageIndex
					);
					
				if (result == EResult::Error_OutOfDate_KHR) 
				{
					Default_ReinitializeRenderer(_window);

					return;
				}
				else if (result != EResult::Success && result != EResult::Suboptimal_KHR) 
				{
					throw std::runtime_error("Failed to acquire swap chain image!");
				}

				if (ImagesInFlight[imageIndex] != nullptr && *ImagesInFlight[imageIndex] != Null<Fence::Handle>) 
					ImagesInFlight[imageIndex]->WaitFor(UInt64Max);

				ImagesInFlight[imageIndex] = &InFlightFences[CurrentFrame];


				// Updating

				CommandPools_Old[imageIndex].Reset(0);

				RecordToBuffers(imageIndex);

				UpdateUniformBuffers(imageIndex);

				// Submit to Graphics Queue

				CommandBuffer::SubmitInfo submitInfo {};

				Semaphore::Handle waitSemaphores[] = { ImageAvailable_Semaphores[CurrentFrame] };

				Pipeline::StageFlags waitStages[1] {};

				waitStages[0].Set(EPipelineStageFlag::ColorAttachmentOutput);

				CommandBuffersToSubmit.push_back(CommandBuffers_Old[CurrentFrame]);

				submitInfo.WaitSemaphoreCount = 1             ;
				submitInfo.WaitSemaphores     = waitSemaphores;
				submitInfo.WaitDstStageMask   = waitStages    ;

				submitInfo.CommandBufferCount = 1;
				submitInfo.CommandBuffers     = CommandBuffers_Old[imageIndex];


				Semaphore::Handle signalSemaphores[] = { RenderFinished_Semaphores[CurrentFrame] };

				submitInfo.SignalSemaphoreCount = 1               ;
				submitInfo.SignalSemaphores     = signalSemaphores;

				InFlightFences[CurrentFrame].Reset();

				if (GPU_Comms::GetGraphicsQueue().SubmitToQueue(1, submitInfo, InFlightFences[CurrentFrame]) != EResult::Success) 
					throw std::runtime_error("Failed to submit draw command buffer!");

				CommandBuffersToSubmit.clear();

				// Submit to presentation

				Swapchain::PresentationInfo presentInfo{};

				presentInfo.WaitSemaphoreCount = 1                    ;
				presentInfo.WaitSemaphores     = signalSemaphores     ;


				Swapchain::Handle swapChains[] = { *SwapChain_Old };

				presentInfo.SwapchainCount = 1          ;
				presentInfo.Swapchains     = swapChains ;
				presentInfo.ImageIndices   = &imageIndex;

				presentInfo.Results = nullptr; // Optional

				result = GPU_Comms::GetEngagedDevice().GetGraphicsQueue().QueuePresentation(presentInfo);

				if (result == EResult::Error_OutOfDate_KHR || result == EResult::Suboptimal_KHR || FramebufferResized) 
				{
					FramebufferResized = false;

					Default_ReinitializeRenderer(_window);
				}
				else if (result != EResult::Success) 
				{
					throw std::runtime_error("failed to present swap chain image!");
				}

				CurrentFrame = (CurrentFrame + 1) % MaxFramesInFlight;
			}

			void Default_DeinitializeRenderer(ptr<OSAL::Window> /* _window */)
			{
				CLog("Deinitializing the dirty renderer");

				CleanupSwapChain();

				//Heap
				//(
					TextureSampler    .Destroy(); 
					TextureImageView  .Destroy(); 
					TextureImage      .Destroy(); 
					TextureImageMemory.Free   (); 

					DescriptorSetLayout_Old.Destroy();

					IndexBuffer_Old      .Destroy(); 
					IndexBufferMemory.Free   (); 

					VertexBuffer_Old  .Destroy(); 
					VertexBufferMemory.Free   (); 

					for (WordSize index = 0; index < WordSize(MaxFramesInFlight); index++) 
					{
						RenderFinished_Semaphores[index].Destroy(); 
						ImageAvailable_Semaphores[index].Destroy(); 
						InFlightFences           [index].Destroy(); 
					}

					SingleTimeCommandPool.Destroy(); 

					for (auto& pool : CommandPools_Old)
					{
						pool.Destroy(); 
					}

					//if (Meta::Vulkan::EnableLayers) GPU_Messenger.Destroy();

					Rendering::Retire_SwapChain(SwapChain_Old);

					Rendering::Retire_Surface(Surface_Old);
				//);
			}
		}
	}

#endif
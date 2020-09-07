// Parent Header
#include "GPU_Vulkan.hpp"



// TUtorial: This needs to be removed from GPU_Vulkan
// This can be done by making the design of the GPU HAL agnostic to what the tutorial requires.
#include "_TutorialRelated.hpp"

// Proper stuff
#include "HAL_Backend.hpp"
#include "GPUVK_Comms.hpp"
#include "GPUVK_PayloadDeck.hpp"
#include "GPUVK_Renderer.hpp"


#include "Dev/Console.hpp"

#if VulkanAPI_Interface == VaultedThermals_Interface

	namespace HAL::GPU
	{
		namespace Vulkan
		{
			// Static Data

			// Command

			CommandPool                         SingleTimeCommandPool;
			DynamicArray<CommandPool          > CommandPools_Old     ;   
			DynamicArray<CommandBuffer        > CommandBuffers_Old   ;
			DynamicArray<CommandBuffer::Handle> CommandBufferHandles ;

			// Model V1 Rendering Pipeline

			GraphicsPipeline GraphicsPipeline_Old;

			RenderPass RenderPass_Old;   

			Pipeline::Cache PipelineCache;   // Implement.

			Pipeline::Layout::DescriptorSet DescriptorSetLayout;
			Pipeline::Layout                PipelineLayout;

			V3::DescriptorPool DescriptorPool;

			DynamicArray<DescriptorSet        > DescriptorSets      ;
			DynamicArray<DescriptorSet::Handle> DescriptorSetHandles;

			uint32       MipMapLevels                   ;
			ESampleCount MSAA_Samples = ESampleCount::_1;

			// Basic Rendering Synch (Upgrade this later)			

			DynamicArray<Fence> InFlightFences;
			DynamicArray<Fence> ImagesInFlight;

			DynamicArray<Semaphore> ImageAvailable_Semaphores;
			DynamicArray<Semaphore> RenderFinished_Semaphores;

			// Surface Context

			ptr<Surface> Surface_Old;

			// Swapchain for surface

			Swapchain SwapChain_Old        ;
			Extent2D  SwapChain_Extent     ;
			EFormat   SwapChain_ImageFormat;

			DynamicArray<Image>       SwapChain_Images      ;
			DynamicArray<ImageView>   SwapChain_ImageViews  ;
			DynamicArray<Framebuffer> SwapChain_Framebuffers;

			uint32 Swap_MinImageCount;

			// Related to Rendering State Tracking

			bool     FramebufferResized = false;
			WordSize CurrentFrame       = 0;
			sint32   MaxFramesInFlight  = 2;
			
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
				uint32                _width,
				uint32                _height,
				uint32                _mipLevels,
				ESampleCount          _numSamples,
				EFormat               _format,
				EImageTiling          _tiling,
				Image::UsageFlags     _usage,
				Memory::PropertyFlags _properties,
				Image& _image,
				Memory& _imageMemory
			);

			ImageView CreateImageView(Image& _image, EFormat _format, Image::AspectFlags _aspectFlags, uint32 /*_miplevels*/);

			ptr<Surface> ClearColor_Surface;
			ptr<Swapchain> ClearColor_Swap;
			ptr<RenderContext> ClearColor_Context;

			void Start_ClearColorDemo(ptr<OSAL::Window> _window)
			{
				ClearColor_Surface = &Request_Surface(_window);

				Surface::Format format;

				format.Format = EFormat::B8_G8_R8_A8_UNormalized;
				format.ColorSpace = EColorSpace::SRGB_NonLinear;

				ClearColor_Swap = &Request_SwapChain(*ClearColor_Surface, format);

				ClearColor_Context = &Request_RenderContext(*ClearColor_Swap);
			}

			void Render()
			{
				Renderer_Update();
			}

			void Present()
			{
				Renderer_Present();
			}

			void Stop_ClearColorDemo()
			{
				Retire_RenderContext(ClearColor_Context);
				Retire_Swapchain    (ClearColor_Swap   );
				Retire_Surface      (ClearColor_Surface);
			}

		#pragma region Staying

			uint32 GetMinimumFramebufferCount()
			{
				return 2;
			}

			uint32 GetNumberOfFramebuffers()
			{
				return SCast<uint32>(SwapChain_Images.size());
			}

			CommandBuffer RequestSingleTimeBuffer()
			{
				EResult result;

				return Heap(SingleTimeCommandPool.BeginSingleTimeCommands(result));
			}

			void EndSingleTimeBuffer(CommandBuffer& _buffer)
			{
				//SingleTimeCommandPool.EndSingleTimeCommands(_buffer, *GraphicsQueue);
				Heap(SingleTimeCommandPool.EndSingleTimeCommands(_buffer, GetEngagedDevice().GetGraphicsQueue()));
			}

			EResult RequestDescriptorPool(V3::DescriptorPool& _pool, V3::DescriptorPool::CreateInfo _info)
			{
				EResult returnCode = Heap(_pool.Create(GetEngagedDevice(), _info));

				return returnCode;
			}

		#pragma endregion Staying

			// Functions

			void CleanupSwapChain()
			{
				//Heap
				//(
					ColorImageView  .Destroy();
					ColorImage      .Destroy();
					ColorImageMemory.Free();

					DepthImageView  .Destroy();
					DepthImage      .Destroy();
					DepthImageMemory.Free   ();

					for (WordSize index = 0; index < SwapChain_Framebuffers.size(); index++)
					{
						SwapChain_Framebuffers[index].Destroy();
					}

					GraphicsPipeline_Old.Destroy();
					PipelineLayout  .Destroy();
					RenderPass_Old      .Destroy();

					for (WordSize index = 0; index < SwapChain_ImageViews.size(); index++)
					{
						SwapChain_ImageViews[index].Destroy();
					}

					SwapChain_Old.Destroy();

					for (WordSize index = 0; index < SwapChain_ImageViews.size(); index++)
					{
						UniformBuffers      [index].Destroy();
						UniformBuffersMemory[index].Free();
					}
					
					DescriptorPool.Destroy();
				//);
			}

			void CopyBufferToImage(Buffer _buffer, Image _image, uint32 _width, uint32 _height)
			{
				EResult result;

				CommandBuffer commandBuffer = Heap(SingleTimeCommandPool.BeginSingleTimeCommands(result));

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

				commandBuffer.CopyBufferToImage(_buffer, _image, VT::Corridors::EImageLayout::TransferDestination_Optimal, 1, &region);
				
				//SingleTimeCommandPool.EndSingleTimeCommands(commandBuffer, *GraphicsQueue);
				Heap(SingleTimeCommandPool.EndSingleTimeCommands(commandBuffer, GetEngagedDevice().GetGraphicsQueue()));
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

				renderPassInfo.RenderArea.Extent = SwapChain_Extent;

				StaticArray<ClearValue, 2> clearValues {}; 

				clearValues[0].Color        = { 0.0f, 0.0f, 0.0f, 0.1f };
				clearValues[1].DepthStencil = { 1.0f, 0                };

				renderPassInfo.ClearValueCount = SCast<uint32>(clearValues.size());
				renderPassInfo.ClearValues     = clearValues.data()               ;

				CommandBuffers_Old[index].BeginRenderPass(renderPassInfo, ESubpassContents::Inline);

				CommandBuffers_Old[index].BindPipeline(EPipelineBindPoint::Graphics, GraphicsPipeline_Old);

				Buffer::Handle vertexBuffers = VertexBuffer_Old;

				DeviceSize offsets = 0;

				CommandBuffers_Old[index].BindVertexBuffers(0, 1, &vertexBuffers, &offsets);

				CommandBuffers_Old[index].BindIndexBuffer(IndexBuffer, 0, EIndexType::uInt32);

				CommandBuffers_Old[index].BindDescriptorSets
				(
					EPipelineBindPoint::Graphics,
					PipelineLayout,
					0,
					1,
					&DescriptorSets[index].GetHandle()
				);

				CommandBuffers_Old[index].DrawIndexed
				(
					SCast<uint32>(ModelIndicies.size()),
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

			void CreateColorResources()
			{
				EFormat colorFormat = SwapChain_ImageFormat;

				CreateImage
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

			void CreateFrameObjects()
			{
				CommandPool::CreateInfo poolInfo {};

				//poolInfo.QueueFamilyIndex = QueueIndices.Graphics.value();
				poolInfo.QueueFamilyIndex = GetEngagedDevice().GetGraphicsQueue().GetFamilyIndex();
				poolInfo.Flags            = CommandPool::CreateFlgas()                            ;   // Optional

				Heap(SingleTimeCommandPool.Create(GetEngagedDevice(), poolInfo));

				CommandPools_Old    .resize(SwapChain_Images.size());
				CommandBuffers_Old  .resize(SwapChain_Images.size());
				CommandBufferHandles.resize(SwapChain_Images.size());

				for (DeviceSize index = 0; index < SwapChain_Images.size(); index++)
				{
					if (Heap(CommandPools_Old[index].Create(GetEngagedDevice(), poolInfo)) != EResult::Success)
					{
						throw std::runtime_error("failed to create command pool!");
					}

					CommandPool::AllocateInfo info;

					info.BufferCount = 1;
					info.Pool        = CommandPools_Old[index];
					info.Level       = ECommandBufferLevel::Primary;

					if (Heap(CommandPools_Old[index].Allocate(info,  CommandBuffers_Old[index])) != EResult::Success)
						throw std::runtime_error("failed to allocate command buffers!");

					CommandBufferHandles[index] = CommandBuffers_Old[index];
				}
			}

			void CreateDepthResources()
			{
				EFormat depthFormat = FindDepthFormat();

				CreateImage
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

				DepthImage.TransitionLayout(EImageLayout::Undefined, EImageLayout::DepthStencil_AttachmentOptimal);

				//TransitionImageLayout(DepthImage, depthFormat, EImageLayout::Undefined, EImageLayout::DepthStencil_AttachmentOptimal, 1);
			}

			void CreateDescriptorPool()
			{
				StaticArray<DescriptorPool::Size, 2> poolSizes {};

				poolSizes[0].Type = EDescriptorType::UniformBuffer;
				poolSizes[0].Count = SCast<uint32>(SwapChain_Images.size());

				poolSizes[1].Type = EDescriptorType::Sampler;
				poolSizes[1].Count = SCast<uint32>(SwapChain_Images.size());

				DescriptorPool::CreateInfo poolInfo {};

				poolInfo.PoolSizeCount = SCast<uint32>(poolSizes.size());
				poolInfo.PoolSizes     = poolSizes.data()               ;

				poolInfo.MaxSets = SCast<uint32>(SwapChain_Images.size());

				if (Heap(DescriptorPool.Create(GetEngagedDevice(), poolInfo)) != EResult::Success)
					throw RuntimeError("Failed to create descriptor pool!");
			}

			void CreateDescriptorSets()
			{
				DynamicArray<Pipeline::Layout::DescriptorSet::Handle> layouts(SwapChain_Images.size(), Pipeline::Layout::DescriptorSet::Handle(DescriptorSetLayout));

				DescriptorPool::AllocateInfo allocInfo{};

				allocInfo.DescriptorPool     = DescriptorPool;
				allocInfo.DescriptorSetCount = SCast<uint32>(SwapChain_Images.size());
				allocInfo.SetLayouts         = layouts.data();

				if (Heap(DescriptorPool.Allocate(allocInfo, DescriptorSets, DescriptorSetHandles)) != EResult::Success)
					throw std::runtime_error("failed to allocate descriptor sets!");

				for (WordSize index = 0; index < SwapChain_Images.size(); index++)
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

					DescriptorSets[0].Update(SCast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);	
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

				layoutInfo.BindingCount = SCast<uint32>(bindings.size());
				layoutInfo.Bindings = bindings.data();

				DescriptorSetLayout.Assign(GetEngagedDevice(), layoutInfo);

				if (Heap(DescriptorSetLayout.Create()) != EResult::Success)
					throw RuntimeError("Failed to create descriptor set layout!");
			}

			void CreateFrameBuffers()
			{
				SwapChain_Framebuffers.resize(SwapChain_ImageViews.size());

				for (WordSize index = 0; index < SwapChain_ImageViews.size(); index++) 
				{
					StaticArray<ImageView::Handle, 2> attachments = 
					{
						SwapChain_ImageViews[index],
						DepthImageView
						//ColorImageView.GetHandle(), // Sampler image.
					};

					Framebuffer::CreateInfo framebufferInfo {};

					framebufferInfo.RenderPass      = RenderPass_Old       ;
					framebufferInfo.AttachmentCount = SCast<uint32>(attachments.size());
					framebufferInfo.Attachments     = attachments.data()               ;
					framebufferInfo.Width           = SwapChain_Extent.Width           ;
					framebufferInfo.Height          = SwapChain_Extent.Height          ;
					framebufferInfo.Layers          = 1                                ;

					if (Heap(SwapChain_Framebuffers[index].Create(GetEngagedDevice(), framebufferInfo)) != EResult::Success) 
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
			void CreateGraphicsPipeline(StaticArray<ShaderModule, 2>& _shaders)
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

				auto binding = Vertex::GetBindingDescription();
				auto attributes = Vertex::GetAttributeDescriptions();

				vertexInputState_CreationSpec.VertexBindingDescriptionCount = 1;
				vertexInputState_CreationSpec.AttributeDescriptionCount = SCast<uint32>(attributes.size());

				vertexInputState_CreationSpec.BindingDescriptions = &binding;
				vertexInputState_CreationSpec.AttributeDescription = attributes.data();

				Pipeline::InputAssemblyState::CreateInfo inputAssembly_CreationSpec{};

				inputAssembly_CreationSpec.Topology               = EPrimitiveTopology::TriangleList;
				inputAssembly_CreationSpec.PrimitiveRestartEnable = false                           ;

				Viewport viewport{};

				viewport.X        = 0.0f                          ;
				viewport.Y        = 0.0f                          ;
				viewport.Width    = float(SwapChain_Extent.Width) ;
				viewport.Height   = float(SwapChain_Extent.Height);
				viewport.MinDepth = 0.0f                          ;
				viewport.MaxDepth = 1.0f                          ;

				Rect2D scissor{};

				scissor.Offset.X = 0;
				scissor.Offset.Y = 0;

				scissor.Extent = SwapChain_Extent;

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
				pipelineLayout_CreationSpec.SetLayouts             = DescriptorSetLayout;
				pipelineLayout_CreationSpec.PushConstantRangeCount = 0                  ;
				pipelineLayout_CreationSpec.PushConstantRanges     = nullptr            ;

				EResult piplineLayout_CreationResult = 
					Heap(PipelineLayout.Create(GetEngagedDevice(), pipelineLayout_CreationSpec));
					//Pipeline::Layout::Create(LogicalDevice.GetHandle(), pipelineLayout_CreationSpec, nullptr, PipelineLayout);


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

				EResult returnCode = Heap(GraphicsPipeline_Old.Create(GetEngagedDevice(), pipelineInfo));

				if (returnCode != EResult::Success) 
					throw std::runtime_error("Failed to create graphics pipeline!");
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
				Image&        _image      , 
				Memory&       _imageMemory
			)
			{
				Image::CreateInfo imageInfo {};

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

				if (Heap(_image.Create(GetEngagedDevice(), imageInfo)) != EResult::Success)
					throw RuntimeError("Failed to create image!");

				Memory::AllocateInfo allocationInfo {};

				auto& gpu = GetEngagedDevice().GetPhysicalDevice();

				allocationInfo.AllocationSize = _image.GetMemoryRequirements().Size;
				allocationInfo.MemoryTypeIndex = gpu.FindMemoryType(_image.GetMemoryRequirements().MemoryTypeBits, _properties);

				if (Heap(_imageMemory.Allocate(_image.GetMemoryRequirements(), _properties)) != EResult::Success)
					throw RuntimeError("Failed to allocate image memory!");

				_image.BindMemory(_imageMemory, 0);
			}

			ImageView CreateImageView(Image& _image, EFormat _format, Image::AspectFlags _aspectFlags, uint32 /*_miplevels*/)
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

				if (Heap(result.Create(GetEngagedDevice(), viewInfo)) != EResult::Success )
					throw RuntimeError("Failed to create texture image view!");

				return result;
			}

			void CreateSwapChain_ImageViews()
			{
				SwapChain_ImageViews.resize(SwapChain_Images.size());

				for (WordSize index = 0; index < SwapChain_Images.size(); index++)
				{
					SwapChain_ImageViews[index] = CreateImageView(SwapChain_Images[index], SwapChain_ImageFormat, Image::AspectFlags(EImageAspect::Color), MipMapLevels);
				}
			}

			void CreateIndexBuffer()
			{
				DeviceSize bufferSize = sizeof(ModelIndicies[0]) * ModelIndicies.size();

				Buffer             stagingBuffer       ;
				Buffer::CreateInfo stagingBufferInfo {};
				Memory             stagingBufferMemory ;

				stagingBufferInfo.SharingMode = ESharingMode::Exclusive;
				stagingBufferInfo.Size        = bufferSize            ; 

				stagingBufferInfo.Usage.Set(EBufferUsage::TransferSource);

				Heap
				(
					stagingBuffer.CreateAndBind
					(
						GetEngagedDevice(),
						stagingBufferInfo, 
						Memory::PropertyFlags(EMemoryPropertyFlag::HostVisible, EMemoryPropertyFlag::HostCoherent), 
						stagingBufferMemory
					);
				);
				

				// Hard coded vertex data...

				VoidPtr address = ModelIndicies.data();

				stagingBufferMemory.WriteToGPU(0, bufferSize, 0, address);

				Buffer::CreateInfo indexBufferInfo {};

				indexBufferInfo.SharingMode = ESharingMode::Exclusive;
				indexBufferInfo.Size        = bufferSize;

				indexBufferInfo.Usage.Set(EBufferUsage::TransferDestination, EBufferUsage::IndexBuffer);

				Heap
				(
					IndexBuffer.CreateAndBind
					(
						GetEngagedDevice(), 
						indexBufferInfo, 
						Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal), 
						IndexBufferMemory
					);
				);

				Buffer::CopyInfo copyInfo {}; copyInfo.DestinationOffset = 0; copyInfo.SourceOffset = 0; copyInfo.Size = bufferSize;

				Heap(SingleTimeCommandPool.CopyBuffer(stagingBuffer, IndexBuffer, copyInfo, GetEngagedDevice().GetGraphicsQueue()));

				stagingBuffer      .Destroy();
				stagingBufferMemory.Free();
			}

			void CreateRenderPass()
			{
				RenderPass::AttachmentDescription colorAttachment {};

				colorAttachment.Format  = SwapChain_ImageFormat;
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

				colorAttachmentResolveRef.Layout = EImageLayout::PresentSource_KHR;

				RenderPass::SubpassDescription subpass {};

				subpass.PipelineBindPoint      = EPipelineBindPoint::Graphics;
				subpass.ColorAttachmentCount   = 1                           ;
				subpass.ColorAttachments       = &colorAttachmentRef         ;
				subpass.DepthStencilAttachment = &depthAttachmentRef         ;
				//subpass.ResolveAttachments     = &colorAttachmentResolveRef  ;

				StaticArray<RenderPass::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

				RenderPass::CreateInfo renderPassInfo;

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

				if (Heap(RenderPass_Old.Create(GetEngagedDevice(), renderPassInfo) != EResult::Success))
				{
					throw std::runtime_error("failed to create render pass!");
				}
			}

			void CreateSyncObjects()
			{
				ImageAvailable_Semaphores.resize(MaxFramesInFlight);
				RenderFinished_Semaphores.resize(MaxFramesInFlight);

				InFlightFences.resize(MaxFramesInFlight      );
				ImagesInFlight.resize(SwapChain_Images.size());

				Semaphore::CreateInfo semaphore_CreationSpec;

				Fence::CreateInfo fence_CreationSpec;

				fence_CreationSpec.Flags.Set(EFenceCreateFlag::Signaled);

				for (WordSize index = 0; index < MaxFramesInFlight; index++)
				{
					EResult
				
					Heap
					(
						result = ImageAvailable_Semaphores[index].Create(GetEngagedDevice(), semaphore_CreationSpec);
						result = RenderFinished_Semaphores[index].Create(GetEngagedDevice(), semaphore_CreationSpec);
						result = InFlightFences           [index].Create(GetEngagedDevice(), fence_CreationSpec    );
					);

					if (result != EResult::Success)
						throw std::runtime_error("Failed to create synchronization objects for a frame!");
				}
			}

			void CreateTextureImage(RoCStr _filePath)
			{
				int textureWidth, textureHeight, textureChannels;

				using ImageBytes = stbi_uc;

				ImageBytes* imageData = Heap(stbi_load(_filePath, &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha));

				DeviceSize imageSize = textureWidth * textureHeight	 * 4;

				MipMapLevels = SCast<uint32>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1;

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

				Heap
				(
					stagingBuffer.CreateAndBind
					(
						GetEngagedDevice(),
						stagingBufferInfo,
						Memory::PropertyFlags(EMemoryPropertyFlag::HostVisible, EMemoryPropertyFlag::HostCoherent),
						stagingBufferMemory
					);
				);
				
				VoidPtr address = imageData;

				//Vault_2::Memory::WriteToGPU(LogicalDevice.GetHandle(), stagingBufferMemory, 0, imageSize, 0, address);

				stagingBufferMemory.WriteToGPU(0, imageSize, 0, address);

				Heap(stbi_image_free(imageData));

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

				CopyBufferToImage(stagingBuffer, TextureImage, SCast<uint32>(textureWidth), SCast<uint32>(textureHeight));

				GenerateMipMaps(TextureImage, EFormat::R8_G8_B8_A8_UNormalized, textureWidth, textureHeight, MipMapLevels);

				Heap
				(
					stagingBuffer.Destroy(); 
					stagingBufferMemory.Free();
				);
			}

			void CreateTextureImageView()
			{
				TextureImageView = CreateImageView(TextureImage, EFormat::R8_G8_B8_A8_UNormalized, Image::AspectFlags(EImageAspect::Color), MipMapLevels);
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
				samplerInfo.MaxLod     = SCast<float32>(MipMapLevels);

				if (Heap(TextureSampler.Create(GetEngagedDevice(), samplerInfo)) != EResult::Success)
					throw RuntimeError("Failed to create texture sampler!");
			}

			StaticArray<ShaderModule, 2> CreateTriShaders()
			{
				using Bytecode_Buffer = DynamicArray<Bytecode>;

				// Shader setup

				using namespace Renderer::Shader;

				auto triShader_VertCode = IO::BufferFile(String(Paths::TriangleShader) + "TriangleShader_Vert.spv");
				auto triShader_FragCode = IO::BufferFile(String(Paths::TriangleShader) + "TriangleShader_Frag.spv");

				//TODO: FIGURE THIS OUT.
				Bytecode_Buffer triShader_Vert_Bytecode(triShader_VertCode.begin(), triShader_VertCode.end());
				Bytecode_Buffer triShader_Frag_Bytecode(triShader_FragCode.begin(), triShader_FragCode.end());

				ShaderModule::CreateInfo vertInfo(triShader_VertCode.data(), triShader_VertCode.size());
				ShaderModule::CreateInfo fragInfo(triShader_FragCode.data(), triShader_FragCode.size());

				ShaderModule triShaderModule_Vert; triShaderModule_Vert.Create(GetEngagedDevice(), vertInfo);

				ShaderModule triShaderModule_Frag; triShaderModule_Frag.Create(GetEngagedDevice(), fragInfo);

				StaticArray<ShaderModule, 2> result = { triShaderModule_Vert, triShaderModule_Frag };

				return result;
			}

			void CreateUniformBuffers()
			{
				DeviceSize bufferSize = sizeof(UniformBufferObject);

				UniformBuffers      .resize(SwapChain_Images.size());
				UniformBuffersMemory.resize(SwapChain_Images.size());

				Buffer::CreateInfo uniformBufferInfo {};

				uniformBufferInfo.Size = bufferSize;

				uniformBufferInfo.Usage.Set(EBufferUsage::UniformBuffer);

				uniformBufferInfo.SharingMode = ESharingMode::Exclusive;

				for (WordSize index = 0; index < SwapChain_Images.size(); index++)
				{
					Heap
					(
						UniformBuffers[index].CreateAndBind
						(
							GetEngagedDevice(),
							uniformBufferInfo,
							Memory::PropertyFlags(EMemoryPropertyFlag::HostVisible, EMemoryPropertyFlag::HostCoherent),
							UniformBuffersMemory[index],
							Memory::DefaultAllocator
						);
					);
				}
			}

			StaticArray<ShaderModule, 2> Create_VKTut_Shaders()
			{
				using Bytecode_Buffer = DynamicArray<Bytecode>;

				// Shader setup

				using namespace Renderer::Shader;

				auto vertCode = IO::BufferFile(String(Paths::VKTut) + "VKTut_V5_Vert.spv");
				auto fragCode = IO::BufferFile(String(Paths::VKTut) + "VKTut_V5_Frag.spv");

				//TODO: FIGURE THIS OUT.
				Bytecode_Buffer vertBytecode(vertCode.begin(), vertCode.end());
				Bytecode_Buffer fragBytecode(fragCode.begin(), fragCode.end());

				ShaderModule vertShaderModule; ShaderModule::CreateInfo vertShaderInfo(vertCode.data(), vertCode.size());
				ShaderModule fragShaderModule; ShaderModule::CreateInfo fragShaderInfo(fragCode.data(), fragCode.size());

				vertShaderModule.Create(GetEngagedDevice(), vertShaderInfo);
				fragShaderModule.Create(GetEngagedDevice(), fragShaderInfo);
				
				StaticArray<ShaderModule, 2> result = { vertShaderModule, fragShaderModule };

				return result;
			}

			void CreateVertexBuffers(Buffer& _vertexBuffer, V3::Memory&_vertexBufferMemory)
			{
				DeviceSize bufferSize = sizeof(ModelVerticies[0]) * ModelVerticies.size();

				Buffer stagingBuffer;

				Memory stagingBufferMemory;

				Buffer::CreateInfo stagingBufferInfo {};

				stagingBufferInfo.SharingMode = ESharingMode::Exclusive;
				stagingBufferInfo.Size        = bufferSize;

				stagingBufferInfo.Usage.Set(EBufferUsage::TransferSource);

				Heap
				(
					stagingBuffer.CreateAndBind
					(
						GetEngagedDevice(),
						stagingBufferInfo,
						Memory::PropertyFlags(EMemoryPropertyFlag::HostVisible, EMemoryPropertyFlag::HostCoherent), 
						stagingBufferMemory
					);
				);
				
				// Hard coded vertex data...

				VoidPtr vertexData = ModelVerticies.data();

				Memory::MapFlags mapflags;

				stagingBufferMemory.WriteToGPU(0, bufferSize, mapflags, vertexData);

				Buffer::CreateInfo vertexBufferInfo {};

				vertexBufferInfo.Size        = bufferSize;
				vertexBufferInfo.SharingMode = ESharingMode::Exclusive;

				vertexBufferInfo.Usage.Set(EBufferUsage::TransferDestination, EBufferUsage::VertexBuffer);

				Heap
				(
					_vertexBuffer.CreateAndBind
					(
						GetEngagedDevice(), 
						vertexBufferInfo, 
						Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal), 
						_vertexBufferMemory
					);
				);

				Buffer::CopyInfo copyInfo {}; copyInfo.DestinationOffset = 0; copyInfo.SourceOffset = 0; copyInfo.Size = bufferSize;

				Heap(SingleTimeCommandPool.CopyBuffer(stagingBuffer, _vertexBuffer, copyInfo, GetEngagedDevice().GetGraphicsQueue()));

				Heap
				(
					stagingBuffer.Destroy();
					stagingBufferMemory.Free();
				);
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
					FormatProperties properties = GetEngagedDevice().GetPhysicalDevice().GetFormatProperties(format);

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

			void GenerateMipMaps(V3::Image _image, EFormat _format, uint32 _textureWidth, uint32 _textureHeight, uint32 _mipLevels)
			{
				EResult result = EResult::Incomplete;

				// Check if image format supports linear blitting
				FormatProperties formatProperties = GetEngagedDevice().GetPhysicalDevice().GetFormatProperties(_format);

				if (!(formatProperties.OptimalTilingFeatures.Has(EFormatFeatureFlag::SampledImageFilterLinear)))
				{
					throw std::runtime_error("Texture image format does not support linear blitting!");
				}

				CommandBuffer commandBuffer = SingleTimeCommandPool.BeginSingleTimeCommands(result);

				if (result != EResult::Success) throw RuntimeError("Could not begin single time commands for GenerateMipmaps.");

				Image::Memory_Barrier barrier{};

				barrier.Image               = _image             ;
				barrier.SrcQueueFamilyIndex = QueueFamily_Ignored;
				barrier.DstQueueFamilyIndex = QueueFamily_Ignored;

				barrier.SubresourceRange.AspectMask.Set(EImageAspect::Color);

				barrier.SubresourceRange.BaseArrayLayer = 0;
				barrier.SubresourceRange.LayerCount     = 1;
				barrier.SubresourceRange.LevelCount     = 1;

				sint32 mipWidth = _textureWidth;
				sint32 mipHeight = _textureHeight;

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

				result = SingleTimeCommandPool.EndSingleTimeCommands(commandBuffer, GetEngagedDevice().GetGraphicsQueue());

				if (result != EResult::Success) throw RuntimeError("Could not end command buffer commands properly: GenerateMipmaps");
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

				UniformBuffersMemory[_currentImage].WriteToGPU(0, sizeof(ubo), 0, address);
			}

			// GPU_HAL

			void Start_GPUComms(RoCStr _applicationName, AppVersion _applicationVersion)
			{
				CLog("Initializing GPU Communication");

				AppGPU_Comms_Initialize(_applicationName, _applicationVersion);

				AcquirePhysicalDevices();

				GenerateLogicalDevices();

				EngageMostSuitableDevice();

				PrepareDecks();
			}

			void Cease_GPUComms()
			{
				WipeDecks();

				AppGPU_Comms_Cease();	
			}

			void WaitFor_GPUIdle()
			{
				GetEngagedDevice().WaitUntilIdle();
			}
			
			ptr<ARenderContext> GetRenderContext(ptr<OSAL::Window> /* _window */)
			{
				return &RenderContext_Default;
			}

			void SetRenderContext()
			{
				RenderContext_Default.ApplicationInstance =  GetAppInstance();
				RenderContext_Default.PhysicalDevice      =  GetEngagedDevice().GetPhysicalDevice();
				RenderContext_Default.LogicalDevice       =  GetEngagedDevice()          ;
				RenderContext_Default.Queue               = GetEngagedDevice().GetGraphicsQueue();
				RenderContext_Default.PipelineCache       = PipelineCache           ;
				RenderContext_Default.ImageFormat         = SwapChain_ImageFormat   ;
				RenderContext_Default.FrameSize           = SwapChain_Extent        ;
				RenderContext_Default.Allocator           = Memory::DefaultAllocator;
				RenderContext_Default.RenderPass          = RenderPass_Old              ;
				RenderContext_Default.MinimumFrameBuffers = SwapChain_Old.GetMinimumImageCount();
				RenderContext_Default.FrameBufferCount    = SCast<uint32>(SwapChain_Images.size());
				RenderContext_Default.MSAA_Samples        = MSAA_Samples            ;
			}

			void Default_InitializeRenderer(ptr<Window> _window)
			{
				CLog("Doing dirty renderer initialization, clean this up!");

				Surface_Old = &Request_Surface(_window);

				Surface::Format format;

				format.Format     = EFormat::B8_G8_R8_A8_UNormalized;
				format.ColorSpace = EColorSpace::SRGB_NonLinear;

				auto swapchain =Request_SwapChain(*Surface_Old, format); 

				SwapChain_Old = swapchain;
				SwapChain_Extent = swapchain.GetExtent();
				SwapChain_ImageFormat = swapchain.GetFormat();
				SwapChain_Images = swapchain.GetImages();
				SwapChain_ImageViews = swapchain.GetImageViews();

				CreateRenderPass();

				CreateDescriptorSetLayout();

				StaticArray<ShaderModule, 2> shaders = Create_VKTut_Shaders();

				CreateGraphicsPipeline(shaders);

				for (auto shader : shaders)
				{
					shader.Destroy();
				}

				CreateFrameObjects();

				CreateColorResources();

				CreateDepthResources();

				CreateFrameBuffers();

				CreateSyncObjects();

				// Hard coded model setup

				LoadModel(VikingRoom_ModelPath);

				CreateVertexBuffers(VertexBuffer_Old, VertexBufferMemory);

				CreateIndexBuffer();

				CreateUniformBuffers();

				CreateTextureImage(VikingRoom_TexturePath.c_str()); 

				CreateTextureImageView();

				//

				CreateTextureSampler();

				CreateDescriptorPool();

				CreateDescriptorSets();

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

				GetEngagedDevice().GetGraphicsQueue().WaitUntilIdle();

				CleanupSwapChain();

				Surface_Old->RequeryCapabilities();

				Surface::Format format;

				format.Format = EFormat::B8_G8_R8_A8_UNormalized;
				format.ColorSpace = EColorSpace::SRGB_NonLinear;

				auto swapchain = Request_SwapChain(*Surface_Old, format);

				SwapChain_Old = swapchain;
				SwapChain_Extent = swapchain.GetExtent();
				SwapChain_ImageFormat = swapchain.GetFormat();
				SwapChain_Images = swapchain.GetImages();
				SwapChain_ImageViews = swapchain.GetImageViews();

				CreateSwapChain_ImageViews();

				CreateRenderPass();

				StaticArray<ShaderModule, 2> shaders = Create_VKTut_Shaders();

				CreateGraphicsPipeline(shaders);

				for (auto shader : shaders)
				{
					shader.Destroy();
				}

				CreateColorResources();

				CreateDepthResources();

				CreateFrameBuffers();

				CreateUniformBuffers();

				CreateDescriptorPool();

				CreateDescriptorSets();
			}

			DynamicArray<CommandBuffer::Handle> CommandBuffersToSubmit;

			void Default_DrawFrame(ptr<Window> _window)
			{
				InFlightFences[CurrentFrame].WaitFor(UInt64Max);


				uint32 imageIndex;

				EResult result = 
					SwapChain_Old.AcquireNextImage
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

				if (ImagesInFlight[imageIndex] != Null<Fence::Handle>) 
					ImagesInFlight[imageIndex].WaitFor(UInt64Max);

				ImagesInFlight[imageIndex] = InFlightFences[CurrentFrame];


				// Updating

				CommandPools_Old[imageIndex].Reset(0);

				RecordToBuffers(imageIndex);

				UpdateUniformBuffers(imageIndex);

				// Submit to Graphics Queue

				CommandBuffer::SubmitInfo submitInfo {};

				Semaphore::Handle waitSemaphores[] = { ImageAvailable_Semaphores[CurrentFrame] };

				Pipeline::StageFlags waitStages[1] {};

				waitStages[0].Set(EPipelineStageFlag::ColorAttachmentOutput);

				CommandBuffersToSubmit.push_back(CommandBuffers_Old[CurrentFrame].operator CommandBuffer::Handle());

				submitInfo.WaitSemaphoreCount = 1             ;
				submitInfo.WaitSemaphores     = waitSemaphores;
				submitInfo.WaitDstStageMask   = waitStages    ;

				submitInfo.CommandBufferCount = 1;
				submitInfo.CommandBuffers     = CommandBuffers_Old[imageIndex];// CommandBuffersToSubmit.data();


				Semaphore::Handle signalSemaphores[] = { RenderFinished_Semaphores[CurrentFrame] };

				submitInfo.SignalSemaphoreCount = 1               ;
				submitInfo.SignalSemaphores     = signalSemaphores;

				InFlightFences[CurrentFrame].Reset();

				if (GetGraphicsQueue().SubmitToQueue(1, submitInfo, InFlightFences[CurrentFrame]) != EResult::Success) 
					throw std::runtime_error("Failed to submit draw command buffer!");

				CommandBuffersToSubmit.clear();

				// Submit to presentation

				Swapchain::PresentationInfo presentInfo{};

				presentInfo.WaitSemaphoreCount = 1                    ;
				presentInfo.WaitSemaphores     = signalSemaphores     ;


				Swapchain::Handle swapChains[] = { SwapChain_Old };

				presentInfo.SwapchainCount = 1          ;
				presentInfo.Swapchains     = swapChains ;
				presentInfo.ImageIndices   = &imageIndex;

				presentInfo.Results = nullptr; // Optional

				result = GetEngagedDevice().GetGraphicsQueue().QueuePresentation(presentInfo);

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

					DescriptorSetLayout.Destroy();

					IndexBuffer      .Destroy();
					IndexBufferMemory.Free   ();

					VertexBuffer_Old      .Destroy();
					VertexBufferMemory.Free   ();

					for (WordSize index = 0; index < MaxFramesInFlight; index++) 
					{
						RenderFinished_Semaphores[index].Destroy();
						ImageAvailable_Semaphores[index].Destroy();
						InFlightFences           [index].Destroy();
					}

					SingleTimeCommandPool.Destroy();   

					for (auto pool : CommandPools_Old)
					{
						pool.Destroy();
					}

					//if (Meta::Vulkan::EnableLayers) GPU_Messenger.Destroy();

					Retire_Surface(Surface_Old);
				//);
			}
		}
	}

#endif
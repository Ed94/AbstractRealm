// Parent Header
#include "GPUVK_Renderer.hpp"



namespace HAL::GPU::Vulkan
{
#pragma region Surface

	EResult Surface::Create(ptr<OSAL::Window> _window)
	{
		AssignPhysicalDevice(GetEngagedPhysicalGPU());

		EResult result = Parent::Create(AppGPU_Comms, OSAL::GetOS_WindowHandle(_window));

		if (result != EResult::Success) return result;

		Bool presentationSupport = false; 

		CheckPhysicalDeviceSupport(GetGraphicsQueue().GetFamilyIndex(), presentationSupport);

		if (!presentationSupport) return EResult::Incomplete;

		window = _window;

		GetSupportedPresentationModes(presentationModes);

		GetAvailableFormats(formats);

		GetPhysicalDeviceCapabilities(capabilities);

		return result;
	}

	bool Surface::IsFormatAvailable(const Format& _format) const
	{
		for (auto& format : formats)
		{
			if (format.Format == _format.Format && format.ColorSpace == _format.ColorSpace)
			{
				return true;
			}
		}

		return false;
	}

	const Surface::Capabilities& Surface::GetCapabilities() const
	{
		return capabilities;
	}

	Surface::Format Surface::GetPreferredFormat() const
	{
		return formats[0];
	}

	const DynamicArray<EPresentationMode>& Surface::GetPresentationModes() const
	{
		return presentationModes;
	}

	ptr<OSAL::Window> Surface::GetWindow() const
	{
		return window;
	}

	bool Surface::RequeryCapabilities() 
	{
		Capabilities queryResult;

		GetPhysicalDeviceCapabilities(queryResult);

		if (queryResult != capabilities)
		{
			capabilities = queryResult;

			return true;
		}
		else
		{
			return false;
		}
	}

#pragma endregion Surface

#pragma region SwapChain

	// Public	

	EResult SwapChain::Create(Surface& _surface, Surface::Format _format)
	{
		surface = &_surface;

		info.Surface = _surface.GetHandle();

		const auto& presentationModes = _surface.GetPresentationModes();

		if (std::find(presentationModes.begin(), presentationModes.end(), EPresentationMode::Mailbox) != presentationModes.end())
		{
			info.PresentationMode = EPresentationMode::Mailbox;
		}
		else
		{
			info.PresentationMode = EPresentationMode::FIFO;
		}

		info.MinImageCount = _surface.GetCapabilities().MinImageCount;
		info.MinImageCount += 1;   // Want one more image than minimum...

		if (_surface.GetCapabilities().MaxImageCount > 0 && info.MinImageCount > _surface.GetCapabilities().MaxImageCount)
		{
			info.MinImageCount = _surface.GetCapabilities().MaxImageCount;
		}

		if (_surface.IsFormatAvailable(_format))
		{
			info.ImageFormat     = _format.Format    ;
			info.ImageColorSpace = _format.ColorSpace;
		}
		else
		{
			return EResult::Error_FormatNotSupported;
		}

		if (_surface.GetCapabilities().CurrentExtent.Width != UInt32Max)
			info.ImageExtent = _surface.GetCapabilities().CurrentExtent;
		else
		{
			OSAL::FrameBufferDimensions frameBufferSize = OSAL::GetFramebufferDimensions(_surface.GetWindow());

			Extent2D actualExtent;

			actualExtent.Width  = SCast<uint32>(frameBufferSize.Width );
			actualExtent.Height = SCast<uint32>(frameBufferSize.Height);

			actualExtent.Width  = std::clamp(actualExtent.Width , _surface.GetCapabilities().MinImageExtent.Width , _surface.GetCapabilities().MaxImageExtent.Width );
			actualExtent.Height = std::clamp(actualExtent.Height, _surface.GetCapabilities().MinImageExtent.Height, _surface.GetCapabilities().MaxImageExtent.Height);

			info.ImageExtent = actualExtent;
		}

		// Change to transfer destination if not rendering directly...
		info.ImageUsage.Set(EImageUsage::Color_Attachment);

		info.ImageArrayLayers = 1;   // If your supporting 3d rendering change this.

		// TODO: Add this back if logical device supports separate presentation queue.
		/*
		uint32 queueFamilyIndices[] = 
		{
			GetEngagedDevice().GetGraphicsQueue().GetFamilyIndex()
		};

		if (GetEngadedDevice().GetGraphicsQueue().GetFamilyIndex() != QueueIndices.PresentationSupported)
		{
			creationSpec.ImageSharingMode      = ESharingMode::Concurrent;
			creationSpec.QueueFamilyIndexCount = 2                       ;
			creationSpec.QueueFamilyIndices    = queueFamilyIndices      ;
		}
		else 
		{
			creationSpec.ImageSharingMode      = ESharingMode::Exclusive;
			creationSpec.QueueFamilyIndexCount = 0                      ; // Optional
			creationSpec.QueueFamilyIndices    = nullptr                ; // Optional
		}
		*/

		info.ImageSharingMode      = ESharingMode::Exclusive;
		info.QueueFamilyIndexCount = 0                      ; // Optional
		info.QueueFamilyIndices    = nullptr                ; // Optional

		info.PreTransform     = _surface.GetCapabilities().CurrentTransform;
		info.CompositeAlpha   = ECompositeAlpha::Opaque                    ;   // Swapchain hard coded to only support opaque surfaces.
		info.Clipped          = true                                       ;
		info.OldSwapchain     = Null<SwapChain::Handle>                    ;

		EResult result = Heap(Parent::Create(GetEngagedDevice().GetHandle(), info));

		if (result != EResult::Success) return result;

		result = RetrieveImages();

		if (result != EResult::Success) return result;

		result = GenerateViews();

		return result;
	}

	Extent2D SwapChain::GetExtent() const
	{
		return info.ImageExtent;
	}

	EFormat SwapChain::GetFormat() const
	{
		return info.ImageFormat;
	}

	bool SwapChain::QuerySurfaceChanges()
	{
		if (surface->RequeryCapabilities())
		{
			GetEngagedDevice().GetGraphicsQueue().WaitUntilIdle();	

			Regenerate();

			return true;
		}
		else
		{
			return false;
		}
	}

	const DynamicArray<Image>& SwapChain::GetImages() const
	{
		return images;
	}

	const DynamicArray<ImageView>& SwapChain::GetImageViews() const
	{
		return imageViews;
	}


	// Protected

	void SwapChain::Regenerate()
	{
		for (auto& imageView : imageViews)
		{
			imageView.Destroy();
		}

		Destroy();

		const auto& presentationModes = surface->GetPresentationModes();

		if (std::find(presentationModes.begin(), presentationModes.end(), EPresentationMode::Mailbox) != presentationModes.end())
		{
			info.PresentationMode = EPresentationMode::Mailbox;
		}
		else
		{
			info.PresentationMode = EPresentationMode::FIFO;
		}

		info.MinImageCount = surface->GetCapabilities().MinImageCount;
		info.MinImageCount += 1;   // Want one more image than minimum...

		if (surface->GetCapabilities().MaxImageCount > 0 && info.MinImageCount > surface->GetCapabilities().MaxImageCount)
		{
			info.MinImageCount = surface->GetCapabilities().MaxImageCount;
		}

		if (surface->GetCapabilities().CurrentExtent.Width != UInt32Max)
			info.ImageExtent = surface->GetCapabilities().CurrentExtent;
		else
		{
			OSAL::FrameBufferDimensions frameBufferSize = OSAL::GetFramebufferDimensions(surface->GetWindow());

			Extent2D actualExtent;

			actualExtent.Width  = SCast<uint32>(frameBufferSize.Width );
			actualExtent.Height = SCast<uint32>(frameBufferSize.Height);

			actualExtent.Width  = std::clamp(actualExtent.Width , surface->GetCapabilities().MinImageExtent.Width , surface->GetCapabilities().MaxImageExtent.Width );
			actualExtent.Height = std::clamp(actualExtent.Height, surface->GetCapabilities().MinImageExtent.Height, surface->GetCapabilities().MaxImageExtent.Height);

			info.ImageExtent = actualExtent;
		}

		// Change to transfer destination if not rendering directly...
		info.ImageUsage.Set(EImageUsage::Color_Attachment);

		info.ImageArrayLayers = 1;   // If your supporting 3d rendering change this.

		info.ImageSharingMode      = ESharingMode::Exclusive;
		info.QueueFamilyIndexCount = 0                      ; // Optional
		info.QueueFamilyIndices    = nullptr                ; // Optional

		info.PreTransform     = surface->GetCapabilities().CurrentTransform;
		info.CompositeAlpha   = ECompositeAlpha::Opaque                    ;   // Swapchain hard coded to only support opaque surfaces.
		info.Clipped          = true                                       ;
		info.OldSwapchain     = Null<SwapChain::Handle>                    ;

		EResult result = Heap(Parent::Create(GetEngagedDevice().GetHandle(), info));

		if (result != EResult::Success) throw RuntimeError("Unable to regenerate swapchain.");

		result = RetrieveImages();

		if (result != EResult::Success) throw RuntimeError("Unable to retrieve swapchain images after regeneration.");

		result = GenerateViews();

		if (result != EResult::Success) throw RuntimeError("Unable to generate new image views after regenerating swapchain.");
	}

	EResult SwapChain::GenerateViews()
	{
		ImageView::CreateInfo viewInfo {};

		viewInfo.Format   = info.ImageFormat         ;
		viewInfo.ViewType = ImageView::EViewType::_2D;

		viewInfo.SubresourceRange.AspectMask.Set(EImageAspect::Color);

		viewInfo.SubresourceRange.BaseMipLevel   = 0;
		viewInfo.SubresourceRange.LevelCount     = 1;
		viewInfo.SubresourceRange.BaseArrayLayer = 0;
		viewInfo.SubresourceRange.LayerCount     = 1;

		EResult result;

		imageViews.clear();

		for (auto& image : images)
		{
			viewInfo.Image = image.GetHandle();

			ImageView view;

			result = Heap(view.Create(GetEngagedDevice().GetHandle(), viewInfo));	

			if (result != EResult::Success)
				return result;

			imageViews.push_back(view);
		}

		return result;
	}

	EResult SwapChain::RetrieveImages()
	{
		uint32 numImages;

		EResult result = Parent::QueryImages(numImages, nullptr);

		if (result != EResult::Success) return result;

		if (images.size() != numImages)
		{
			images.resize(numImages);	
		}
		
		std::vector<Image::Handle> handles(numImages);

		result = Parent::QueryImages(numImages, handles.data());

		for (DeviceSize index = 0; index < numImages; index++)
		{
			images[index].Assign(device, handles[index]);
		}

		return result;
	}

#pragma endregion SwapChain

#pragma region FrameRenderer

	Fence& FrameRenderer::GetFence_RenderQueued()
	{
		return renderQueued;
	}

	Semaphore& FrameRenderer::GetSemaphore_SwapImageAcquired()
	{
		return swapImageAcquired;
	}

	EResult FrameRenderer::Prepare()
	{
		//frameBuffer = _frameBuffer;

		// Query threads assigned.

		// Allocate command pools for each thread / fences /semaphores...

		// For now I'm just assigning the GeneralPool.

		if (!Meta::UseConcurrency)
		{
			threadsAssigned = 0;

			Fence::CreateInfo fenceInfo;

			fenceInfo.Flags.Set(EFenceCreateFlag::Signaled);

			renderQueued.Create(GetEngagedDevice(), fenceInfo);
			
			
			Semaphore::CreateInfo semaphoreInfo;

			swapImageAcquired.Create(GetEngagedDevice(), semaphoreInfo);

			ptr<CommandPool> pools = RequestCommandPools(1);

			commandPools.push_back(pools);

			Primary_CmdBuffer = &commandPools[0]->RequestBuffer();
		}

		return EResult::Success;
	}

	/*void FrameRenderer::Record()
	{
		if (!Meta::UseConcurrency)
		{
			commandPools[0].get().Reset(0);
		}
	}*/

	const CommandBuffer& FrameRenderer::Request_PrimaryCmdBuffer()
	{
		return *Primary_CmdBuffer;
	}

	void FrameRenderer::ResetCommandPool()
	{
		commandPools[0]->Reset(0);
	}

	/*void FrameRenderer::Submit()
	{

	}*/

	void FrameRenderer::WaitFor_RenderQueued()
	{
		renderQueued.WaitFor(UInt64Max);
	}

#pragma endregion FrameRenderer

#pragma region RenderContext

	// Public

	EResult RenderContext::Create(SwapChain& _swapChain)
	{
		swapchain = &_swapChain;

		EResult result = CreateDepthBuffer();	

		if (result != EResult::Success) return result;

		result = CreateRenderPass();

		if (result != EResult::Success) return result;

		result = CreateFramebuffer();

		if (result != EResult::Success) return result;

		frameRenders.resize(frameBuffers.size());

		for (DataSize index = 0; index < frameRenders.size(); index++)
		{
			frameRenders[index].Prepare();
		}

		Semaphore::CreateInfo semaphoreInfo;

		frameSubmitedToPresentation.Create(GetEngagedDevice(), semaphoreInfo);

		swapImagesInFlight.resize(frameRenders.size());	

		Fence::CreateInfo fenceInfo; fenceInfo.Flags.Set(EFenceCreateFlag::Signaled);

		for (auto& fence : swapImagesInFlight)
		{
			fence.Create(GetEngagedDevice(), fenceInfo);
		}

		return result;
	}

	void RenderContext::ProcessNextFrame()
	{
		// Not sure if necessary: Make sure next frame to process has been processed by the GPU.
		frameRenders[currentFrame].WaitFor_RenderQueued();

 		CheckContext();

		// Get the next available image from the swapchain to render to.
		EResult result = swapchain->AcquireNextImage
		(
			UInt64Max,
			frameRenders[currentFrame].GetSemaphore_SwapImageAcquired(),
			Null<Fence::Handle>                                        ,
			currentSwapImage
		);

		// Make sure that the swapchain is still ok to render to.
		if (result == EResult::Suboptimal_KHR || result == EResult::Error_OutOfDate_KHR)
			CheckContext();

		// Make sure that the acquired image has been processed by the presentation queue.
		if (swapImagesInFlight[currentSwapImage].GetHandle() == Null<Fence::Handle>)
			swapImagesInFlight[currentSwapImage].WaitFor(UInt64Max);

		// Set the current swap image fence to that of the frame queue fence.
		swapImagesInFlight[currentSwapImage] = frameRenders[currentFrame].GetFence_RenderQueued();

		// Prep the ClearValue for the render pass...
		switch (currentFrame)    
		{ 
			case 0: 
			{
				clearValues[0].Color = { 1.0f,0.0f,0.0f,1.0f };

				break;
			}
			case 1: 
			{
				clearValues[0].Color = { 0.0f, 1.0f,0.0f,1.0f };

				break;
			}
			case 2: 
			{
				clearValues[0].Color = { 0.0f,0.0f,1.0f,1.0f };
			 
				break;
			}
			default:
			{
				clearValues[0].Color = { 1.0f,0.0f,0.0f, 1.0f };

				break;    
			}
		}

		if (bufferDepth) 
		{
			clearValues[1].DepthStencil.Depth   = 1.0f;
			clearValues[1].DepthStencil.Stencil = 0;
		}

		beginInfo.Framebuffer = frameBuffers[currentFrame];

		if (!Meta::UseConcurrency)
		{
			frameRenders[currentFrame].ResetCommandPool();

			auto& primaryBuffer = frameRenders[currentFrame].Request_PrimaryCmdBuffer();

			CommandBuffer::BeginInfo cmdBeginInfo;

			cmdBeginInfo.InheritanceInfo = nullptr;

			// Begin Recording

			primaryBuffer.BeginRecord(cmdBeginInfo);

			// Begin the render pass

			primaryBuffer.BeginRenderPass(beginInfo, ESubpassContents::Inline);


			// right now no render groups are setup properly... (or renderables for that matter...)

			//for (auto& renderGroup : renderGroups)
			//{
			//	// Using secondary buffers ?

			//	primaryBuffer.BindPipeline(EPipelineBindPoint::Graphics, *renderGroup.Pipeline);	

			//	for (auto& renderable : renderGroup.Renderables)
			//	{
			//		renderable->RecordRender(primaryBuffer);
			//	}
			//}

			primaryBuffer.EndRenderPass();

			// End the render pass

			// rest of render passes + its pipeline related objs

			// End Recording...

			primaryBuffer.EndRecord();

			CommandBuffer::SubmitInfo submitInfo;

			Pipeline::StageFlags waitStage(EPipelineStageFlag::ColorAttachmentOutput);

			submitInfo.WaitSemaphoreCount = 1;
			submitInfo.WaitSemaphores     = &frameRenders[currentFrame].GetSemaphore_SwapImageAcquired().GetHandle();
			submitInfo.WaitDstStageMask   = &waitStage;

			submitInfo.CommandBufferCount = 1;
			submitInfo.CommandBuffers     = &frameRenders[currentFrame].Request_PrimaryCmdBuffer().GetHandle();

			submitInfo.SignalSemaphoreCount = 1;
			submitInfo.SignalSemaphores  = &frameSubmitedToPresentation.GetHandle();

			frameRenders[currentFrame].GetFence_RenderQueued().Reset();

			result = GetGraphicsQueue().SubmitToQueue(1, submitInfo, frameRenders[currentFrame].GetFence_RenderQueued());

			if (result != EResult::Success)
			{
				throw RuntimeError("Unable to submit frame to graphics queue");
			}

			//frameRenders[currentFrame].Record();
		
			//frameRenders[currentFrame].Submit();
		}
		else
		{
			// For each thread

			// Reset designated command pool

			// Begin recording on the primary command buffer

			// For each render pass (primary command buffer)

			// For each renderGroup 

			// Give the thread a job
		}
	}

	void RenderContext::SubmitFrameToPresentation()
	{
		SwapChain::PresentationInfo presentInfo;

		presentInfo.WaitSemaphoreCount = 1;
		presentInfo.WaitSemaphores = &frameSubmitedToPresentation.GetHandle();
		//presentInfo.WaitSemaphoreCount = 0;
		//presentInfo.WaitSemaphores = nullptr;

		presentInfo.SwapchainCount = 1;
		presentInfo.Swapchains     = &swapchain->GetHandle();
		presentInfo.ImageIndices   = &currentSwapImage;

		presentInfo.Results = nullptr;

		EResult result = GetGraphicsQueue().QueuePresentation(dref(presentInfo.operator const VkPresentInfoKHR*()));

		if (result == EResult::Error_OutOfDate_KHR || result == EResult::Suboptimal_KHR)
			CheckContext();
		/*else if (result != EResult::Success)
		{
			throw RuntimeError("Not able to render frame and was not due to outofdate or suboptimal...");
		}*/

		previousFrame = currentFrame;

		currentFrame = (currentFrame + 1) % SCast<uint32>(frameRenders.size());
	}


	// Protected

	void RenderContext::CheckContext()
	{
		// Make sure swapchain is ok first...
		if (swapchain->QuerySurfaceChanges())
		{
			for (auto& framebuffer : frameBuffers)
			{
				framebuffer.Destroy();
			}

			renderPass.Destroy();

			EResult result = CreateDepthBuffer();

			if (result != EResult::Success) throw RuntimeError("Failed to recreate depth buffer in context.");

			result = CreateRenderPass();

			if (result != EResult::Success) throw RuntimeError("Failed to recreate render pass in context.");

			result = CreateFramebuffer();

			if (result != EResult::Success) throw RuntimeError("Failed to recreate frame renderer in context.");
		}
	}

	EResult RenderContext::CreateDepthBuffer()
	{
		Image::CreateInfo imgInfo {};

		imgInfo.ImageType = EImageType::_2D;

		GetEngagedPhysicalGPU().FindSupportedFormat
		(
			{ EFormat::D32_SFloat, EFormat::D32_SFloat_S8_UInt, EFormat::D24_UNormalized_S8_UInt },
			EImageTiling::Optimal,
			EFormatFeatureFlag::DepthStencilAttachment, 

			imgInfo.Format
		);

		imgInfo.Extent.Width  = swapchain->GetExtent().Width ;
		imgInfo.Extent.Height = swapchain->GetExtent().Height;
		imgInfo.Extent.Depth  = 1                             ;
		imgInfo.Tiling        = EImageTiling::Optimal         ;

		imgInfo.ArrayLayers  = 1                      ;
		imgInfo.InitalLayout = EImageLayout::Undefined;
		imgInfo.MipmapLevels = 1                      ;
		imgInfo.Samples      = samples                ;
		imgInfo.SharingMode  = ESharingMode::Exclusive;

		imgInfo.QueueFamilyIndexCount = 0   ;
		imgInfo.QueueFamilyIndices    = NULL;

		imgInfo.Usage.Set(EImageUsage::DepthStencil_Attachment);

		EResult result = Heap(depthBuffer.image.CreateAndBind
		(
			GetEngagedDevice(), 
			imgInfo, 
			Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal), 
			depthBuffer.memory
		));

		if (result != EResult::Success) return result;

		depthBuffer.image.TransitionLayout(EImageLayout::Undefined, EImageLayout::DepthStencil_AttachmentOptimal);

		ImageView::CreateInfo viewInfo {};

		viewInfo.Image    = depthBuffer.image;
		viewInfo.Format   = imgInfo.Format;
		viewInfo.ViewType = ImageView::EViewType::_2D;

		viewInfo.SubresourceRange.AspectMask.Set(EImageAspect::Depth);   // TODO: Make sure this is ok for later (stencil...)

		viewInfo.SubresourceRange.BaseMipLevel   = 0;
		viewInfo.SubresourceRange.LevelCount     = 1;  //_miplevels  ;
		viewInfo.SubresourceRange.BaseArrayLayer = 0;
		viewInfo.SubresourceRange.LayerCount     = 1;

		result = Heap(depthBuffer.view.Create(GetEngagedDevice().GetHandle(), viewInfo));

		return result;
	}

	EResult RenderContext::CreateFramebuffer()
	{
		EResult result = EResult::Incomplete;

		DynamicArray<ImageView::Handle> viewHandles;

		viewHandles.resize(renderPass.GetAttachmentCount());

		Framebuffer::CreateInfo info;

		info.RenderPass = renderPass;
		info.AttachmentCount = SCast<uint32>(viewHandles.size());
		info.Attachments = viewHandles.data();
		info.Width = swapchain->GetExtent().Width;
		info.Height = swapchain->GetExtent().Height;
		info.Layers = 1;

		if (bufferDepth)
		{
			viewHandles[1] = depthBuffer.view;
		}

		frameBuffers.clear();

		for (auto& swapViews : swapchain->GetImageViews())
		{
			viewHandles[0] = swapViews;

			Framebuffer framebuffer;

			result = framebuffer.Create(GetEngagedDevice().GetHandle(), info);

			frameBuffers.push_back(framebuffer);

			if (result != EResult::Success) return result;
		}


		return result;
	}

	EResult RenderContext::CreateRenderPass()
	{
		RenderPass::CreateInfo info {};

		DynamicArray<RenderPass::AttachmentDescription> attachments;

		RenderPass::AttachmentDescription colorAttachment;
		RenderPass::AttachmentDescription depthAttachment;

		colorAttachment.Format  = swapchain->GetFormat();
		colorAttachment.Samples = samples;

		colorAttachment.LoadOp  = shouldClear ? EAttachmentLoadOperation::Clear : EAttachmentLoadOperation::DontCare;
		colorAttachment.StoreOp = EAttachmentStoreOperation::Store;

		colorAttachment.StencilLoadOp  = EAttachmentLoadOperation::DontCare;
		colorAttachment.StencilStoreOp = EAttachmentStoreOperation::DontCare;

		//colorAttachment.InitialLayout = EImageLayout::Color_AttachmentOptimal;
		//colorAttachment.FinalLayout   = EImageLayout::Color_AttachmentOptimal;
		colorAttachment.InitialLayout = EImageLayout::Undefined;
		colorAttachment.FinalLayout   = EImageLayout::PresentSource_KHR;

		attachments.push_back(colorAttachment);

		if (bufferDepth)
		{
			depthAttachment.Format = depthBuffer.GetFormat();
			depthAttachment.Samples = samples;

			depthAttachment.LoadOp = shouldClear ? EAttachmentLoadOperation::Clear : EAttachmentLoadOperation::DontCare;
			depthAttachment.StoreOp = EAttachmentStoreOperation::Store;

			depthAttachment.StencilLoadOp = EAttachmentLoadOperation::Load;
			depthAttachment.StencilStoreOp = EAttachmentStoreOperation::Store;

			depthAttachment.InitialLayout = EImageLayout::DepthStencil_AttachmentOptimal;
			depthAttachment.FinalLayout = EImageLayout::DepthStencil_AttachmentOptimal;

			attachments.push_back(depthAttachment);
		}

		RenderPass::AttachmentReference colorReference;
		RenderPass::AttachmentReference depthReference;

		colorReference.Attachment = 0;
		colorReference.Layout     = EImageLayout::Color_AttachmentOptimal;

		if (bufferDepth) 
		{
			depthReference.Attachment = 1;
			depthReference.Layout     = EImageLayout::DepthStencil_AttachmentOptimal;
		}

		RenderPass::SubpassDescription subpass;

		subpass.PipelineBindPoint = EPipelineBindPoint::Graphics;

		subpass.ColorAttachmentCount = 1;
		subpass.ColorAttachments = &colorReference;

		subpass.DepthStencilAttachment = bufferDepth ? &depthReference : nullptr;

		info.AttachmentCount = SCast<uint32>(attachments.size());
		info.Attachments = attachments.data();

		info.SubpassCount = 1;
		info.Subpasses = &subpass;

		info.DependencyCount = 0;
		info.Dependencies = nullptr;

		EResult result = renderPass.Create(GetEngagedDevice().GetHandle(), info);

		if (result != EResult::Success) return result;

		beginInfo.RenderPass = renderPass;

		beginInfo.RenderArea.Offset.X = 0;
		beginInfo.RenderArea.Offset.Y = 0;

		beginInfo.RenderArea.Extent = swapchain->GetExtent();

		if (shouldClear)
		{
			clearValues.resize(renderPass.GetAttachmentCount());

			beginInfo.ClearValues = clearValues.data();
		}

		beginInfo.ClearValueCount = SCast<uint32>(clearValues.size());

		return result;
	}

#pragma endregion RenderContext



	Deque<Surface> Surfaces;

	Deque<SwapChain> SwapChains;

	Deque<RenderContext> RenderContexts;

	void DeinitalizeRenderer()
	{
		for (auto& swapchain : SwapChains)
		{
			Heap(swapchain.Destroy());
		}

		for (auto& surface : Surfaces)
		{
			Heap(surface.Destroy());
		}

		for (auto& renderContext : RenderContexts)
		{

		}
	}

	Surface& CreateSurface(ptr<OSAL::Window> _window)
	{
		Surface surface;

		surface.AssignPhysicalDevice(GetEngagedPhysicalGPU());

		if (Heap(surface.Create(_window) != EResult::Success))
		{
			throw RuntimeError("Failed to create surface for targeted window.");
		}

		Surfaces.push_back(surface);

		return Surfaces.back();
	}

	SwapChain& CreateSwapChain(Surface& _surface, Surface::Format _formatDesired)
	{
		SwapChain swapchain;

		SwapChain::CreateInfo info{};

		EResult result = swapchain.Create(_surface, _formatDesired);

		if (result != EResult::Success)
		{
			throw std::runtime_error("Failed to create the swap chain!");
		}

		SwapChains.push_back(swapchain);

		return SwapChains.back();
	}

	const RenderContext& CreateRenderContext(SwapChain& _swapchain)
	{
		RenderContext context;

		EResult result = context.Create(_swapchain);

		if (result != EResult::Success) throw RuntimeError("Failed to create render context");	

		RenderContexts.push_back(move(context));

		return RenderContexts.back();
	}

	void InitalizeRenderer()
	{
	}

	void Renderer_Update()
	{
		for (auto& renderContext : RenderContexts)
		{
			renderContext.ProcessNextFrame();

			renderContext.SubmitFrameToPresentation();
		}
	}
}
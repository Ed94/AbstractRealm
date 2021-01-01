// Parent Header
#include "GPUVK_Renderer.hpp"



namespace HAL::GPU::Vulkan
{
#pragma region Surface

	EResult Surface::Create(ptr<OSAL::Window> _window)
	{
		AssignPhysicalDevice(GetEngagedPhysicalGPU());

		EResult result = Parent::Create(GetAppInstance(), OSAL::GetOS_WindowHandle(_window));

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

		if (capabilities.operator!=(queryResult))
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

#pragma region Swapchain

	// Public	

	EResult Swapchain::Create(Surface& _surface, Surface::Format _format)
	{
		surface = &_surface;

		info.Surface = _surface;

		const auto& presentationModes = _surface.GetPresentationModes();

		EPresentationMode desiredPresentationMode = EPresentationMode::Mailbox;

		switch (presentationMode)
		{
			case Meta::EGPU_PresentMode::Immediate   : desiredPresentationMode = EPresentationMode::Immediate   ; break;
			case Meta::EGPU_PresentMode::FIFO        : desiredPresentationMode = EPresentationMode::FIFO        ; break;
			case Meta::EGPU_PresentMode::FIFO_Relaxed: desiredPresentationMode = EPresentationMode::FIFO_Relaxed; break;
			case Meta::EGPU_PresentMode::MailBox     : desiredPresentationMode = EPresentationMode::Mailbox     ; break;
		}

		switch (desiredPresentationMode)
		{
			case EPresentationMode::Immediate:
			{
				if (find(presentationModes.begin(), presentationModes.end(), EPresentationMode::Immediate) != presentationModes.end())
					info.PresentationMode = EPresentationMode::Immediate;

				break;
			}
			case EPresentationMode::FIFO_Relaxed:
			{
				if (find(presentationModes.begin(), presentationModes.end(), EPresentationMode::FIFO_Relaxed) != presentationModes.end())
					info.PresentationMode = EPresentationMode::FIFO_Relaxed;

				break;
			}
			case EPresentationMode::Mailbox:
			{
				if (find(presentationModes.begin(), presentationModes.end(), EPresentationMode::Mailbox) != presentationModes.end())
					info.PresentationMode = EPresentationMode::Mailbox;

				break;
			}
			default:
			{
				info.PresentationMode = EPresentationMode::FIFO;
			}
		}

		info.MinImageCount = _surface.GetCapabilities().MinImageCount;

		if (frameBuffering == Meta::EGPU_FrameBuffering::Triple)
		{
			info.MinImageCount += 1;   // Want one more image than minimum... (Triple buffering)
		}

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

		info.ImageSharingMode      = ESharingMode::Exclusive;
		info.QueueFamilyIndexCount = 0                      ; // Optional
		info.QueueFamilyIndices    = nullptr                ; // Optional

		info.PreTransform     = _surface.GetCapabilities().CurrentTransform;
		info.CompositeAlpha   = ECompositeAlpha::Opaque                    ;   // Swapchain hard coded to only support opaque surfaces.
		info.Clipped          = true                                       ;
		info.OldSwapchain     = Null<Swapchain::Handle>                    ;

		Heap() EResult result = Parent::Create(GetEngagedDevice(), info);

		if (result != EResult::Success) return result;

		result = RetrieveImages();

		if (result != EResult::Success) return result;

		result = GenerateViews();

		return result;
	}

	void Swapchain::Destroy()
	{
		for (auto& image : images)
		{
			image.Clear();
		}

		for (auto& imageView : imageViews)
		{
			imageView.Destroy();
		}

		Parent::Destroy();
	}

	Extent2D Swapchain::GetExtent() const
	{
		return info.ImageExtent;
	}

	EFormat Swapchain::GetFormat() const
	{
		return info.ImageFormat;
	}

	uint32 Swapchain::GetMinimumImageCount() const
	{
		return info.MinImageCount;
	}

	bool Swapchain::QuerySurfaceChanges()
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

	const DynamicArray<Image>& Swapchain::GetImages() const
	{
		return images;
	}

	const DynamicArray<ImageView>& Swapchain::GetImageViews() const
	{
		return imageViews;
	}


	// Protected

	void Swapchain::Regenerate()
	{
		Destroy();

		const auto& presentationModes = surface->GetPresentationModes();

		EPresentationMode desiredPresentationMode = EPresentationMode::Mailbox;

		switch (presentationMode)
		{
			case Meta::EGPU_PresentMode::Immediate   : desiredPresentationMode = EPresentationMode::Immediate   ; break;
			case Meta::EGPU_PresentMode::FIFO        : desiredPresentationMode = EPresentationMode::FIFO        ; break;
			case Meta::EGPU_PresentMode::FIFO_Relaxed: desiredPresentationMode = EPresentationMode::FIFO_Relaxed; break;
			case Meta::EGPU_PresentMode::MailBox     : desiredPresentationMode = EPresentationMode::Mailbox     ; break;
		}

		switch (desiredPresentationMode)
		{
			case EPresentationMode::Immediate:
			{
				if (find(presentationModes.begin(), presentationModes.end(), EPresentationMode::Immediate) != presentationModes.end())
					info.PresentationMode = EPresentationMode::Immediate;

				break;
			}
			case EPresentationMode::FIFO_Relaxed:
			{
				if (find(presentationModes.begin(), presentationModes.end(), EPresentationMode::FIFO_Relaxed) != presentationModes.end())
					info.PresentationMode = EPresentationMode::FIFO_Relaxed;

				break;
			}
			case EPresentationMode::Mailbox:
			{
				if (find(presentationModes.begin(), presentationModes.end(), EPresentationMode::Mailbox) != presentationModes.end())
					info.PresentationMode = EPresentationMode::Mailbox;

				break;
			}
			default:
			{
				info.PresentationMode = EPresentationMode::FIFO;
			}
		}

		info.MinImageCount = surface->GetCapabilities().MinImageCount;

		if (frameBuffering == Meta::EGPU_FrameBuffering::Triple)
		{
			info.MinImageCount += 1;   // Want one more image than minimum... (Triple buffering)
		}

		if (surface->GetCapabilities().MaxImageCount > 0 && info.MinImageCount > surface->GetCapabilities().MaxImageCount)
		{
			info.MinImageCount = surface->GetCapabilities().MaxImageCount;
		}

		if (surface->GetCapabilities().CurrentExtent.Width != UInt32Max)
		{
			info.ImageExtent = surface->GetCapabilities().CurrentExtent;
		}
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
		info.OldSwapchain     = Null<Swapchain::Handle>                    ;

		Heap() EResult result = Parent::Create(GetEngagedDevice(), info);

		if (result != EResult::Success) throw RuntimeError("Unable to regenerate swapchain.");

		result = RetrieveImages();

		if (result != EResult::Success) throw RuntimeError("Unable to retrieve swapchain images after regeneration.");

		result = GenerateViews();

		if (result != EResult::Success) throw RuntimeError("Unable to generate new image views after regenerating swapchain.");
	}

	EResult Swapchain::GenerateViews()
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
			viewInfo.Image = image;

			ImageView view;

			Heap() result = view.Create(GetEngagedDevice(), viewInfo);	

			if (result != EResult::Success)
				return result;

			imageViews.push_back(move(view));
		}

		return result;
	}

	EResult Swapchain::RetrieveImages()
	{
		uint32 numImages;

		EResult result = Parent::QueryImages(numImages, nullptr);

		if (result != EResult::Success) return result;

		if (images.size() != numImages)
		{
			images.resize(numImages);	
		}
		
		DynamicArray<Image::Handle> handles(numImages);

		result = Parent::QueryImages(numImages, handles.data());

		for (DeviceSize index = 0; index < numImages; index++)
		{
			images[index].Assign(*device, handles[index]);
		}

		return result;
	}

#pragma endregion Swapchain

#pragma region RenderPass

	EResult RenderPass::Create(const LogicalDevice& _device, CreateInfo& _info)
	{
		info = _info;

		return Parent::Create(_device, _info);
	}

	EResult RenderPass::Create(const LogicalDevice& _device, CreateInfo& _info, const Memory::AllocationCallbacks& _allocator)
	{
		info = _info;

		return Parent::Create(_device, _info, _allocator);
	}

	uint32 RenderPass::GetAttachmentCount() const { return info.AttachmentCount; }

#pragma endregion

#pragma region FrameReference

	Fence& FrameReference::RenderingInFlight()
	{
		return flightToRender;
	}

	Semaphore& FrameReference::SwapAcquisionStatus()
	{
		return swapAcquisitionStatus;
	}

	EResult FrameReference::Prepare()
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

			flightToRender.Create(GetEngagedDevice(), fenceInfo);

			
			Semaphore::CreateInfo semaphoreInfo;

			swapAcquisitionStatus.Create(GetEngagedDevice(), semaphoreInfo);

			presentSubmitStatus.Create(GetEngagedDevice(), semaphoreInfo);


			ptr<CommandPool> pools = RequestCommandPools(1);

			commandPools.push_back(pools);

			Primary_CmdBuffer = &commandPools[0]->RequestBuffer();
		}

		return EResult::Success;
	}

	const CommandBuffer& FrameReference::Request_PrimaryCmdBuffer()
	{
		return *Primary_CmdBuffer;
	}

	Semaphore& FrameReference::PresentSubmitStatus()
	{
		return presentSubmitStatus;
	}

	void FrameReference::ResetCommandPool()
	{
		commandPools[0]->Reset(0);
	}

	void FrameReference::WaitFor_RenderingInFlight()
	{
		flightToRender.WaitFor(UInt64Max);
	}

#pragma endregion FrameReference

#pragma region RenderContext

	// Public

	EResult RenderContext::Create(Swapchain& _swapChain)
	{
		swapchain = &_swapChain;

		EResult result = CreateDepthBuffer();	

		if (result != EResult::Success) return result;

		result = CreateRenderPass();

		if (result != EResult::Success) return result;

		result = CreateFramebuffers();

		if (result != EResult::Success) return result;

		maxFramesInFlight = SCast<uint32>(frameBuffers.size() - 1);

		frameRefs.resize(maxFramesInFlight);

		for (WordSize index = 0; index < frameRefs.size(); index++)
		{
			frameRefs[index].Prepare();
		}

		Semaphore::CreateInfo semaphoreInfo;

		presentSubmitStatus.Create(GetEngagedDevice(), semaphoreInfo);

		swapsInFlight.resize(frameBuffers.size());	

		Fence::CreateInfo fenceInfo; fenceInfo.Flags.Set(EFenceCreateFlag::Signaled);

		return result;
	}

	void RenderContext::Destroy()
	{
		for (auto& frameBuffer : frameBuffers)
		{
			frameBuffer.Destroy();
		}

		renderPass.Destroy();

		depthBuffer.image.Destroy();

		//depthBuffer.memory.Free();

		depthBuffer.view.Destroy();
	}

	void RenderContext::ProcessNextFrame()
	{
		auto& frameRef = frameRefs[currentFrameBuffer];

		// Make sure next frame to process has been processed by the GPU.
		frameRef.RenderingInFlight().WaitFor(UInt64Max);

		// Get the next available image from the swapchain to render to.
		EResult result = swapchain->AcquireNextImage
		(
			UInt64Max,
			frameRef.SwapAcquisionStatus(),
			Null<Fence::Handle>           ,
			currentSwap
		);

		// Make sure that the swapchain is still ok to render to.
		if (result == EResult::Error_OutOfDate_KHR)
		{
			CheckContext();

			return;
		}
		else if (result != EResult::Success && result != EResult::Suboptimal_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		// Make sure that the acquired image has been processed by the presentation queue.
		if (swapsInFlight[currentSwap] != nullptr && *swapsInFlight[currentSwap] != Null<Fence::Handle>)
			swapsInFlight[currentSwap]->WaitFor(UInt64Max);

		// Set the current swap image fence to that of the frame queue fence.
		swapsInFlight[currentSwap] = &frameRef.RenderingInFlight();

		// Prep the ClearValue for the render pass...
		switch (currentFrameBuffer)
		{ 
			case 0: clearValues[0].Color = { 1.0f, 0.0f, 0.0f, 1.0f }; break;
			case 1: clearValues[0].Color = { 0.0f, 1.0f, 0.0f, 1.0f }; break;
			case 2: clearValues[0].Color = { 0.0f, 0.0f, 1.0f, 1.0f }; break;
			default:
			{
				clearValues[0].Color = { 1.0f,0.0f,0.0f, 1.0f };

				break;    
			}
		}

		if (bufferDepth) 
		{
			clearValues[1].DepthStencil.Depth   = 1.0f;
			clearValues[1].DepthStencil.Stencil = 0   ;
		}

		beginInfo.Framebuffer = frameBuffers[currentSwap];

		if (!Meta::UseConcurrency)
		{
			frameRefs[currentFrameBuffer].ResetCommandPool();

			auto& primaryBuffer = frameRef.Request_PrimaryCmdBuffer();

			CommandBuffer::BeginInfo cmdBeginInfo;

			cmdBeginInfo.InheritanceInfo = nullptr;

			primaryBuffer.BeginRecord(cmdBeginInfo);

			primaryBuffer.BeginRenderPass(beginInfo, ESubpassContents::Inline);

			primaryBuffer.EndRenderPass();

			primaryBuffer.EndRecord();

			CommandBuffer::SubmitInfo submitInfo;

			Pipeline::StageFlags waitStage(EPipelineStageFlag::ColorAttachmentOutput);

			// Need to wait for swap image to be acquired before submitting render commands to queue.
			submitInfo.WaitSemaphoreCount = 1                             ;
			submitInfo.WaitSemaphores     = frameRef.SwapAcquisionStatus();
			submitInfo.WaitDstStageMask   = &waitStage                    ;

			submitInfo.CommandBufferCount = 1                                  ;
			submitInfo.CommandBuffers     = frameRef.Request_PrimaryCmdBuffer();

			// Let the presentation queue know that it can submit itself when render commands are finished in the queue.
			submitInfo.SignalSemaphoreCount = 1;
			submitInfo.SignalSemaphores     =  frameRef.PresentSubmitStatus();  // &frameSubmitedToPresentation.GetHandle();

			frameRef.RenderingInFlight().Reset();

			result = GetGraphicsQueue().SubmitToQueue(1, submitInfo, frameRef.RenderingInFlight());

			if (result != EResult::Success)
			{
				throw RuntimeError("Unable to submit frame to graphics queue");
			}
		}
		else
		{
			// For each thread

			// Reset designated command pool

			// Begin recording on the primary command buffer

			// For each render pass (primary command buffer)

			// For each renderGroup 

			// Give the thread a job (Request a secondary command buffer...)
		}
	}

	void RenderContext::SubmitFrameToPresentation()
	{
		auto& frameRef = frameRefs[currentFrameBuffer];

		static DynamicArray<Swapchain::Handle> swapchainsToSubmit;

		Swapchain::PresentationInfo presentInfo;

		// Wait to make sure that the frame to put into the swap for presentation has finished being processed.
		presentInfo.WaitSemaphoreCount = 1                             ;
		presentInfo.WaitSemaphores     = frameRef.PresentSubmitStatus();
		
		swapchainsToSubmit.push_back(swapchain->operator const Swapchain::Handle&());

		presentInfo.SwapchainCount = 1;
		presentInfo.Swapchains     = *swapchain;
		presentInfo.ImageIndices   = &currentSwap;

		presentInfo.Results = nullptr;

		EResult result = GetGraphicsQueue().QueuePresentation(presentInfo);

		if (result == EResult::Error_OutOfDate_KHR || result == EResult::Suboptimal_KHR)
		{
			CheckContext();
		}

		else if (result != EResult::Success)
		{
			throw RuntimeError("Not able to render frame and was not due to out-of-date or suboptimal...");
		}

		previousFrame = currentFrameBuffer;

		// Frames are numbers 0 to the number of frames buffered - 1 (to offset for initial index being 0)
		currentFrameBuffer = (currentFrameBuffer + 1) % maxFramesInFlight;
	}

	bool RenderContext::operator== (const RenderContext& _other)
	{
		return this == &_other;
	}


	// Protected

	void RenderContext::CheckContext()
	{
		// Make sure swapchain is ok first...
		if (swapchain->QuerySurfaceChanges())
		{
			Destroy();

			EResult result = CreateDepthBuffer();

			if (result != EResult::Success) throw RuntimeError("Failed to recreate depth buffer in context.");

			result = CreateRenderPass();

			if (result != EResult::Success) throw RuntimeError("Failed to recreate render pass in context.");

			result = CreateFramebuffers();

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

		Heap() 
		EResult result = depthBuffer.image.Create(GetEngagedDevice(), imgInfo);

		auto& memReq = depthBuffer.image.GetMemoryRequirements();

		Memory::AllocateInfo allocInfo;

		allocInfo.AllocationSize = memReq.Size;

		allocInfo.MemoryTypeIndex = 
			GetEngagedDevice().GetPhysicalDevice().FindMemoryType
			(memReq.MemoryTypeBits, Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal));

		depthBuffer.memory.Allocate(GetEngagedDevice(), allocInfo);

		depthBuffer.image.BindMemory(depthBuffer.memory, depthBuffer.memoryOffset);

		/*EResult result = depthBuffer.image.CreateAndBind
		(
			GetEngagedDevice(), 
			imgInfo, 
			Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal), 
			depthBuffer.memory
		);*/

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

		Heap() result = depthBuffer.view.Create(GetEngagedDevice(), viewInfo);

		return result;
	}

	EResult RenderContext::CreateFramebuffers()
	{
		EResult result = EResult::Incomplete;

		DynamicArray<ImageView::Handle> viewHandles;

		viewHandles.resize(renderPass.GetAttachmentCount());

		Framebuffer::CreateInfo info;

		info.RenderPass      = renderPass;
		info.AttachmentCount = SCast<uint32>(viewHandles.size());
		info.Attachments     = viewHandles.data();
		info.Width           = swapchain->GetExtent().Width;
		info.Height          = swapchain->GetExtent().Height;
		info.Layers          = 1;

		if (bufferDepth)
		{
			viewHandles[1] = depthBuffer.view;
		}

		frameBuffers.clear();

		frameBuffers.resize(swapchain->GetImageViews().size());

		auto& swapImageViews = swapchain->GetImageViews();

		for (WordSize index = 0; index < frameBuffers.size(); index++)
		{
			viewHandles[0] = swapImageViews[index];

			result = frameBuffers[index].Create(GetEngagedDevice(), info);

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
		colorAttachment.InitialLayout = EImageLayout::Undefined        ;
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
			depthAttachment.FinalLayout   = EImageLayout::DepthStencil_AttachmentOptimal;

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

		EResult result = renderPass.Create(GetEngagedDevice(), info);

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


	ESubmissionType SubmissionMode = ESubmissionType::Individual;

	Deque<Surface> Surfaces;

	Deque<Swapchain> Swapchains;

	Deque<RenderContext> RenderContexts;



	void Renderer_SetSubmissionMode(ESubmissionType _submissionBehaviorDesired)
	{
		SubmissionMode = _submissionBehaviorDesired;
	}

	void Renderer_Shutdown()
	{
		for (auto& swapchain : Swapchains)
		{
			Heap() swapchain.Destroy();
		}

		for (auto& surface : Surfaces)
		{
			Heap() surface.Destroy();
		}

		for (auto& renderContext : RenderContexts)
		{
			renderContext.Destroy();
		}
	}

	Surface& Request_Surface(ptr<OSAL::Window> _window)
	{
		Surfaces.resize(Surfaces.size() + 1);

		Surface& surface = Surfaces.back();

		surface.AssignPhysicalDevice(GetEngagedPhysicalGPU());

		Heap() if (surface.Create(_window) != EResult::Success)
		{
			throw RuntimeError("Failed to create surface for targeted window.");
		}

		return surface;
	}

	void Retire_Surface(ptr<Surface> _surface)
	{
		_surface->Destroy();

		Surfaces.erase(find(Surfaces.begin(), Surfaces.end(), *_surface));
	}

	Swapchain& Request_SwapChain(Surface& _surface, Surface::Format _formatDesired)
	{
		Swapchain::CreateInfo info;

		Swapchains.resize(Swapchains.size() + 1);

		Swapchain& swapchain = Swapchains.back();

		EResult result = swapchain.Create(_surface, _formatDesired);

		if (result != EResult::Success)
		{
			throw std::runtime_error("Failed to create the swap chain!");
		}

		return swapchain;
	}

	void Retire_Swapchain(ptr<Swapchain> _swapchain)
	{
		_swapchain->Destroy();

		Swapchains.erase(find(Swapchains.begin(), Swapchains.end(), *_swapchain));
	}

	RenderContext& Request_RenderContext(Swapchain& _swapchain)
	{
		RenderContexts.resize(RenderContexts.size() + 1);

		EResult result = RenderContexts.back().Create(_swapchain);

		if (result != EResult::Success) throw RuntimeError("Failed to create render context");	

		return RenderContexts.back();
	}

	void Retire_RenderContext(ptr<RenderContext> _renderContext)
	{
		_renderContext->Destroy();

		RenderContexts.erase(find(RenderContexts.begin(), RenderContexts.end(), *_renderContext));
	}

	void Renderer_Initalize()
	{
	}

	void Renderer_Update()
	{
		for (auto& renderContext : RenderContexts)
		{
			renderContext.ProcessNextFrame();

			//renderContext.SubmitFrameToPresentation();
		}
	}

	void Renderer_Present()
	{
		for (auto& renderContext : RenderContexts)
		{
			renderContext.SubmitFrameToPresentation();
		}
	}
}
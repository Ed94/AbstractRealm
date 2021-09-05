// Parent Header
#include "GPUVK_Rendering.hpp"


#include "Meta/EngineInfo.hpp"


namespace HAL::GPU::Vulkan
{
	void (*TestCallback)
	(
		int                   _index,
		const CommandBuffer&  _buffer,
		Framebuffer::Handle   _frameBuffer,
		ptr<const Swapchain>  _swap,
		RenderPass::BeginInfo _beginInfo
	);



	void SetTestCallback(void (*_recordToBuffers)
	(
		int                   _index,
		const CommandBuffer&  _buffer,
		Framebuffer::Handle   _frameBuffer,
		ptr<const Swapchain>  _swap,
		RenderPass::BeginInfo _beginInfo
	)
	)
	{
		TestCallback = _recordToBuffers;
	}

#pragma region Surface

	EResult Surface::Create(ptr<OSAL::Window> _window_in)
	{
		AssignPhysicalDevice(Comms::GetEngagedPhysicalGPU());

		EResult result = Parent::Create(Comms::GetAppInstance(), OSAL::GetOS_WindowHandle(_window_in));

		if (result != EResult::Success) 
			return result;

		Bool presentationSupport = false; 

		CheckPhysicalDeviceSupport(Comms::GetGraphicsQueue().GetFamilyIndex(), presentationSupport);

		if (!presentationSupport) 
			return EResult::Incomplete;

		window = _window_in;

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

	EResult Swapchain::Create(Surface& _surface_in, Surface::Format _format)
	{
		surface = getPtr(_surface_in);

		info.Surface = _surface_in;

		const auto& presentationModes = _surface_in.GetPresentationModes();

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

		info.MinImageCount = _surface_in.GetCapabilities().MinImageCount;

		if (frameBuffering == Meta::EGPU_FrameBuffering::Triple)
		{
			info.MinImageCount += 1;   // Want one more image than minimum... (Triple buffering)
		}

		if (_surface_in.GetCapabilities().MaxImageCount > 0 && info.MinImageCount > _surface_in.GetCapabilities().MaxImageCount)
		{
			info.MinImageCount = _surface_in.GetCapabilities().MaxImageCount;
		}

		if (_surface_in.IsFormatAvailable(_format))
		{
			info.ImageFormat     = _format.Format    ;
			info.ImageColorSpace = _format.ColorSpace;
		}
		else
		{
			return EResult::Error_FormatNotSupported;
		}

		if (_surface_in.GetCapabilities().CurrentExtent.Width != UInt32Max)
			info.ImageExtent = _surface_in.GetCapabilities().CurrentExtent;
		else
		{
			OSAL::FrameBufferDimensions frameBufferSize = OSAL::GetFramebufferDimensions(_surface_in.GetWindow());

			Extent2D actualExtent;

			actualExtent.Width  = SCast<u32>(frameBufferSize.Width );
			actualExtent.Height = SCast<u32>(frameBufferSize.Height);

			actualExtent.Width  = clamp(actualExtent.Width , _surface_in.GetCapabilities().MinImageExtent.Width , _surface_in.GetCapabilities().MaxImageExtent.Width );
			actualExtent.Height = clamp(actualExtent.Height, _surface_in.GetCapabilities().MinImageExtent.Height, _surface_in.GetCapabilities().MaxImageExtent.Height);

			info.ImageExtent = actualExtent;
		}

		// Change to transfer destination if not rendering directly...
		info.ImageUsage.Set(EImageUsage::Color_Attachment);

		info.ImageArrayLayers = 1;   // If your supporting 3d rendering change this.

		info.ImageSharingMode      = ESharingMode::Exclusive;
		info.QueueFamilyIndexCount = 0                      ; // Optional
		info.QueueFamilyIndices    = nullptr                ; // Optional

		info.PreTransform     = _surface_in.GetCapabilities().CurrentTransform;
		info.CompositeAlpha   = ECompositeAlpha::Opaque                    ;   // Swapchain hard coded to only support opaque surfaces.
		info.Clipped          = true                                       ;
		info.OldSwapchain     = Null<Swapchain::Handle>                    ;

		EResult result = Parent::Create(Comms::GetEngagedDevice(), info);

		if (result != EResult::Success)
			return result;

		result = RetrieveImages();

		if (result != EResult::Success) 
			return result;

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

	u32 Swapchain::GetMinimumImageCount() const
	{
		return info.MinImageCount;
	}

	bool Swapchain::QuerySurfaceChanges()
	{
		if (surface->RequeryCapabilities())
		{
			Comms::GetEngagedDevice().GetGraphicsQueue().WaitUntilIdle();	

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

			actualExtent.Width  = SCast<u32>(frameBufferSize.Width );
			actualExtent.Height = SCast<u32>(frameBufferSize.Height);

			actualExtent.Width  = clamp(actualExtent.Width , surface->GetCapabilities().MinImageExtent.Width , surface->GetCapabilities().MaxImageExtent.Width );
			actualExtent.Height = clamp(actualExtent.Height, surface->GetCapabilities().MinImageExtent.Height, surface->GetCapabilities().MaxImageExtent.Height);

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

		EResult result = Parent::Create(Comms::GetEngagedDevice(), info);

		if (result != EResult::Success) 
			Exception::Fatal::Throw("Unable to regenerate swapchain.");

		result = RetrieveImages();

		if (result != EResult::Success) 
			Exception::Fatal::Throw("Unable to retrieve swapchain images after regeneration.");

		result = GenerateViews();

		if (result != EResult::Success) 
			Exception::Fatal::Throw("Unable to generate new image views after regenerating swapchain.");
	}

	EResult Swapchain::GenerateViews()
	{
		ImageView::CreateInfo viewInfo;

		viewInfo.Format   = info.ImageFormat         ;
		viewInfo.ViewType = ImageView::EViewType::_2D;

		viewInfo.SubresourceRange.AspectMask.Set(EImageAspect::Color);

		viewInfo.SubresourceRange.BaseMipLevel   = 0;
		viewInfo.SubresourceRange.LevelCount     = 1;
		viewInfo.SubresourceRange.BaseArrayLayer = 0;
		viewInfo.SubresourceRange.LayerCount     = 1;

		EResult result = EResult::Incomplete;

		imageViews.clear();

		for (auto& image : images)
		{
			viewInfo.Image = image;

			ImageView view;

			result = view.Create(Comms::GetEngagedDevice(), viewInfo);	

			if (result != EResult::Success)
				return result;

			imageViews.push_back(move(view));
		}

		return result;
	}

	EResult Swapchain::RetrieveImages()
	{
		ui32 numImages;

		EResult result = Parent::QueryImages(numImages, nullptr);

		if (result != EResult::Success) 
			return result;

		if (images.size() != numImages)
		{
			images.resize(numImages);	
		}
		
		DynamicArray<Image::Handle> handles(numImages);

		result = Parent::QueryImages(numImages, handles.data());

		for (DeviceSize index = 0; index < numImages; index++)
		{
			images[index].Assign(dref(device), handles[index]);
		}

		return result;
	}

#pragma endregion Swapchain

#pragma region RenderPass

	EResult RenderPass::Create(const LogicalDevice& _device, const CreateInfo& _info)
	{
		info = _info;

		return Parent::Create(_device, _info);
	}

	EResult RenderPass::Create(const LogicalDevice& _device, const CreateInfo& _info, const Memory::AllocationCallbacks& _allocator)
	{
		info = _info;

		return Parent::Create(_device, _info, _allocator);
	}

	u32 RenderPass::GetAttachmentCount() const { return info.AttachmentCount; }

#pragma endregion RenderPass

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

		if (!Meta::UseConcurrency())
		{
			threadsAssigned = 0;

			Fence::CreateInfo fenceInfo;

			fenceInfo.Flags.Set(EFenceCreateFlag::Signaled);

			flightToRender.Create(Comms::GetEngagedDevice(), fenceInfo);

			
			Semaphore::CreateInfo semaphoreInfo;

			swapAcquisitionStatus.Create(Comms::GetEngagedDevice(), semaphoreInfo);

			presentSubmitStatus.Create(Comms::GetEngagedDevice(), semaphoreInfo);


			ptr<CommandPool> pools = Deck::RequestCommandPools(1);

			commandPools.push_back(pools);

			Primary_CmdBuffer = getPtr(commandPools[0]->RequestBuffer());
		}

		return EResult::Success;
	}

	const CommandBuffer& FrameReference::Request_PrimaryCmdBuffer()
	{
		return dref(Primary_CmdBuffer);
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

	EResult RenderContext::Create(Swapchain& _swapChain_in)
	{
		swapchain = getPtr(_swapChain_in);

		EResult result = CreateDepthBuffer();	

		if (result != EResult::Success) return result;

		result = CreateRenderPass();

		if (result != EResult::Success) return result;

		result = CreateFramebuffers();

		if (result != EResult::Success) return result;

		result = CreateDescriptorPool();

		if (result != EResult::Success) return result;

		maxFramesInFlight = SCast<u32>(frameBuffers.size() - 1);

		frameRefs.resize(maxFramesInFlight);

		for (uDM index = 0; index < frameRefs.size(); index++)
		{
			frameRefs[index].Prepare();
		}

		Semaphore::CreateInfo semaphoreInfo;

		presentSubmitStatus.Create(Comms::GetEngagedDevice(), semaphoreInfo);

		swapsInFlight.resize(frameBuffers.size());	

		Fence::CreateInfo fenceInfo; fenceInfo.Flags.Set(EFenceCreateFlag::Signaled);

		// Will always have at least one view context.
		viewContexts.resize(1);

		Viewport viewport;

		const auto& swapExtent = _swapChain_in.GetExtent();

		viewport.Height   = SCast<f32>(swapExtent.Height) / 3.f;
		viewport.Width    = SCast<f32>(swapExtent.Width)  / 3.f;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.X        = 0.f;
		viewport.Y        = 0.f;

		viewContexts[0].SetViewport(viewport);

		Rect2D scissor;

		scissor.Offset.X = 0;
		scissor.Offset.Y = 0;

		scissor.Extent = swapExtent;

		viewContexts[0].SetScissor(scissor);

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

		depthBuffer.memory.Free();

		depthBuffer.view.Destroy();
	}

	void RenderContext::AddRenderable(ptr<ARenderable> _renderable_in)
	{
		_renderable_in->CreateDescriptorSets(SCast<u32>(swapchain->GetImages().size()), descriptorPool);

		if (renderGroups.empty())
		{
			renderGroups.resize(1);

			renderGroups.back().Pipeline = getPtr(Request_GraphicsPipeline(_renderable_in));

			renderGroups.back().Renderables.push_back(_renderable_in);

			return;
		}
		else
		{
			for (auto& renderGroup : renderGroups)
			{
				for (auto renderable : renderGroup.Renderables)
				{
					if (renderable == _renderable_in)
					{
						return;
					}
				}

				if (renderGroup.Pipeline == getPtr(Request_GraphicsPipeline(_renderable_in)))
				{
					renderGroup.Renderables.push_back(_renderable_in);

					return;
				}
			}

			renderGroups.resize(renderGroups.size() + 1);

			renderGroups.back().Pipeline = getPtr(Request_GraphicsPipeline(_renderable_in));

			renderGroups.back().Renderables.push_back(_renderable_in);

			return;
		}
	}

	void RenderContext::AddRenderCallback(RenderCallback _callback_in)
	{
		renderCallbacks.push_back(_callback_in);
	}

	const RenderPass& RenderContext::GetRenderPass() const
	{
		return renderPass;
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
			Exception::Fatal::Throw("Failed to acquire swap chain image!");
		}

		// Make sure that the acquired image has been processed by the presentation queue.
		if (swapsInFlight[currentSwap] != nullptr && dref(swapsInFlight[currentSwap]) != Null<Fence::Handle>)
			swapsInFlight[currentSwap]->WaitFor(UInt64Max);

		// Set the current swap image fence to that of the frame queue fence.
		swapsInFlight[currentSwap] = getPtr(frameRef.RenderingInFlight());

		clearValues[0].Color = { { 0.13f, 0.13f, 0.13f, 0.1f } };

		/*switch (currentFrameBuffer)
		{
			case 0: clearValues[0].Color = { 1.0f, 0.0f, 0.0f, 1.0f }; break;
			case 1: clearValues[0].Color = { 0.0f, 1.0f, 0.0f, 1.0f }; break;
			case 2: clearValues[0].Color = { 0.0f, 0.0f, 1.0f, 1.0f }; break;
			default:
			{
				clearValues[0].Color = { 1.0f,0.0f,0.0f, 1.0f };

				break;
			}
		}*/

		if (bufferDepth) 
		{
			clearValues[1].DepthStencil = { 1.0f, 0 };
		}

		beginInfo.Framebuffer = frameBuffers[currentSwap].operator Framebuffer::Handle&();

		beginInfo.RenderPass = renderPass;

		if (!Meta::UseConcurrency())
		{
			frameRefs[currentFrameBuffer].ResetCommandPool();

			auto& primaryBuffer = frameRef.Request_PrimaryCmdBuffer();

			CommandBuffer::BeginInfo cmdBeginInfo;

			cmdBeginInfo.InheritanceInfo = nullptr;

			primaryBuffer.BeginRecord(cmdBeginInfo);

			primaryBuffer.BeginRenderPass(beginInfo, ESubpassContents::Inline);

			viewContexts[0].Prepare(primaryBuffer);

			primaryBuffer.BindPipeline(EPipelineBindPoint::Graphics, dref(renderGroups[0].Pipeline));

			for (auto renderCallback : renderCallbacks)
			{
				renderCallback(primaryBuffer, currentSwap);
			}

			for (auto& viewContext : viewContexts)
			{
				viewContext.Prepare(primaryBuffer);

				for (auto& renderGroup : renderGroups)
				{
					primaryBuffer.BindPipeline(EPipelineBindPoint::Graphics, dref(renderGroup.Pipeline));

					for (auto& renderable : renderGroup.Renderables)
					{
						renderable->RecordRender(currentFrameBuffer, primaryBuffer, renderGroup.Pipeline->GetLayout());
					}
				}
			}

			primaryBuffer.EndRenderPass();

			primaryBuffer.EndRecord();

			CommandBuffer::SubmitInfo submitInfo;

			Pipeline::StageFlags waitStage(EPipelineStageFlag::ColorAttachmentOutput);

			// Need to wait for swap image to be acquired before submitting render commands to queue.
			submitInfo.WaitSemaphoreCount = 1;
			submitInfo.WaitSemaphores     = frameRef.SwapAcquisionStatus();
			submitInfo.WaitDstStageMask   = getPtr(waitStage);

			submitInfo.CommandBufferCount = 1;
			submitInfo.CommandBuffers     = primaryBuffer;

			// Let the presentation queue know that it can submit itself when render commands are finished in the queue.
			submitInfo.SignalSemaphoreCount = 1;
			submitInfo.SignalSemaphores     =  frameRef.PresentSubmitStatus();  // &frameSubmitedToPresentation.GetHandle();

			frameRef.RenderingInFlight().Reset();

			result = Comms::GetGraphicsQueue().SubmitToQueue(1, submitInfo, frameRef.RenderingInFlight());

			if (result != EResult::Success)
			{
				Exception::Fatal::Throw("Unable to submit frame to graphics queue");
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

		static DynamicArray<Swapchain::Handle> _SwapchainsToSubmit;

		Swapchain::PresentationInfo presentInfo;

		// Wait to make sure that the frame to put into the swap for presentation has finished being processed.
		presentInfo.WaitSemaphoreCount = 1                             ;
		presentInfo.WaitSemaphores     = frameRef.PresentSubmitStatus();
		
		_SwapchainsToSubmit.push_back(swapchain->operator const Swapchain::Handle&());

		Swapchain::Handle swapChains[] = { dref(swapchain) };

		presentInfo.SwapchainCount = 1;
		presentInfo.Swapchains     = swapChains;
		presentInfo.ImageIndices   = &currentSwap;

		presentInfo.Results = nullptr;

		EResult result = Comms::GetGraphicsQueue().QueuePresentation(presentInfo.operator const VkPresentInfoKHR&());

		if (result == EResult::Error_OutOfDate_KHR || result == EResult::Suboptimal_KHR)
		{
			CheckContext();
		}

		else if (result != EResult::Success)
		{
			Exception::Fatal::Throw("Not able to render frame and was not due to out-of-date or suboptimal...");
		}

		previousFrame = currentFrameBuffer;

		// Frames are numbers 0 to the number of frames buffered - 1 (to offset for initial index being 0)
		currentFrameBuffer = (currentFrameBuffer + 1) % maxFramesInFlight;
	}

	bool RenderContext::operator== (const RenderContext& _other_in)
	{
		return this == getPtr(_other_in);
	}


	// Protected

	void RenderContext::CheckContext()
	{
		// Make sure swapchain is ok first...
		if (swapchain->QuerySurfaceChanges())
		{
			Destroy();

			EResult result = CreateDepthBuffer();

			if (result != EResult::Success) 
				Exception::Fatal::Throw("Failed to recreate depth buffer in context.");

			result = CreateRenderPass();

			if (result != EResult::Success) 
				Exception::Fatal::Throw("Failed to recreate render pass in context.");

			result = CreateFramebuffers();

			if (result != EResult::Success) 
				Exception::Fatal::Throw("Failed to recreate frame renderer in context.");

			// Adjust view contexts to the new relative size of the swapchain.
		}
	}

	EResult RenderContext::CreateDepthBuffer()
	{
		Image::CreateInfo imgInfo;

		imgInfo.ImageType = EImageType::_2D;

		Comms::GetEngagedPhysicalGPU().FindSupportedFormat
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

		EResult result = depthBuffer.image.Create(Comms::GetEngagedDevice(), imgInfo);

		auto& memReq = depthBuffer.image.GetMemoryRequirements();

		Memory::AllocateInfo allocInfo;

		allocInfo.AllocationSize = memReq.Size;

		allocInfo.MemoryTypeIndex = Comms::GetEngagedDevice().GetPhysicalDevice().
			FindMemoryType(memReq.MemoryTypeBits, Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal));

		result = depthBuffer.memory.Allocate(Comms::GetEngagedDevice(), allocInfo);

		result = depthBuffer.image.BindMemory(depthBuffer.memory, depthBuffer.memoryOffset);

		if (result != EResult::Success) return result;

		ImageView::CreateInfo viewInfo;

		viewInfo.Image    = depthBuffer.image;
		viewInfo.Format   = imgInfo.Format;
		viewInfo.ViewType = ImageView::EViewType::_2D;

		viewInfo.Components.R = EComponentSwizzle::Identitity;
		viewInfo.Components.G = EComponentSwizzle::Identitity;
		viewInfo.Components.B = EComponentSwizzle::Identitity;
		viewInfo.Components.A = EComponentSwizzle::Identitity;

		viewInfo.SubresourceRange.AspectMask.Set(EImageAspect::Depth);   // TODO: Make sure this is ok for later (stencil...)

		viewInfo.SubresourceRange.BaseMipLevel   = 0;
		viewInfo.SubresourceRange.LevelCount     = 1;  //_miplevels  ;
		viewInfo.SubresourceRange.BaseArrayLayer = 0;
		viewInfo.SubresourceRange.LayerCount     = 1;

		result = depthBuffer.view.Create(Comms::GetEngagedDevice(), viewInfo);

		depthBuffer.image.TransitionLayout(EImageLayout::Undefined, EImageLayout::DepthStencil_AttachmentOptimal);

		return result;
	}

	EResult RenderContext::CreateFramebuffers()
	{
		EResult result = EResult::Incomplete;

		DynamicArray<ImageView::Handle> viewHandles;

		viewHandles.resize(renderPass.GetAttachmentCount());

		Framebuffer::CreateInfo info;

		info.RenderPass      = renderPass;
		info.AttachmentCount = SCast<u32>(viewHandles.size());
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

		for (uDM index = 0; index < frameBuffers.size(); index++)
		{
			viewHandles[0] = swapImageViews[index];

			result = frameBuffers[index].Create(Comms::GetEngagedDevice(), info);

			if (result != EResult::Success) return result;
		}

		return result;
	}

	EResult RenderContext::CreateRenderPass()
	{
		DynamicArray<RenderPass::AttachmentDescription> attachments;

		RenderPass::AttachmentDescription colorAttachment;
		RenderPass::AttachmentDescription depthAttachment;

		colorAttachment.Format  = swapchain->GetFormat();
		colorAttachment.Samples = samples;

		//colorAttachment.LoadOp  = shouldClear ? EAttachmentLoadOperation::Clear : EAttachmentLoadOperation::DontCare;
		colorAttachment.LoadOp  = EAttachmentLoadOperation::Clear;
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

			//depthAttachment.LoadOp = shouldClear ? EAttachmentLoadOperation::Clear : EAttachmentLoadOperation::DontCare;
			depthAttachment.LoadOp = EAttachmentLoadOperation::Clear;
			depthAttachment.StoreOp = EAttachmentStoreOperation::DontCare;

			depthAttachment.StencilLoadOp  = EAttachmentLoadOperation::DontCare;
			depthAttachment.StencilStoreOp = EAttachmentStoreOperation::DontCare;

			depthAttachment.InitialLayout = EImageLayout::Undefined;
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


		RenderPass::AttachmentDescription colorAttachmentResolve;

		colorAttachmentResolve.Format = swapchain->GetFormat();

		colorAttachmentResolve.Samples = samples;

		colorAttachmentResolve.LoadOp  = EAttachmentLoadOperation ::DontCare;
		colorAttachmentResolve.StoreOp = EAttachmentStoreOperation::Store   ;

		colorAttachmentResolve.StencilLoadOp  = EAttachmentLoadOperation ::DontCare;
		colorAttachmentResolve.StencilStoreOp = EAttachmentStoreOperation::DontCare;

		colorAttachmentResolve.InitialLayout = EImageLayout::Undefined        ;
		colorAttachmentResolve.FinalLayout   = EImageLayout::PresentSource_KHR;

		RenderPass::AttachmentReference colorAttachmentResolveRef;

		colorAttachmentResolveRef.Attachment = 2;

		colorAttachmentResolveRef.Layout = EImageLayout::PresentSource_KHR;


		RenderPass::SubpassDescription subpass;

		subpass.PipelineBindPoint = EPipelineBindPoint::Graphics;

		subpass.ColorAttachmentCount = 1;
		subpass.ColorAttachments     = getPtr(colorReference);

		subpass.DepthStencilAttachment = bufferDepth ? getPtr(depthReference) : nullptr;

		RenderPass::CreateInfo info;

		info.AttachmentCount = SCast<u32>(attachments.size());
		info.Attachments     = attachments.data();

		info.SubpassCount = 1;
		info.Subpasses    = getPtr(subpass);

		RenderPass::SubpassDependency dependency;

		dependency.SourceSubpass      = VK_SUBPASS_EXTERNAL;
		dependency.DestinationSubpass = 0;

		dependency.SourceStageMask.Set(EPipelineStageFlag::ColorAttachmentOutput);

		dependency.SourceAccessMask = 0;

		dependency.DestinationStageMask .Set(EPipelineStageFlag::ColorAttachmentOutput);
		dependency.DestinationAccessMask.Set(EAccessFlag::ColorAttachmentWrite);

		info.DependencyCount = 1;
		info.Dependencies    = getPtr(dependency);

		EResult result = renderPass.Create(Comms::GetEngagedDevice(), info);

		if (result != EResult::Success) 
			return result;

		beginInfo.RenderPass = renderPass;

		beginInfo.RenderArea.Offset.X = 0;
		beginInfo.RenderArea.Offset.Y = 0;

		beginInfo.RenderArea.Extent = swapchain->GetExtent();

		if (shouldClear)
		{
			clearValues.resize(renderPass.GetAttachmentCount());

			beginInfo.ClearValues     = clearValues.data();
			beginInfo.ClearValueCount = SCast<u32>(clearValues.size());
		}

		return result;
	}

	EResult RenderContext::CreateDescriptorPool()
	{
		// This is currently hardcoded for the V5 Vert Shader...
		// Descriptor pools must be created based on the requirements of a render group's descriptor layout.
		// The descriptor layout must provide information on the bindings defined for the layout.

		StaticArray<V3::DescriptorPool::Size, 2> poolSizes{};

		poolSizes[0].Type = EDescriptorType::UniformBuffer;
		poolSizes[0].Count = SCast<u32>(swapchain->GetImages().size());

		poolSizes[1].Type = EDescriptorType::Sampler;
		poolSizes[1].Count = SCast<u32>(swapchain->GetImages().size());

		V3::DescriptorPool::CreateInfo poolInfo{};

		poolInfo.PoolSizeCount = SCast<u32>(poolSizes.size());
		poolInfo.PoolSizes     = poolSizes.data();
		poolInfo.MaxSets       = SCast<u32>(swapchain->GetImages().size());

		EResult result = descriptorPool.Create(Comms::GetEngagedDevice(), poolInfo);

		return result;
	}

	GraphicsPipeline& RenderContext::Request_GraphicsPipeline(ptr<ARenderable> _renderable_in)
	{
		// Check if existing pipeline meets criteria

		// If criteria not met, create a new pipeline.

		if (graphicsPipelines.empty())
		{
			graphicsPipelines.resize(1);

			graphicsPipelines.back().Create
			(
				renderPass, 
				_renderable_in->GetShader(), 
				_renderable_in->GetVertexAttributes(), 
				_renderable_in->GetVertexBindings(),
				_renderable_in->GetDescriptorsLayout(),
				bufferDepth
			);

			return graphicsPipelines.back();
		}
		else
		{
			for (auto& graphicsPipeline : graphicsPipelines)
			{
				if 
				(
					_renderable_in->GetShader()->GetShaderStageInfos().data() == graphicsPipeline.GetShaderStages()                           &&
					_renderable_in->GetVertexAttributes()             .data() == graphicsPipeline.GetVertexInputState().AttributeDescriptions &&
					_renderable_in->GetVertexBindings()               .data() == graphicsPipeline.GetVertexInputState().BindingDescriptions   
					// Need to figure out a better check later.. I don't account for the layout...
					//*_renderable->GetDescriptorsLayout()                   == graphicsPipeline.GetDescriptorSetLayout()
				)
				{
					return graphicsPipeline;
				}
			}

			graphicsPipelines.resize(graphicsPipelines.size() + 1);

			graphicsPipelines.back().Create
			(
				renderPass, 
				_renderable_in->GetShader(), 
				_renderable_in->GetVertexAttributes(), 
				_renderable_in->GetVertexBindings(), 
				_renderable_in->GetDescriptorsLayout(),
				bufferDepth
			);

			return graphicsPipelines.back();
		}
	}

#pragma endregion RenderContext



#pragma region ViewContext

	ViewContext::ViewContext()
	{
		//viewport.Height = 
	}

	ViewContext::~ViewContext()
	{
	}

	void ViewContext::SetViewport(const Viewport& _viewport)
	{
		viewport = _viewport;
	}

	void ViewContext::SetScissor(const Rect2D& _scissor)
	{
		scissor = _scissor;
	}

	//

	void ViewContext::Prepare(const CommandBuffer& _commandbuffer)
	{
		_commandbuffer.SetViewport(viewport);
		_commandbuffer.SetScissor(scissor);
	}

	void ViewContext::Render(const CommandBuffer& _commandBuffer)
	{
	}

	void ViewContext::SetupScissor()
	{
	}

	void ViewContext::SetupViewport()
	{
	}

	void ViewContext::SetupView()
	{
	}

#pragma endregion ViewContext

	ESubmissionType SubmissionMode = ESubmissionType::Individual;

	Deque<Surface> Surfaces;

	Deque<Swapchain> SwapChains;

	Deque<RenderContext> RenderContexts;

	using Rendering_SingleGPU = Renderer_Maker<Meta::EGPU_Engage::Single>;

	void Rendering_SingleGPU::SetSubmissionMode(ESubmissionType _submissionBehaviorDesired)
	{
		SubmissionMode = _submissionBehaviorDesired;
	}

	void Rendering_SingleGPU::Shutdown()
	{
		for (auto& swapchain : SwapChains)
		{
			swapchain.Destroy(); 
		}

		for (auto& surface : Surfaces)
		{
			surface.Destroy();
		}

		for (auto& renderContext : RenderContexts)
		{
			renderContext.Destroy();
		}
	}

	Surface& Rendering_SingleGPU::Request_Surface(ptr<OSAL::Window> _window_in)
	{
		Surfaces.resize(Surfaces.size() + 1);

		Surface& surface = Surfaces.back();

		surface.AssignPhysicalDevice(Comms::GetEngagedPhysicalGPU());

		if (surface.Create(_window_in) != EResult::Success)
		{
			Exception::Fatal::Throw("Failed to create surface for targeted window.");
		}

		return surface;
	}

	void Rendering_SingleGPU::Retire_Surface(ptr<Surface> _surface_in)
	{
		_surface_in->Destroy();

		Surfaces.erase(find(Surfaces.begin(), Surfaces.end(), dref(_surface_in)));
	}

	Swapchain& Rendering_SingleGPU::Request_SwapChain(Surface& _surface_in, Surface::Format _formatDesired)
	{
		Swapchain::CreateInfo info;

		SwapChains.resize(SwapChains.size() + 1);

		Swapchain& swapchain = SwapChains.back();

		EResult result = swapchain.Create(_surface_in, _formatDesired);

		if (result != EResult::Success)
		{
			Exception::Fatal::Throw("Failed to create the swap chain!");
		}

		return swapchain;
	}

	void Rendering_SingleGPU::Retire_SwapChain(ptr<Swapchain> _swapchain_in)
	{
		_swapchain_in->Destroy();

		SwapChains.erase(find(SwapChains.begin(), SwapChains.end(), dref(_swapchain_in)));
	}

	RenderContext& Rendering_SingleGPU::Request_RenderContext(Swapchain& _swapchain_in)
	{
		RenderContexts.resize(RenderContexts.size() + 1);

		EResult result = RenderContexts.back().Create(_swapchain_in);

		if (result != EResult::Success) 
			Exception::Fatal::Throw("Failed to create render context");

		return RenderContexts.back();
	}

	void Rendering_SingleGPU::Retire_RenderContext(ptr<RenderContext> _renderContext_in)
	{
		_renderContext_in->Destroy();

		RenderContexts.erase(find(RenderContexts.begin(), RenderContexts.end(), dref(_renderContext_in)));
	}

	void Rendering_SingleGPU::Initalize()
	{
	}

	void Rendering_SingleGPU::Update()
	{
		for (auto& renderContext : RenderContexts)
		{
			renderContext.ProcessNextFrame();
		}
	}

	void Rendering_SingleGPU::Present()
	{
		for (auto& renderContext : RenderContexts)
		{
			renderContext.SubmitFrameToPresentation();
		}
	}
}
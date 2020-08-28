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

	const DynamicArray<EPresentationMode>& Surface::GetPresentationModes() const
	{
		return presentationModes;
	}

	ptr<OSAL::Window> Surface::GetWindow() const
	{
		return window;
	}

	void Surface::RequeryCapabilities()
	{
		GetPhysicalDeviceCapabilities(capabilities);
	}

#pragma endregion Surface

#pragma region SwapChain

	// Public	

	EResult SwapChain::Create(const Surface& _surface, const Surface::Format _format)
	{
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

	const DynamicArray<Image>& SwapChain::GetImages() const
	{
		return images;
	}

	const DynamicArray<ImageView>& SwapChain::GetImageViews() const
	{
		return imageViews;
	}


	// Protected

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

		images.resize(numImages);	std::vector<Image::Handle> handles(numImages);

		result = Parent::QueryImages(numImages, handles.data());

		for (DeviceSize index = 0; index < numImages; index++)
		{
			images[index].Assign(device, handles[index]);
		}

		return result;
	}

#pragma endregion SwapChain

#pragma region RenderContext

	// Public

	EResult RenderContext::Create(const SwapChain& _swapChain)
	{
		swapchain = &_swapChain;

		EResult result = CreateDepthBuffer();	

		result = CreateRenderPass();

		return result;
	}

	void RenderContext::Record()
	{
		switch (currentFrame)    
		{ 
			case 0: 
			{
				clearValues[0].Color = { 1.0f,0.0f,0.0f,1.0f };

				break;
			}
			case 1: 
			{
				clearValues[0].Color = { 0.0f,1.0f,0.0f,1.0f };

				break;
			}
			case 2: 
			{
				clearValues[0].Color = { 0.0f,0.0f,1.0f,1.0f };
			 
				break;
			}
			default:
			{
				clearValues[0].Color = { 0.0f,0.0f,0.0f,1.0f };

				break;    
			}
		}

		if (bufferDepth) 
		{
			clearValues[1].DepthStencil.Depth   = 1.0f;
			clearValues[1].DepthStencil.Stencil = 0;
		}

		beginInfo.Framebuffer = frameBuffers[currentFrame];


	}


	// Protected

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

		for (auto& swapViews : swapchain->GetImageViews())
		{
			viewHandles[0] = swapViews;

			Framebuffer framebuffer;

			result = framebuffer.Create(GetEngagedDevice().GetHandle(), info);

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

		colorAttachment.InitialLayout = EImageLayout::Color_AttachmentOptimal;
		colorAttachment.FinalLayout   = EImageLayout::Color_AttachmentOptimal;

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

		RenderPass::CreateInfo info;

		info.Attachments = attachments.data();

		info.SubpassCount = 1;
		info.Subpasses = &subpass;

		info.DependencyCount = 0;
		info.Dependencies = nullptr;

		EResult result = renderPass.Create(GetEngagedDevice().GetHandle(), info);

		if (result != EResult::Success) return result;

		clearValues.resize(renderPass.GetAttachmentCount());

		
		beginInfo.RenderPass = renderPass;
		beginInfo.RenderArea.Extent = swapchain->GetExtent();
		beginInfo.ClearValueCount = SCast<uint32>(clearValues.size());
		beginInfo.ClearValues = clearValues.data();
	}

#pragma endregion RenderContext



	DynamicArray<Surface> Surfaces;

	DynamicArray<SwapChain> SwapChains;

	DynamicArray<RenderContext> RenderContexts;



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
	}

	const Surface& CreateSurface(ptr<OSAL::Window> _window)
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

	const SwapChain& CreateSwapChain(const Surface& _surface, const Surface::Format _formatDesired)
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

	const RenderContext& CreateRenderContext(const SwapChain& _swapchain)
	{
		RenderContext context;

		EResult result = context.Create(_swapchain);

		if (result != EResult::Success) throw RuntimeError("Failed to create render context");	

		RenderContexts.push_back(context);

		return RenderContexts.back();
	}

	void InitalizeRenderer()
	{
	}

	void Renderer_Draw()
	{
		for (auto& renderContext : RenderContexts)
		{
			// Make sure to sync before recording...

			//renderContext.Record();

			//renderContext.Submit();
		}
	}
}
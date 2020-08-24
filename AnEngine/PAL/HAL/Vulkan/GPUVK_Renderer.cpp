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

	const DynamicArray<Image>& SwapChain::GetImages()
	{
		return images;
	}

	const DynamicArray<ImageView>& SwapChain::GetImageViews()
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

	EResult RenderContext::Create(const Surface& _surface, const SwapChain& _swapChain)
	{
		surface   = &_surface  ;
		swapchain = &_swapChain;

		return CreateDepthBuffer();	
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
		imgInfo.Samples      = ESampleCount::_1       ;   // TODO: Change me.
		imgInfo.SharingMode  = ESharingMode::Exclusive;

		imgInfo.QueueFamilyIndexCount = 0   ;
		imgInfo.QueueFamilyIndices    = NULL;

		imgInfo.Usage.Set(EImageUsage::DepthStencil_Attachment);

		EResult result = Heap(depthBuffer.image.Create(GetEngagedDevice().GetHandle(), imgInfo));

		if (result != EResult::Success) return result;

		result = Heap(depthBuffer.memory.Allocate
		(
					  depthBuffer.image.GetMemoryRequirements(), 
					  Memory::PropertyFlags(EMemoryPropertyFlag::DeviceLocal)
		));

		if (result != EResult::Success) return result;

		depthBuffer.image.BindMemory(depthBuffer.memory, 0);

		depthBuffer.image.TransitionLayout(EImageLayout::Undefined, EImageLayout::DepthStencil_AttachmentOptimal);

		ImageView::CreateInfo viewInfo {};

		viewInfo.Image    = depthBuffer.image;
		viewInfo.Format   = imgInfo.Format;
		viewInfo.ViewType = ImageView::EViewType::_2D;

		viewInfo.SubresourceRange.AspectMask.Set(EImageAspect::Depth);

		viewInfo.SubresourceRange.BaseMipLevel   = 0;
		viewInfo.SubresourceRange.LevelCount     = 1;  //_miplevels  ;
		viewInfo.SubresourceRange.BaseArrayLayer = 0;
		viewInfo.SubresourceRange.LayerCount     = 1;

		result = Heap(depthBuffer.view.Create(GetEngagedDevice().GetHandle(), viewInfo));

		return result;
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

	const RenderContext& CreateRenderContext(const Surface& _surface, const SwapChain& _swapchain)
	{
		RenderContext context;

		EResult result = context.Create(_surface, _swapchain);

		if (result != EResult::Success) throw RuntimeError("Failed to create render context");	

		RenderContexts.push_back(context);

		return RenderContexts.back();
	}

	void InitalizeRenderer()
	{
		
	}
}
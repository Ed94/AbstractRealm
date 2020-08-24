#pragma once



#include "GPUVK_Comms.hpp"
#include "GPUVK_PayloadDeck.hpp"

#include "GPUVK_Memory.hpp"
#include "GPUVK_Resources.hpp"
#include "LAL.hpp"
#include "OSAL.hpp"



namespace HAL::GPU::Vulkan
{
	using namespace LAL;



	// Classes

	class Surface : public V4::Surface
	{
	public:

		using Parent = V4::Surface;

		EResult Create(ptr<OSAL::Window> _window);

		bool IsFormatAvailable(const Format& _format) const;

		const Capabilities& GetCapabilities() const;

		const DynamicArray<EPresentationMode>& GetPresentationModes() const;

		ptr<OSAL::Window> GetWindow() const;

	protected:

		Capabilities capabilities;

		DynamicArray<Format> formats;

		DynamicArray<EPresentationMode> presentationModes;

		ptr<OSAL::Window> window;
	};

	class SwapChain : public V4::SwapChain
	{
	public:

		using Parent = V4::SwapChain;

		EResult Create(const Surface& _surface, const Surface::Format _format);

		Extent2D GetExtent() const;

		EFormat GetFormat() const;

		const DynamicArray<Image>& GetImages();

		const DynamicArray<ImageView>& GetImageViews();

	protected:

		EResult GenerateViews();

		EResult RetrieveImages();

		uint32 SupportedImageCount;

		DynamicArray<Image    > images    ;
		DynamicArray<ImageView> imageViews;
		
	};

	class RenderContext
	{
	public:

		EResult Create(const Surface& _surface, const SwapChain& _swapChain);

	protected:

		EResult CreateDepthBuffer();

		ImagePackage depthBuffer;

		DataSize currentFrame = 0;

		DynamicArray<Framebuffer> frameBuffers;

		ptr<const SwapChain> swapchain;

		ptr<const Surface> surface;
	};



	eGlobal DynamicArray<Surface      > Surfaces      ;
	eGlobal DynamicArray<SwapChain    > SwapChains    ;
	eGlobal DynamicArray<RenderContext> RenderContexts;



	const Surface& CreateSurface(ptr<OSAL::Window> _window);

	const SwapChain& CreateSwapChain(const Surface& _surface, const Surface::Format _formatDesired);

	const RenderContext& CreateRenderContext(const Surface& _surface, const SwapChain& _swapchain);

	void DeinitalizeRenderer();

	void InitalizeRenderer();

	void Renderer_Draw();
}

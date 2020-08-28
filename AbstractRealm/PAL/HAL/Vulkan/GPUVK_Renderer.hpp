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

		Format GetPreferredFormat() const;

		ptr<OSAL::Window> GetWindow() const;

		void RequeryCapabilities();

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

		const DynamicArray<Image>& GetImages() const;

		const DynamicArray<ImageView>& GetImageViews() const;

	protected:

		EResult GenerateViews();

		EResult RetrieveImages();

		ptr<const Surface> surface;

		uint32 SupportedImageCount;

		DynamicArray<Image    > images    ;
		DynamicArray<ImageView> imageViews;
		
	};

	class RenderPass_WIP : V4::RenderPass
	{
	public:
		struct Attachment 
		{
			AttachmentDescription Description;
			AttachmentReference   Reference  ;
		};

		struct SubpassInfo
		{
			SubpassDescription Description;
			SubpassDependency  Dependency ;
		};




	protected:
		DynamicArray<Attachment > attachments;
		DynamicArray<SubpassInfo> subpasses;
	};

	class ARenderable
	{
	public:
		~ARenderable();

		virtual void RecordRender();
	};

	struct RenderableFamily
	{
		DynamicArray< ptr<ARenderable>> Renderables;

		ptr<GraphicsPipeline> Pipeline;
	};

	class RenderContext
	{
	public:

		EResult Create(const SwapChain& _swapChain);

		void Destroy();

		void Record();

		void Submit();

	protected:

		EResult CreateDepthBuffer();

		EResult CreateFramebuffer();

		EResult CreateRenderPass();

		ptr<const SwapChain> swapchain;

		bool shouldClear = true, bufferDepth = true;

		DynamicArray<ClearValue> clearValues;

		ESampleCount samples = ESampleCount::_1;

		RenderPass renderPass;

		RenderPass::BeginInfo beginInfo;

		DataSize currentFrame = 0;

		DynamicArray<RenderableFamily> renderableFamilies;

		DynamicArray<ptr<CommandBuffer>> commandBuffers;

		DynamicArray<Framebuffer> frameBuffers;

		ImagePackage depthBuffer;
	};

	class PrimitiveRenderable : public ARenderable
	{
	public:

		implem void RecordRender();

	protected:
		VertexBuffer vertBuffer;
	};



	eGlobal DynamicArray<Surface      > Surfaces      ;
	eGlobal DynamicArray<SwapChain    > SwapChains    ;
	eGlobal DynamicArray<RenderContext> RenderContexts;



	const Surface& CreateSurface(ptr<OSAL::Window> _window);

	const SwapChain& CreateSwapChain(const Surface& _surface, const Surface::Format _formatDesired);

	const RenderContext& CreateRenderContext(const SwapChain& _swapchain);

	void DeinitalizeRenderer();

	void InitalizeRenderer();

	void Renderer_Draw();
}

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

		Format GetPreferredFormat() const;

		const DynamicArray<EPresentationMode>& GetPresentationModes() const;

		ptr<OSAL::Window> GetWindow() const;

		bool RequeryCapabilities();

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

		EResult Create(Surface& _surface, Surface::Format _format);

		Extent2D GetExtent() const;

		EFormat GetFormat() const;

		bool QuerySurfaceChanges();

		const DynamicArray<Image>& GetImages() const;

		const DynamicArray<ImageView>& GetImageViews() const;

	protected:

		void Regenerate();

		EResult GenerateViews();

		EResult RetrieveImages();

		ptr<Surface> surface;

		uint32 SupportedImageCount;

		DynamicArray<Image    > images    ;
		DynamicArray<ImageView> imageViews;
	};

	class ARenderable
	{
	public:
		~ARenderable();

		virtual void RecordRender(const CommandBuffer& _commandBuffer);
	};

	struct RenderGroup
	{
		DynamicArray< ptr<ARenderable>> Renderables;

		ptr<GraphicsPipeline> Pipeline;
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

		EResult Create();


	protected:

		DynamicArray<Attachment > attachments;
		DynamicArray<SubpassInfo> subpasses;

		ImagePackage depthBuffer;

		BeginInfo beginInfo;

		ESampleCount samples = ESampleCount::_1;

		bool shouldClear = true, bufferDepth = true;
	};


	// Upgrade the frame buffer?
	class FrameRenderer
	{
	public:

		EResult Prepare
		(
		/*	Framebuffer& frameBuffer,
			DynamicArray<RenderPass_WIP> _renderPasses*/
		);

		void WaitFor_RenderQueued();

		//void Reset_RenderQueuedFence();

		Fence& GetFence_RenderQueued();

		//void WaitFor_FrameInFlight();

		Semaphore& GetSemaphore_SwapImageAcquired();

		const CommandBuffer& Request_PrimaryCmdBuffer();

		//const CommandBuffer& Request_SecondaryCmdBuffer();

		void ResetCommandPool();

		//void Record();

		//void Submit();

	private:

		ptr<Framebuffer> frameBuffer;

		DynamicArray<ptr<CommandPool>> commandPools;   // 1 Per working thread for the frame.

		ptr<const CommandBuffer> Primary_CmdBuffer;

		DynamicArray<ptr<CommandBuffer>> Secondary_CmdBuffers;

		// Fence Pool
		//DynamicArray<Fence> fences;

		Fence renderQueued, frameInFlight;

		Semaphore swapImageAcquired;

		// Semaphore Pool
		//DynamicArray<Semaphore> semaphores;

		ptr<DynamicArray<RenderPass_WIP>> renderPasses;

		// Thread Count
		DataSize threadsAssigned;
	};

	class RenderContext
	{
	public:

		EResult Create(SwapChain& _swapChain);



		void ProcessNextFrame();

		void SubmitFrameToPresentation();

	protected:

		EResult CreateDepthBuffer();

		EResult CreateFramebuffer();

		EResult CreateRenderPass();

		void CheckContext();

		ptr<SwapChain> swapchain;

		bool processingFrame = false;

		uint32 currentFrame = 0, previousFrame,
			currentSwapImage;

		DynamicArray<RenderGroup> renderGroups;

		//DynamicArray<ptr<CommandBuffer>> commandBuffers;

		DynamicArray<Framebuffer> frameBuffers;

		DynamicArray<FrameRenderer> frameRenders;

		Semaphore frameSubmitedToPresentation;

		DynamicArray<Fence> swapImagesInFlight;


		// Render pass object (To be used later)

		RenderPass renderPass;

		RenderPass::BeginInfo beginInfo;

		ImagePackage depthBuffer;

		ESampleCount samples = ESampleCount::_1;

		DynamicArray<ClearValue> clearValues;

		bool shouldClear = true, bufferDepth = true;


		// Proper render pass array...

		DynamicArray<RenderPass_WIP> RenderPasses;
	};

	class PrimitiveRenderable : public ARenderable
	{
	public:

		implem void RecordRender();

	protected:
		VertexBuffer vertBuffer;
	};



	eGlobal Deque<Surface      > Surfaces      ;
	eGlobal Deque<SwapChain    > SwapChains    ;
	eGlobal Deque<RenderContext> RenderContexts;



	Surface& CreateSurface(ptr<OSAL::Window> _window);

	SwapChain& CreateSwapChain(Surface& _surface, Surface::Format _formatDesired);

	const RenderContext& CreateRenderContext(SwapChain& _swapchain);

	void DeinitalizeRenderer();

	void InitalizeRenderer();

	void Renderer_Update();
}

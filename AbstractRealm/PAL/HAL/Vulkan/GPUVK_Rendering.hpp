#pragma once


#include "GPUVK_Comms.hpp"
#include "GPUVK_PayloadDeck.hpp"
#include "GPUVK_Memory.hpp"
#include "GPUVK_Resources.hpp"
#include "GPUVK_Pipeline.hpp"

#include "OSAL.hpp"


namespace HAL::GPU::Vulkan
{
	using namespace LAL;

	class ViewContext;


	enum class ESubmissionType
	{
		Individual,   // Individual render and presentation context submissions.
		Combined  ,   // Combined render and presentation context submissions.
		IR_CP     ,   // Individual render and combined presentation context submissions.
		CR_IP         // Combined render and individual presentation context submissions.
	};


#pragma region Classes

	class Surface : public V3::Surface
	{
	public:

		using Parent = V3::Surface;

		EResult Create(ptr<OSAL::Window> _window_in);

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

	class Swapchain : public V3::Swapchain
	{
	public:

		using Parent = V3::Swapchain;

		EResult Create(Surface& _surface_in, Surface::Format _format);

		void Destroy();

		Extent2D GetExtent() const;

		EFormat GetFormat() const;

		u32 GetMinimumImageCount() const;

		bool QuerySurfaceChanges();

		const DynamicArray<Image>& GetImages() const;

		const DynamicArray<ImageView>& GetImageViews() const;

	protected:

		void Regenerate();

		EResult GenerateViews();

		EResult RetrieveImages();

		CreateInfo info;

		ptr<Surface> surface;

		u32 SupportedImageCount;

		Meta::EGPU_PresentMode presentationMode = Meta::GPU_PresentationPref();

		Meta::EGPU_FrameBuffering frameBuffering = Meta::GPU_FrameBufferingPref();

		DynamicArray<Image    > images    ;
		DynamicArray<ImageView> imageViews;
	};

	class RenderPass : public V3::RenderPass
	{
	public:

		using Parent = V3::RenderPass;

		struct Attachment 
		{
			AttachmentDescription Description;
			AttachmentReference   Reference  ;
		};

		/*struct SubpassInfo
		{
			SubpassDescription Description;
			SubpassDependency  Dependency ;
		};*/

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info);

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info, const Memory::AllocationCallbacks& _allocator);

		u32 GetAttachmentCount() const;

	protected:

		struct SubpassInfo
		{
			SubpassDescription Description;
			SubpassDependency  Dependency;

			DynamicArray<AttachmentReference> Inputs       ;
			DynamicArray<AttachmentReference> Colors       ;
			DynamicArray<AttachmentReference> Resolves     ;
			DynamicArray<AttachmentReference> DepthStencils;

			DynamicArray<u32> Preserves;
		};

		CreateInfo info;

		//DynamicArray<Attachment > attachments;
		//DynamicArray<SubpassInfo> subpasses  ;

		/*DynamicArray<AttachmentDescription> attachments;
		DynamicArray<SubpassInfo          > subpasses  ;

		ImagePackage depthBuffer;

		BeginInfo beginInfo;

		ESampleCount samples = ESampleCount::_1;

		bool shouldClear = true, bufferDepth = true;*/
	};

	// Upgrade the frame buffer?

	/**
	 * This is a way to keep track of the current frame
	 * the render context is working on when rendering or submitting
	 * to presentation.
	 */
	class FrameReference
	{
	public:

		EResult Prepare();

		void Destroy();

		//void Reset_RenderQueuedFence();

		Fence& RenderingInFlight();

		Fence& PresentInFlight();

		Semaphore& SwapAcquisionStatus();

		Semaphore& PresentSubmitStatus();

		const CommandBuffer& Request_PrimaryCmdBuffer();

		//const CommandBuffer& Request_SecondaryCmdBuffer();

		void ResetCommandPool();

		void WaitFor_RenderingInFlight();

	private:

		DynamicArray< ptr<CommandPool>> commandPools;   // 1 Per working thread for the frame.
 
		ptr<const CommandBuffer> Primary_CmdBuffer;

		DynamicArray< ptr<CommandBuffer>> Secondary_CmdBuffers;

		// Fence Pool
		//DynamicArray<Fence> fences;

		Fence flightToRender;

		Semaphore swapAcquisitionStatus, presentSubmitStatus;

		// Semaphore Pool
		//DynamicArray<Semaphore> semaphores;

		//Deque<RenderPass> renderPasses;

		// Thread Count
		uDM threadsAssigned;
	};

	struct RenderGroup
	{
		DynamicArray< ptr<ARenderable>> Renderables;

		ptr<GraphicsPipeline> Pipeline;
	};

	using RenderCallback = fnPtr<void,  const CommandBuffer& /*_buffer*/, int /*_index*/>;

	class RenderContext
	{
	public:

		EResult Create(Swapchain& _swapChain_in);

		void Destroy();

		void AddRenderable(ptr<ARenderable> _renderable_in);

		void AddRenderCallback(RenderCallback _callback_in);

		const RenderPass& GetRenderPass() const;

		void ProcessNextFrame();

		void SubmitFrameToPresentation();

		bool operator== (const RenderContext& _other_in);

	protected:

		EResult CreateDepthBuffer();

		EResult CreateFramebuffers();

		EResult CreateRenderPass();

		EResult CreateDescriptorPool();

		void CheckContext();

		GraphicsPipeline& Request_GraphicsPipeline(ptr<ARenderable> _renderable_in);

		ptr<Swapchain> swapchain;

		bool processingFrame = false;

		ui32 currentFrameBuffer = 0 ,    // Current frame to process
			 previousFrame    ,    // Previously processed frame
			 currentSwap      ,    // Currently rendered frame to present.
			 maxFramesInFlight ;   // Maximum number of frames to process at the same time.

		DynamicArray<Framebuffer> frameBuffers;   // TODO: use frame reference?

		DynamicArray<FrameReference> frameRefs;

		Semaphore presentSubmitStatus;

		DynamicArray< ptr<Fence>> swapsInFlight;


		// Render pass object (To be used later)

		RenderPass renderPass;

		RenderPass::BeginInfo beginInfo;

		ImagePackage depthBuffer;

		ESampleCount samples = ESampleCount::_1;

		DynamicArray<ClearValue> clearValues;

		bool shouldClear = true, bufferDepth = true;


		DynamicArray<RenderCallback> renderCallbacks;


		// Proper render pass array...

		DynamicArray<RenderPass> RenderPasses;

		// Proper ViewContexts

		DynamicArray<ViewContext> viewContexts;

		// New stuff...

		DynamicArray<GraphicsPipeline> graphicsPipelines;

		DynamicArray<RenderGroup> renderGroups;

		// Resource stuff....

		DescriptorPool descriptorPool;

		//DynamicArray<DescriptorPool> descriptorPool;

		DynamicArray<DescriptorSet> descriptorSets;
	};

	class ViewContext
	{
	public:

		 ViewContext();
		~ViewContext();

		void SetViewport(const Viewport& _viewport_in);

		void SetScissor(const Rect2D& _scissor_in);

		// 

		void Prepare(const CommandBuffer& _commandBuffer_in);

		void Render(const CommandBuffer& _commandbuffer_in);

		void SetupScissor();

		void SetupViewport();

		void SetupView();

	protected:

		Viewport viewport;
		Rect2D   scissor;
	};
#pragma endregion Classes


	template<Meta::EGPU_Engage>
	class Renderer_Maker;

	template<>
	class Renderer_Maker<Meta::EGPU_Engage::Single>
	{
	public:

		static Surface& Request_Surface(ptr<OSAL::Window> _window_in);
		static void     Retire_Surface (ptr<Surface> _surface_in);

		static Swapchain& Request_SwapChain(Surface& _surface_in, Surface::Format _formatDesired);
		static void       Retire_SwapChain (ptr<Swapchain> _swapchain_in);

		static RenderContext& Request_RenderContext(Swapchain& _swapchain_in);
		static void           Retire_RenderContext(const ptr<RenderContext> _renderContext);

		static void SetSubmissionMode(ESubmissionType _submissionBehaviorDesired);

		static void Initalize();
		static void Shutdown();
		static void Present();
		static void Update();
	};

	using Rendering = Renderer_Maker<Meta::GPU_Engagement>;

	void SetTestCallback(void (*_recordToBuffers)
	(
		int                   _index,
		const CommandBuffer&  _buffer,
		Framebuffer::Handle   _frameBuffer,
		ptr<const Swapchain>  _swap,
		RenderPass::BeginInfo _beginInfo
	)
	);
}

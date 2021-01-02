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

	class Surface : public V3::Surface
	{
	public:

		using Parent = V3::Surface;

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

	class Swapchain : public V3::Swapchain
	{
	public:

		using Parent = V3::Swapchain;

		EResult Create(Surface& _surface, Surface::Format _format);

		void Destroy();

		Extent2D GetExtent() const;

		EFormat GetFormat() const;

		ui32 GetMinimumImageCount() const;

		bool QuerySurfaceChanges();

		const DynamicArray<Image>& GetImages() const;

		const DynamicArray<ImageView>& GetImageViews() const;

	protected:

		void Regenerate();

		EResult GenerateViews();

		EResult RetrieveImages();

		CreateInfo info;

		ptr<Surface> surface;

		ui32 SupportedImageCount;

		Meta::EGPU_PresentMode presentationMode = Meta::GPU_PresentationPref();

		Meta::EGPU_FrameBuffering frameBuffering = Meta::GPU_FrameBufferingPref();

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

		EResult Create(const LogicalDevice& _device, CreateInfo& _info);

		EResult Create(const LogicalDevice& _device, CreateInfo& _info, const Memory::AllocationCallbacks& _allocator);

		ui32 GetAttachmentCount() const;

	protected:

		struct SubpassInfo
		{
			SubpassDescription Description;
			SubpassDependency  Dependency;

			DynamicArray<AttachmentReference> Inputs       ;
			DynamicArray<AttachmentReference> Colors       ;
			DynamicArray<AttachmentReference> Resolves     ;
			DynamicArray<AttachmentReference> DepthStencils;

			DynamicArray<ui32> Preserves;
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

		DynamicArray<ptr<CommandPool>> commandPools;   // 1 Per working thread for the frame.
 
		ptr<const CommandBuffer> Primary_CmdBuffer;

		DynamicArray<ptr<CommandBuffer>> Secondary_CmdBuffers;

		// Fence Pool
		//DynamicArray<Fence> fences;

		Fence flightToRender;

		Semaphore swapAcquisitionStatus, presentSubmitStatus;

		// Semaphore Pool
		//DynamicArray<Semaphore> semaphores;

		//Deque<RenderPass> renderPasses;

		// Thread Count
		WordSize threadsAssigned;
	};

	class RenderContext
	{
	public:

		EResult Create(Swapchain& _swapChain);

		void Destroy();

		void ProcessNextFrame();

		void SubmitFrameToPresentation();

		bool operator== (const RenderContext& _other);

	protected:

		EResult CreateDepthBuffer();

		EResult CreateFramebuffers();

		EResult CreateRenderPass();

		void CheckContext();

		ptr<Swapchain> swapchain;

		bool processingFrame = false;

		ui32 currentFrameBuffer = 0 ,    // Current frame to process
			   previousFrame    ,    // Previously processed frame
			   currentSwap      ,    // Currently rendered frame to present.
			   maxFramesInFlight ;   // Maximum number of frames to process at the same time.

		DynamicArray<RenderGroup> renderGroups;

		DynamicArray<Framebuffer> frameBuffers;   // TODO: use frame reference?

		DynamicArray<FrameReference> frameRefs;

		Semaphore presentSubmitStatus;

		DynamicArray<ptr<Fence>> swapsInFlight;


		// Render pass object (To be used later)

		RenderPass renderPass;

		RenderPass::BeginInfo beginInfo;

		ImagePackage depthBuffer;

		ESampleCount samples = ESampleCount::_1;

		DynamicArray<ClearValue> clearValues;

		bool shouldClear = true, bufferDepth = true;


		// Proper render pass array...

		DynamicArray<RenderPass> RenderPasses;
	};

	class PrimitiveRenderable : public ARenderable
	{
	public:

		implem void RecordRender();

	protected:
		VertexBuffer vertBuffer;
	};



	enum class ESubmissionType
	{
		Individual,   // Individual render and presentation context submissions.
		Combined  ,   // Combined render and presentation context submissions.
		IR_CP     ,   // Individual render and combined presentation context submissions.
		CR_IP         // Combined render and individual presentation context submissions.
	};



	Surface& Request_Surface(ptr<OSAL::Window> _window);

	void Retire_Surface(ptr<Surface> _surface);

	Swapchain& Request_SwapChain(Surface& _surface, Surface::Format _formatDesired);

	void Retire_Swapchain(ptr<Swapchain> _swapchain);

	RenderContext& Request_RenderContext(Swapchain& _swapchain);

	void Retire_RenderContext(const ptr<RenderContext> _renderContext);

	void Renderer_SetSubmissionMode(ESubmissionType _submissionBehaviorDesired);

	void Renderer_Shutdown();

	void Renderer_Initalize();

	void Renderer_Update();

	void Renderer_Present();
}

/*
GPU HAL: Vulkan Interface

This currently contains extra implementation for GPU_HAL, I'm not final on my design so
its going to stay here and get modified and slowly moved to VaultedThermals
(If its more associated wit that).

Right now the implementation is heavily hard coded / procedural, this will change with time.
*/

#pragma once


#include "HAL/HAL_Backend.hpp"
#include "HAL/HAL_GPU_CoreDefs.hpp"
#include "Vulkan_API.hpp"
#include "HAL/Vulkan/GPUVK_PayloadDeck.hpp"

#include "OSAL/OSAL_Windowing.hpp"


#if VulkanAPI_Interface == VaultedVulkan_Interface

namespace HAL::GPU::Vulkan
{
	using namespace VV        ;
	using namespace VV::V3    ;
	using namespace VV::SPIR_V;

	using LAL::DynamicArray;


	//using 

	using ExtensionIdentifierList     = DynamicArray< RoCStr             >;
	using FenceList                   = DynamicArray< Fence::Handle      >;
	using FrameBufferList             = DynamicArray< Framebuffer::Handle>;   
	using ImageList                   = DynamicArray< Image::Handle      >;
	using ImageViewList               = DynamicArray< ImageView          >;
	using SemaphoreList               = DynamicArray< Semaphore::Handle  >;   
	using SurfaceFormatList           = DynamicArray< Surface::Format    >;
	using SurfacePresentationModeList = DynamicArray< EPresentationMode  >;
	using ValidationLayerList         = DynamicArray< RoCStr             >;

	// OS

	using OSAL::Window;


	// Structs

	struct RawRenderContext : ARenderContext
	{
		using Memory = Vulkan::Memory;

		AppInstance::Handle              ApplicationInstance;
		V3::PhysicalDevice               PhysicalDevice     ;
		V3::LogicalDevice::Handle        LogicalDevice      ;
		ui32                             QueueFamilyIndex   ;
		LogicalDevice::Queue             Queue              ;
		V3::Pipeline::Cache::Handle      PipelineCache      ;
		EFormat                          ImageFormat        ;
		V3::RenderPass::Handle           RenderPass         ;
		ptr<Memory::AllocationCallbacks> Allocator          ;
		ui32                             MinimumFrameBuffers;
		ui32                             FrameBufferCount   ;
		Extent2D                         FrameSize          ;
		ESampleCount                     MSAA_Samples       ;
	};

	using RenderContextList = DynamicArray<RawRenderContext>;

	
	// Functions

#pragma region Temp

	void Start_GPUVK_Demo(ptr<OSAL::Window> _window_in);

	void Render();

	void Stop_GPUVK_Demo();

	void Present();

#pragma endregion Temp


#pragma region Staying

	// Descriptor Pool

	EResult RequestDescriptorPool(V3::DescriptorPool& _pool_out, V3::DescriptorPool::CreateInfo _info);
	
	// Command Buffer Related

	void EndSingleTimeBuffer(const CommandBuffer& _buffer_in);

	const CommandBuffer& RequestSingleTimeBuffer();

#pragma endregion Staying


#pragma region UnderReview

	/*
	Checks to see if the validation layers specified are supported.
	*/
	//bool CheckValidationLayerSupport(ValidationLayerList& _validationLayersSpecified);

	void CleanupSwapChain();

	void CreateFrameObjects();

	void CreateFrameBuffers();


	void CreateSyncObjects();

	// Exposed but really should not be used directly unless for another implementation I guess.
	Bool DebugCallback_Error
	(
			  DebugUtils::Messenger::ServerityFlags   _messageServerity,
			  DebugUtils::Messenger::TypeFlags        _messageType     ,
		const V1::DebugUtils::Messenger::CallbackData _callbackData    ,
			  ptr<void>                               _userData_in
	);

	EFormat FindDepthFormat();

	EFormat FindSupportedFormat(const DynamicArray<EFormat>& _canidates_in, EImageTiling _tiling, FormatFeatureFlags _features);

	void GenerateMipMaps(Image& _image_in, EFormat _format, u32 _textureWidth, u32 _textureHeight, u32 _mipLevels);

	void Start_GPUComms(RoCStr _applicationName, Meta::AppVersion _applicationVersion);

	void CeaseComms();

	void WaitForIdle();


	ptr<ARenderContext> GetRenderContext(ptr<OSAL::Window> _window_in);

	u32 GetNumberOfFramebuffers();

	u32 GetMinimumFramebufferCount();

	using RenderCallback = fnPtr<void,  const CommandBuffer& /*_buffer_in*/, int /*_index*/>;

	void AddRenderCallback(RenderCallback _callback_in);

	void Default_InitializeRenderer(ptr<OSAL::Window> _window_in);

	void Default_ReinitializeRenderer(ptr<OSAL::Window> _window_in);

	void Default_DeinitializeRenderer(ptr<OSAL::Window> _window_in);

	void Default_DrawFrame(ptr<OSAL::Window> _window_in);

#pragma endregion UnderReview
}

#endif

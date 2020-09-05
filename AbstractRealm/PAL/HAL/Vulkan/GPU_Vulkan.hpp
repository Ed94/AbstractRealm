/*
GPU HAL: Vulkan Interface


This currently contains extra implementation for GPU_HAL, I'm not final on my design so
its going to stay here and get modified and slowly moved to VaultedThermals
(If its more associated wit that).

Right now the implementation is heavily hard coded / procedural, this will change with time.
*/

#pragma once



#include "Vulkan_API.hpp"

#include "GPU_HAL_CoreDefs.hpp"
#include "Meta/AppInfo.hpp"
#include "Meta/Config/HAL_Config.hpp"
#include "Meta/EngineInfo.hpp"
#include "Meta/Config/OSAL_Config.hpp"
#include "Core/IO/Basic_FileIO.hpp"

#include "OSAL/OSAL_Platform.hpp"
#include "OSAL/OSAL_Windowing.hpp"



#if VulkanAPI_Interface == VaultedThermals_Interface

	namespace HAL::GPU
	{
		namespace Vulkan
		{
			using namespace VT        ;
			using namespace VT::V3    ;
			using namespace VT::SPIR_V;

			using namespace LAL ;
			using namespace Meta;

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
				AppInstance::Handle          ApplicationInstance;
				V3::PhysicalDevice           PhysicalDevice     ;
				V3::LogicalDevice            LogicalDevice      ;
				uint32                       QueueFamilyIndex   ;
				LogicalDevice::Queue         Queue              ;
				Pipeline::Cache              PipelineCache      ;
				EFormat                      ImageFormat        ;
				V3::RenderPass               RenderPass         ;
				Memory::AllocationCallbacks* Allocator          ;
				uint32                       MinimumFrameBuffers;
				uint32                       FrameBufferCount   ;
				Extent2D                     FrameSize          ;
				ESampleCount                 MSAA_Samples       ;
			};

			using RenderContextList = DynamicArray<RawRenderContext>;

			
			// Functions

		#pragma region Temp

			void Start_ClearColorDemo(ptr<OSAL::Window> _window);

			void Render();

			void Stop_ClearColorDemo();

			void Present();

		#pragma endregion Temp


		#pragma region Staying

			// Descriptor Pool

			EResult RequestDescriptorPool(V3::DescriptorPool& _pool, V3::DescriptorPool::CreateInfo _info);
			
			// Command Buffer Related

			void EndSingleTimeBuffer(CommandBuffer& _buffer);

			CommandBuffer RequestSingleTimeBuffer();

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
			Bool DebugCallback
			(
				DebugUtils::MessageServerityFlags         _messageServerity,
				DebugUtils::MessageTypeFlags              _messageType,
				const V1::DebugUtils::Messenger::CallbackData _callbackData,
				ptr<void>                                 _userData
			);

			EFormat FindDepthFormat();

			EFormat FindSupportedFormat(const DynamicArray<EFormat>& _canidates, EImageTiling _tiling, FormatFeatureFlags _features);

			void GenerateMipMaps(Image _image, EFormat _format, uint32 _textureWidth, uint32 _textureHeight, uint32 _mipLevels);

			void Start_GPUComms(RoCStr _applicationName, AppVersion _applicationVersion);

			void Cease_GPUComms();

			void WaitFor_GPUIdle();


			ptr<ARenderContext> GetRenderContext(ptr<OSAL::Window> _window);

			uint32 GetNumberOfFramebuffers();

			uint32 GetMinimumFramebufferCount();

			using RenderCallback = void(*)(CommandBuffer& _buffer, int _index);

			void AddRenderCallback(RenderCallback _callback);

			void Default_InitializeRenderer(ptr<OSAL::Window> _window);

			void Default_ReinitializeRenderer(ptr<OSAL::Window> _window);

			void Default_DeinitializeRenderer(ptr<OSAL::Window> _window);

			void Default_DrawFrame(ptr<OSAL::Window> _window);

		#pragma endregion UnderReview
		}
	}

#endif

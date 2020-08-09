/*
GPU HAL: Vulkan Interface


This currently contains extra implementation for GPU_HAL, I'm not final on my design so
its going to stay here and get modified and slowly moved to VaultedThermals
(If its more associated wit that).

Right now the implementation is heavily hard coded / procedural, this will change with time.
*/

#pragma once


#include "Vulkan_API.hpp"

#include "Meta/AppInfo.hpp"
#include "Meta/Config/HAL_Config.hpp"
#include "Meta/EngineInfo.hpp"
#include "Meta/Config/OSAL_Config.hpp"

#include "Core/IO/Basic_FileIO.hpp"

#include "GPU_HAL_CoreDefs.hpp"

#include "OSAL/Platform.hpp"

#include "OSAL/Windowing.hpp"





#if VULCAN_INTERFACE == VAULTED_THERMALS_INTERFACE

	namespace HAL::GPU
	{
		namespace Vulkan
		{
			using namespace VT;
			using namespace VT::V4;

			using namespace VT::SPIR_V;

			// Usings

			using namespace LAL ;
			using namespace Meta;

			// Vulkan

			//using 

			using ExtensionIdentifierList     = std::vector< RoCStr             >;
			using FenceList                   = std::vector< Fence::Handle      >;
			using FrameBufferList             = std::vector< Framebuffer::Handle>;   
			using ImageList                   = std::vector< Image::Handle      >;
			using ImageViewList               = std::vector< ImageView          >;
			using PhysicalDeviceList          = std::vector< V4::PhysicalDevice >;
			using SemaphoreList               = std::vector< Semaphore::Handle  >;   
			using SurfaceFormatList           = std::vector< Surface::Format    >;
			using SurfacePresentationModeList = std::vector< EPresentationMode  >;
			using ValidationLayerList         = std::vector< RoCStr             >;

			// OS

			using OSAL::Window;


			// Structs

			struct SwapChainSupportDetails
			{
				Surface::Capabilities       Capabilities;
				SurfaceFormatList           Formats;
				SurfacePresentationModeList PresentationModes;
			};

			struct RawRenderContext : ARenderContext
			{
				AppInstance::Handle          ApplicationInstance;
				PhysicalDevice::Handle       PhysicalDevice;
				LogicalDevice::Handle        LogicalDevice;
				uint32                       QueueFamilyIndex;
				LogicalDevice::Queue::Handle Queue;
				Pipeline::Cache::Handle      PipelineCache;
				DescriptorPool::Handle       DescriptorPool;
				EFormat ImageFormat ;
				V4::RenderPass  RenderPass;
				Memory::AllocationCallbacks* Allocator;
				uint32                       MinimumFrameBuffers;
				uint32                       FrameBufferCount;
				Extent2D FrameSize;

				void(*CheckResultFunction)(VkResult returnCode);
			};

			using RenderContextList = DynamicArray<RawRenderContext>;

			
			// Functions

			/*
			Checks to see if the validation layers specified are supported.
			*/
			bool CheckValidationLayerSupport(ValidationLayerList& _validationLayersSpecified);

			void CleanupSwapChain();

			/*
			Creates a GPU application instance. 

			Note: If using optional debug features, both validation layers and the debug messenger must be provided, otherwise this will fail.
			*/
			void CreateApplicationInstance(RoCStr _appName, AppVersion& _version);

			void CreateFrameObjects();

			void CreateFrameBuffers();

			void CreateImage
			(
				uint32                _width,
				uint32                _height,
				uint32                _mipLevels,
				ESampleCount          _numSamples,
				EFormat               _format,
				EImageTiling          _tiling,
				Image::UsageFlags     _usage,
				Memory::PropertyFlags _properties,
				Image& _image,
				Memory& _imageMemory
			);

			ImageView CreateImageView(Image& _image, EFormat _format, Image::AspectFlags _aspectFlags, uint32 _miplevels);

			void CreateLogicalDevice();	

			void CreateSwapChain(OSAL::Window* _window);

			void CreateSyncObjects();

			// Exposed but really should not be used directly unless for another implementation I guess.
			Bool DebugCallback
			(
				MessageServerityFlags   _messageServerity,
				MessageTypeFlags        _messageType,
				const DebugMessenger::CallbackData _callbackData,
				ptr<void>               _userData
			);

			EFormat FindDepthFormat();

			//QueueFamilyIndices FindQueueFamilies(Vault_5::PhysicalDevice _physicalDevice, Surface::Handle _surfaceHandle);

			EFormat FindSupportedFormat(const DynamicArray<EFormat>& _canidates, EImageTiling _tiling, FormatFeatureFlags _features);

			void GenerateMipMaps(Image _image, EFormat _format, uint32 _textureWidth, uint32 _textureHeight, uint32 _mipLevels);

			ExtensionIdentifierList GetRequiredExtensions();

			Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
			CStrArray> GetRequiredVulkanAppExtensions(uint32& _numExtensions);

			//bool IsDeviceSuitable(PhysicalDevice::Handle _deviceHandle, Surface::Handle _surface, ExtensionIdentifierList _extensionsSpecified);
			
			

			void PopulateDebugMessengerCreateInfo(V4::DebugMessenger::CreateInfo& _msngrCreateInfo);

			SwapChainSupportDetails QuerySwapChainSupport(V4::PhysicalDevice& _deviceHandle);

			int RateDeviceSuitability(PhysicalDevice::Handle _deviceHandle);

			/*void SetupDebugMessenger
			(
				AppInstance::Handle _applicationInstance,
				DebugMessenger::Handle& _debugMessenger
			);*/

			Extent2D Surface_SwapChain_ChooseExtent(const Surface::Capabilities& _capabilities, const ptr<Window> _window);

			Surface::Format Surface_SwapChain_ChooseFormat(const SurfaceFormatList& _availableFormats);

			EPresentationMode Surface_SwapChain_ChoosePresentationMode(const SurfacePresentationModeList& _surfacePresentationModes);

			void TransitionImageLayout(Image& _image, EFormat _format, EImageLayout _oldLayout, EImageLayout _newLayout, uint32 _mipMapLevels);

			void Initialize_GPUComms(RoCStr _applicationName, AppVersion _applicationVersion);

			void Cease_GPUComms();

			void WaitFor_GPUIdle();


			ptr<ARenderContext> GetRenderContext(ptr<OSAL::Window> _window);

			uint32 GetNumberOfFramebuffers();

			uint32 GetMinimumFramebufferCount();

			using RenderCallback = void(*)(CommandBuffer& _buffer, int _index);

			void AddRenderCallback(RenderCallback _callback);

			namespace Dirty
			{
				void GetRenderReady(ptr<OSAL::Window> _window);

				void DeInitializeRenderReady(ptr<OSAL::Window> _window);

				void DrawFrame(ptr<OSAL::Window> _window);

				void ReinitializeRenderer(ptr<OSAL::Window> _window);

				CommandBuffer RequestSingleTimeBuffer();

				EResult RequestDescriptorPool(V4::DescriptorPool& _pool, V4::DescriptorPool::CreateInfo _info);

				void EndSingleTimeBuffer(CommandBuffer& _buffer);

				
			}

			eGlobal bool SwapChain_Recreated;
		}
	}

#endif

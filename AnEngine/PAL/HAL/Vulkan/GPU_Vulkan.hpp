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
		namespace Platform_Vulkan
		{
			using namespace VT;
			using namespace VT::Vault_1;

			using namespace VT::SPIR_V;

			// Usings

			using namespace LAL ;
			using namespace Meta;

			// Vulkan

			//using 

			using CommandBufferList           = std::vector< CommandBuffer::Handle>; 
			using ExtensionIdentifierList     = std::vector< RoCStr           >;
			using FenceList                   = std::vector< Fence::Handle          >;
			using FrameBufferList             = std::vector< Framebuffer::Handle    >;   
			using ImageList                   = std::vector< Image::Handle>;
			using ImageViewList               = std::vector< ImageView::Handle>;
			//using PhysicalDeviceList          = std::vector< PhysicalDevice   >;
			using SemaphoreList               = std::vector< Semaphore::Handle      >;   
			using SurfaceFormatList           = std::vector< Surface::Format  >;
			using SurfacePresentationModeList = std::vector< EPresentationMode>;
			using ValidationLayerList         = std::vector< RoCStr           >;

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
				VkPipelineCache              PipelineCache;
				DescriptorPool::Handle       DescriptorPool;
				Memory::AllocationCallbacks* Allocator;
				uint32                       MinimumFrameBuffers;
				uint32                       FrameBufferCount;

				void(*CheckResultFunction)(VkResult returnCode);
			};

			using RenderContextList = DynamicArray<RawRenderContext>;

			
			// Static Data

			//BSS
			//(
				// Moved to an extent from the Vulkan TriangleTest
				// Eventually this will be organized into a proper structural setup for modular use.

				eGlobal Vault_4::AppInstance   AppGPU;
				eGlobal Vault_4::LogicalDevice LogicalDevice;

				eGlobal CommandBufferList CommandBuffers;
				eGlobal VkCommandPool     CommandPool   ;   // TODO: Wrap

				//eGlobal PhysicalDeviceList  PhysicalDevices      ;
				eGlobal DebugMessenger::Handle   DebugMessenger_Handle;

				//eGlobal LogicalDevice::Queue::Handle         GraphicsQueue    ;
				//eGlobal LogicalDevice::Queue::Handle         PresentationQueue;

				//eGlobal QueueFamilyIndices QueueFamilies;

				eGlobal Pipeline::Layout::Handle PipelineLayout;

				eGlobal Pipeline::Handle GraphicsPipeline;

				//eGlobal FenceList InFlightFences;
				//eGlobal FenceList ImagesInFlight;

				//eGlobal SemaphoreList ImageAvailable_Semaphores;
				//eGlobal SemaphoreList RenderFinished_Semaphores;

				eGlobal Surface::Handle SurfaceHandle;

				eGlobal SwapChain::Handle SwapChain;

				eGlobal Extent2D      SwapChain_Extent     ;
				eGlobal EFormat       SwapChain_ImageFormat;

				eGlobal RenderContextList RenderContextPool;    // Contains a reference to every created render context. (Early implementation...)

			//);

			//Data
			//(
				eGlobal bool FramebufferResized;

				eGlobal DataSize CurrentFrame;

				eGlobal sint32 MaxFramesInFlight;

				eGlobal ExtensionIdentifierList DeviceExtensions;

				//eGlobal PhysicalDevice::Handle PhysicalDevice;

				eGlobal ValidationLayerList ValidationLayerIdentifiers;
			//)
						

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

			void CreateCommandPool();

			void CreateFrameBuffers();

			void CreateGraphicsPipeline(StaticArray<ShaderModule::Handle, 2> _shaders);

			void CreateImage
			(
				uint32                         _width,
				uint32                         _height,
				uint32                         _mipLevels,
				ESampleCount                   _numSamples,
				EFormat                        _format,
				EImageTiling                   _tiling,
				Image::UsageFlags              _usage,
				Memory::PropertyFlags          _properties,
				Image::Handle& _image,
				Memory::Handle& _imageMemory
			);

			void CreateImage_V4
			(
				uint32                _width,
				uint32                _height,
				uint32                _mipLevels,
				ESampleCount          _numSamples,
				EFormat               _format,
				EImageTiling          _tiling,
				Image::UsageFlags     _usage,
				Memory::PropertyFlags _properties,
				Vault_4::Image& _image,
				Vault_4::Memory& _imageMemory
			);

			Vault_4::ImageView CreateImageView(Vault_4::Image& _image, EFormat _format, Image::AspectFlags _aspectFlags, uint32 _miplevels);

			void CreateImageViews
			(
				LogicalDevice::Handle _logicalDevice,
				DynamicArray<Vault_4::Image>& _swapChainImages,
				EFormat               _swapChainImageFormat,
				DynamicArray<Vault_4::ImageView>& _imageViewContainer      // Will be populated.
			);

			void CreateLogicalDevice();	

			void CreateSwapChain(OSAL::Window* _window);

			void CreateSyncObjects();

			void CreateVertexBuffers(Buffer::Handle& _vertexBuffer, Memory::Handle& _vertexBufferMemory);

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

			void GenerateMipMaps(Image::Handle _image, EFormat _format, uint32 _textureWidth, uint32 _textureHeight, uint32 _mipLevels);

			ExtensionIdentifierList GetRequiredExtensions();

			Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
			CStrArray> GetRequiredVulkanAppExtensions(uint32& _numExtensions);

			//bool IsDeviceSuitable(PhysicalDevice::Handle _deviceHandle, Surface::Handle _surface, ExtensionIdentifierList _extensionsSpecified);
			
			

			void PopulateDebugMessengerCreateInfo(DebugMessenger::CreateInfo& _msngrCreateInfo);

			SwapChainSupportDetails QuerySwapChainSupport(PhysicalDevice::Handle _deviceHandle, Surface::Handle _surfaceHandle);

			int RateDeviceSuitability(PhysicalDevice::Handle _deviceHandle);

			/*void SetupDebugMessenger
			(
				AppInstance::Handle _applicationInstance,
				DebugMessenger::Handle& _debugMessenger
			);*/

			Extent2D Surface_SwapChain_ChooseExtent(const Surface::Capabilities& _capabilities, const ptr<Window> _window);

			Surface::Format Surface_SwapChain_ChooseFormat(const SurfaceFormatList& _availableFormats);

			EPresentationMode Surface_SwapChain_ChoosePresentationMode(const SurfacePresentationModeList& _surfacePresentationModes);

			void TransitionImageLayout(Image::Handle _image, EFormat _format, EImageLayout _oldLayout, EImageLayout _newLayout, uint32 _mipMapLevels);

			void Initialize_GPUComms(RoCStr _applicationName, AppVersion _applicationVersion);

			void Cease_GPUComms();

			void WaitFor_GPUIdle();

			ptr<ARenderContext> GetRenderContext(ptr<OSAL::Window> _window);


			namespace Dirty
			{
				void GetRenderReady(ptr<OSAL::Window> _window);

				void DeInitializeRenderReady(ptr<OSAL::Window> _window);

				void DrawFrame(ptr<OSAL::Window> _window);

				void ReinitializeRenderer(ptr<OSAL::Window> _window);
			}
		}
	}

#endif

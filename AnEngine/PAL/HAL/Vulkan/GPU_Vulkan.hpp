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
			using namespace VaultedThermals;
			using namespace VaultedThermals::Vault_01;
			using namespace VaultedThermals::SPIR_V;

			// Usings

			using namespace LAL ;
			using namespace Meta;

			// Vulkan

			//using 

			using CommandBufferList           = std::vector< VkCommandBuffer  >;   // TODO: Wrap
			using ExtensionIdentifierList     = std::vector< RoCStr           >;
			using FenceList                   = std::vector< VkFence          >;
			using FrameBufferList             = std::vector< VkFramebuffer    >;   // TODO: Wrap
			using ImageList                   = std::vector< Image::Handle    >;
			using ImageViewList               = std::vector< ImageView::Handle>;
			//using PhysicalDeviceList          = std::vector< PhysicalDevice   >;
			using SemaphoreList               = std::vector< VkSemaphore      >;   // TODO: Wrap
			using SurfaceFormatList           = std::vector< Surface::Format  >;
			using SurfacePresentationModeList = std::vector< EPresentationMode>;
			using ValidationLayerList         = std::vector< RoCStr           >;

			// OS

			using OSAL::Window;


			// Structs

			struct QueueFamilyIndices
			{
				Maybe<uint32> GraphicsFamily;
				Maybe<uint32> PresentationFamily;


				bool IsComplete()
				{
					return
						GraphicsFamily.has_value() &&
						PresentationFamily.has_value()
						;
				}
			};

			struct SwapChainSupportDetails
			{
				Surface::Capabilities       Capabilities;
				SurfaceFormatList           Formats;
				SurfacePresentationModeList PresentationModes;
			};

			struct RenderContext : ARenderContext
			{
				ptr<Window> Window;

				AppInstance::Handle    AppGPU;
				PhysicalDevice::Handle PhysicalDevice;
				LogicalDevice::Handle LogicalDevice;
				LogicalDevice::Queue::Handle GraphicsQueue;
				LogicalDevice::Queue::Handle PresentationQueue;
				QueueFamilyIndices QueueFamilies;
				VkPipelineCache PipelineCache;   // PipelineCache TODO: Wrap the type...
				// DescriptorPool
				const VkAllocationCallbacks* Allocator;
				// MinImageCount
				// 
			};

			using RenderContextList = DynamicArray<RenderContext>;

			
			// Static Data

			//BSS
			//(
				// Moved to an extent from the Vulkan TriangleTest
				// Eventually this will be organized into a proper structural setup for modular use.

				eGlobal AppInstance::Handle AppGPU;

				eGlobal CommandBufferList CommandBuffers;
				eGlobal VkCommandPool     CommandPool   ;   // TODO: Wrap

				//eGlobal PhysicalDeviceList  PhysicalDevices      ;
				eGlobal DebugMessenger::Handle   DebugMessenger_Handle;

				eGlobal LogicalDevice::Handle LogicalDevice    ;
				eGlobal LogicalDevice::Queue::Handle         GraphicsQueue    ;
				eGlobal LogicalDevice::Queue::Handle         PresentationQueue;

				eGlobal QueueFamilyIndices QueueFamilies;

				eGlobal Pipeline::Layout::Handle PipelineLayout;

				eGlobal Pipeline::Handle GraphicsPipeline;

				eGlobal FenceList InFlightFences;
				eGlobal FenceList ImagesInFlight;

				eGlobal SemaphoreList ImageAvailable_Semaphores;
				eGlobal SemaphoreList RenderFinished_Semaphores;

				eGlobal Surface::Handle SurfaceHandle;

				eGlobal SwapChain::Handle SwapChain;

				eGlobal Extent2D      SwapChain_Extent     ;
				eGlobal ImageList     SwapChain_Images     ;
				eGlobal EFormat       SwapChain_ImageFormat;
				eGlobal ImageViewList SwapChain_ImageViews ;

				eGlobal FrameBufferList SwapChain_Framebuffers;

				eGlobal RenderPass::Handle RenderPass;   // TODO: Wrap.

				eGlobal RenderContextList RenderContextPool;    // Contains a reference to every created render context. (Early implementation...)

				eGlobal Buffer::Handle VertexBuffer;
			//);

			//Data
			//(
				eGlobal bool FramebufferResized;

				eGlobal DataSize CurrentFrame;

				eGlobal sint32 MaxFramesInFlight;

				eGlobal ExtensionIdentifierList DeviceExtensions;

				eGlobal PhysicalDevice::Handle PhysicalDevice;

				eGlobal ValidationLayerList ValidationLayerIdentifiers;
			//)
						

			// Functions

			/*
			Checks to see if the validation layers specified are supported.
			*/
			bool CheckValidationLayerSupport(ValidationLayerList& _validationLayersSpecified);

			void CleanupSwapChain
			(
				LogicalDevice::Handle    _logicalDevice,
				Pipeline::Layout::Handle _pipelineLayout,
				VkPipeline               _graphicsPipeline,
				SwapChain::Handle        _swapChain,
				ImageViewList&           _swapChainImageViews,
				FrameBufferList&         _swapChainFramebuffers,
				VkRenderPass             _renderPass,
				VkCommandPool            _commandPool,
				CommandBufferList&       _commandBuffers
			);

			/*
			Creates a GPU application instance. 

			Note: If using optional debug features, both validation layers and the debug messenger must be provided, otherwise this will fail.
			*/
			void CreateApplicationInstance
			(
				RoCStr _appName, 
				AppVersion& _version, 
				const ptr<ValidationLayerList> _optionalValidationLayers, 
				const ptr<DebugMessenger::CreateInfo> _optionalDebugMessengerCreateSpec,
				AppInstance::Handle& _applicationInstance
			);

			void CreateCommandBuffers
			(
				LogicalDevice::Handle _logicalDevice         ,

				VkPipeline            _graphicsPipeline      ,

				FrameBufferList&      _swapChain_FrameBuffers,
				Extent2D              _swapChain_Extent      ,
				VkRenderPass          _renderPass            ,

				VkCommandPool         _commandPool           ,
				CommandBufferList&    _commandBufferContainer
			);

			void CreateCommandPool
			(
				PhysicalDevice::Handle _physicalDevice, 
				LogicalDevice::Handle _logicalDevice, 
				Surface::Handle _surfaceHandle, 
				VkCommandPool& _commandPool
			);

			void CreateFrameBuffers
			(
				LogicalDevice::Handle _logicalDevice,
				VkRenderPass          _renderPass,
				Extent2D              _swapChainExtent,
				ImageViewList& _swapChainImageViews,
				FrameBufferList& _swapChainFrameBuffers
			);

			void CreateGraphicsPipeline
			(
				LogicalDevice::Handle     _logicalDevice   ,
				Extent2D                  _swapChainExtent ,
				StaticArray<ShaderModule::Handle, 2> _shaders,
				Pipeline::Layout::Handle& _pipelineLayout  ,
				RenderPass::Handle        _renderPass      ,
				VkPipeline&               _graphicsPipeline   // Will be provided.
			);

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
				LogicalDevice::Memory::Handle& _imageMemory
			);

			ImageView::Handle CreateImageView(Image::Handle _image, EFormat _format, Image::AspectFlags _aspectFlags, uint32 _miplevels);

			void CreateImageViews
			(
				LogicalDevice::Handle _logicalDevice       ,
				ImageList&            _swapChainImages     , 
				EFormat          _swapChainImageFormat,
				ImageViewList&        _imageViewContainer,      // Will be populated.
				uint32 _mipMapLevels
			);

			void CreateLogicalDevice
			(
				PhysicalDevice::Handle        _physicalDevice          , 
				Surface::Handle               _surfaceHanle            ,
				ExtensionIdentifierList       _extensionsToSpecify     ,
				ptr<ValidationLayerList>      _optionalValidationLayers,
				LogicalDevice::Handle&        _logicalDevice           ,   // Will be provided.
				LogicalDevice::Queue::Handle& _graphicsQueue           ,   // Will be provided.
				LogicalDevice::Queue::Handle& _presentationQueue           // Will be provided.
			);	

			void CreateRenderPass
			(
				LogicalDevice::Handle _logicalDevice,
				EFormat          _imageFormat  ,
				VkRenderPass&         _renderPass       // Will be provided.
			);

			void CreateSurface
			(
				AppInstance::Handle _applicationInstance,
				Window*             _window             , 
				Surface::Handle&    _surfaceHandle          // Will be provided.
			);

			void CreateSwapChain
			(
				OSAL::Window* _window,
				PhysicalDevice::Handle _physicalDevice,
				LogicalDevice::Handle  _logicalDevice,
				Surface::Handle        _surfaceHandle,
				SwapChain::Handle&     _swapChain,
				EFormat&          _swapChainImageFormat,
				Extent2D&              _swapChainExtent,
				ImageList&             _swapChain_Images
			);

			void CreateSyncObjects
			(
				LogicalDevice::Handle _logicalDevicce,
				sint32         _maxFramesInFlight,
				ImageList      _swapChainImages,
				SemaphoreList& _imageAvailableSemaphores,
				SemaphoreList& _renderFinishedSemaphores,
				FenceList&     _inFlightFences,
				FenceList&     _imagesInFlight
			);

			ShaderModule::Handle CreateShaderModule(LogicalDevice::Handle _logicalDevice, const IO::FileBuffer& code);

			StaticArray<ShaderModule::Handle, 2> CreateTriShaders(LogicalDevice::Handle _logicalDevice);

			StaticArray<ShaderModule::Handle, 2> Create_VKTut_V1_Shaders(LogicalDevice::Handle _logicalDevice);

			void CreateVertexBuffers(PhysicalDevice::Handle _physicalDevice, LogicalDevice::Handle _device, Buffer::Handle& _vertexBuffer, Memory::Handle& _vertexBufferMemory);

			// Exposed but really should not be used directly unless for another implementation I guess.
			Bool DebugCallback
			(
				MessageServerityFlags   _messageServerity,
				MessageTypeFlags        _messageType,
				const DebugMessenger::CallbackData _callbackData,
				ptr<void>               _userData
			);

			EFormat FindDepthFormat();

			QueueFamilyIndices FindQueueFamilies(PhysicalDevice::Handle _deviceHandle, Surface::Handle _surfaceHandle);

			uint32 FindMemoryType(PhysicalDevice::Handle _device,  uint32 _typeFilter, Memory::PropertyFlags _properties);

			EFormat FindSupportedFormat(const DynamicArray<EFormat>& _canidates, EImageTiling _tiling, FormatFeatureFlags _features);

			void GenerateMipMaps(Image::Handle _image, EFormat _format, uint32 _textureWidth, uint32 _textureHeight, uint32 _mipLevels);

			ExtensionIdentifierList GetRequiredExtensions();

			Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
			CStrArray> GetRequiredVulkanAppExtensions(uint32& _numExtensions);

			bool IsDeviceSuitable(PhysicalDevice::Handle _deviceHandle, Surface::Handle _surface, ExtensionIdentifierList _extensionsSpecified);
			
			bool PhysicalDevice_CheckExtensionSupport(PhysicalDevice::Handle _handle, ExtensionIdentifierList _extensionsSpecified);
			
			void PickPhysicalDevice(AppInstance::Handle _applicationInstance, PhysicalDevice::Handle& _physicalDevice, Surface::Handle _surface, ExtensionIdentifierList _extensionSpecified);

			void PopulateDebugMessengerCreateInfo(DebugMessenger::CreateInfo& _msngrCreateInfo);

			SwapChainSupportDetails QuerySwapChainSupport(PhysicalDevice::Handle _deviceHandle, Surface::Handle _surfaceHandle);

			int RateDeviceSuitability(PhysicalDevice::Handle _deviceHandle);

			void SetupDebugMessenger
			(
				AppInstance::Handle _applicationInstance,
				DebugMessenger::Handle& _debugMessenger
			);

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

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

#include "OSAL/Platform.hpp"

#include "OSAL/Windowing.hpp"





#if VULCAN_INTERFACE == VAULTED_THERMALS_INTERFACE

	namespace HAL::GPU
	{
		namespace Platform_Vulkan
		{
			// Usings

			using namespace LAL   ;
			using namespace Meta  ;
			using namespace Vulkan;

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

			
			// Static Data

			BSS
			(
				// Moved to an extent from the Vulkan TriangleTest
				// Eventually this will be organized into a proper structural setup for modular use.

				eGlobal AppInstance::Handle AppStateRef;

				eGlobal CommandBufferList CommandBuffers;
				eGlobal VkCommandPool     CommandPool   ;   // TODO: Wrap

				//eGlobal PhysicalDeviceList  PhysicalDevices      ;
				eGlobal Messenger::Handle   DebugMessenger_Handle;

				eGlobal LogicalDevice::Handle        LogicalDevice    ;
				eGlobal LogicalDevice::Queue::Handle GraphicsQueue    ;
				eGlobal LogicalDevice::Queue::Handle PresentationQueue;

				eGlobal Pipeline::Layout::Handle PipelineLayout;

				eGlobal VkPipeline GraphicsPipeline;

				eGlobal FenceList InFlightFences;
				eGlobal FenceList ImagesInFlight;

				eGlobal SemaphoreList ImageAvailable_Semaphores;
				eGlobal SemaphoreList RenderFinished_Semaphores;

				eGlobal Surface::Handle SurfaceHandle;

				eGlobal SwapChain::Handle SwapChain;

				eGlobal Extent2D      SwapChain_Extent     ;
				eGlobal ImageList     SwapChain_Images     ;
				eGlobal EImageFormat  SwapChain_ImageFormat;
				eGlobal ImageViewList SwapChain_ImageViews ;

				eGlobal FrameBufferList SwapChain_Framebuffers;

				eGlobal VkRenderPass RenderPass;   // TODO: Wrap.
			);

			Data
			(
				eGlobal bool FramebufferResized;

				eGlobal DataSize CurrentFrame;

				eGlobal sint32 MaxFramesInFlight;

				eGlobal ExtensionIdentifierList DeviceExtensions;

				eGlobal PhysicalDevice::Handle PhysicalDevice;

				eGlobal ValidationLayerList ValidationLayerIdentifiers;
			)
						

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
				const ptr<Messenger::CreateInfo> _optionalDebugMessengerCreateSpec,
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
				LogicalDevice::Handle    _logicalDevice,
				Extent2D                 _swapChainExtent,
				VkRenderPass             _renderPass,
				Pipeline::Layout::Handle& _pipelineLayout,
				VkPipeline& _graphicsPipeline   // Will be provided.
			);

			void CreateImageViews
			(
				LogicalDevice::Handle _logicalDevice       ,
				ImageList&            _swapChainImages     , 
				EImageFormat          _swapChainImageFormat,
				ImageViewList&        _imageViewContainer      // Will be populated.
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
				EImageFormat          _imageFormat  ,
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
				EImageFormat&          _swapChainImageFormat,
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

			ShaderModule::Handle CreateTriShaderModule(LogicalDevice::Handle _logicalDevice, const IO::FileBuffer& code);

			// Exposed but really should not be used directly unless for another implementation I guess.
			Bool DebugCallback
			(
				MessageServerityFlags   _messageServerity,
				MessageTypeFlags        _messageType,
				const Messenger::CallbackData _callbackData,
				ptr<void>               _userData
			);

			QueueFamilyIndices FindQueueFamilies(PhysicalDevice::Handle _deviceHandle, Surface::Handle _surfaceHandle);

			void FramebufferSizeCallback(Window* _window, int width, int height);

			ExtensionIdentifierList GetRequiredExtensions();

			Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
			CStrArray> GetRequiredVulkanAppExtensions(uint32& _numExtensions);

			bool IsDeviceSuitable(PhysicalDevice::Handle _deviceHandle, Surface::Handle _surface, ExtensionIdentifierList _extensionsSpecified);
			
			bool PhysicalDevice_CheckExtensionSupport(PhysicalDevice::Handle _handle, ExtensionIdentifierList _extensionsSpecified);
			
			void PickPhysicalDevice(AppInstance::Handle _applicationInstance, PhysicalDevice::Handle& _physicalDevice, Surface::Handle _surface, ExtensionIdentifierList _extensionSpecified);

			void PopulateDebugMessengerCreateInfo(Messenger::CreateInfo& _msngrCreateInfo);

			SwapChainSupportDetails QuerySwapChainSupport(PhysicalDevice::Handle _deviceHandle, Surface::Handle _surfaceHandle);

			int RateDeviceSuitability(PhysicalDevice::Handle _deviceHandle);

			void SetupDebugMessenger
			(
				AppInstance::Handle _applicationInstance,
				Messenger::Handle& _debugMessenger
			);

			Extent2D Surface_SwapChain_ChooseExtent(const Surface::Capabilities& _capabilities, const ptr<Window> _window);

			Surface::Format Surface_SwapChain_ChooseFormat(const SurfaceFormatList& _availableFormats);

			EPresentationMode Surface_SwapChain_ChoosePresentationMode(const SurfacePresentationModeList& _surfacePresentationModes);

			void Initalize_GPUComms_Vulkan(RoCStr _applicationName, AppVersion _applicationVersion);
		}
	}

#endif

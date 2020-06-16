/*
Vulkan API

Defines the Vulkan API used with the GPU_HAL.


This currently contains extra implementation for GPU_HAL, I'm not final on my design so 
its going to stay here and get modified and slowly moved to VaultedThermals 
(If its more associated wit that).
*/



#pragma once



#include "Meta/AppInfo.hpp"
#include "Meta/Config/HAL.hpp"
#include "Meta/EngineInfo.hpp"
#include "Meta/Config/OSAL.hpp"
#include "OSAL/Platform.hpp"

#include "OSAL/Windowing.hpp"



#if VULCAN_INTERFACE == VAULTED_THERMALS_INTERFACE

	#include "VaultedThermals/VaultedThermals.hpp"

	namespace HAL::GPU
	{
		namespace Platform_Vulkan
		{
			// Usings

			using namespace LAL   ;
			using namespace Meta  ;
			using namespace Vulkan;

			// Vulkan

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

				eGlobal sInt32 MaxFramesInFlight;

				eGlobal ExtensionIdentifierList DeviceExtensions;

				eGlobal PhysicalDevice::Handle PhysicalDevice;

				eGlobal ValidationLayerList ValidationLayerIdentifiers;
			)
						

			// Functions

			/*
			Checks to see if the validation layers specified are supported.
			*/
			bool CheckValidationLayerSupport(ValidationLayerList& _validationLayersSpecified);

			void CreateApplicationInstance(RoCStr _appName, AppVersion _version, ValidationLayerList _optionalValidationLayers);

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

			void CreateCommandPool(PhysicalDevice::Handle _physicalDevice, VkCommandPool _commandPool);

			void CreateFrameBuffers
			(
				LogicalDevice::Handle _logicalDevice        ,
				VkRenderPass          _renderPass           ,
				Extent2D              _swapChainExtent      ,
				ImageViewList         _swapChainImageViews  ,
				FrameBufferList       _swapChainFrameBuffers
			);

			void CreateGraphicsPipeline
			(
				LogicalDevice::Handle    _logicalDevice   ,
				Extent2D                 _swapChainExtent ,
				VkRenderPass             _renderPass      ,
				Pipeline::Layout::Handle _pipelineLayout  ,
				VkPipeline               _graphicsPipeline
			);

			void CreateImageViews
			(
				LogicalDevice::Handle _logicalDevice       ,
				ImageList             _swapChainImages     ,
				EImageFormat          _swapChainImageFormat,
				ImageViewList         _imageViewContainer
			);

			void CreateLogicalDevice();

			void CreateRenderPass();

			void CreateSurface(AppInstance::Handle _applicationInstance, Surface::Handle _surfaceHandle, Window _window);

			void CreateSwapChain();

			ShaderModule::Handle CreateTriShaderModule(LogicalDevice::Handle _logicalDevice, const IO::FileBuffer& code);

			// Exposed but really should not be used directly unless for another implementation I guess.
			Bool DebugCallback();

			QueueFamilyIndices FindQueueFamilies(PhysicalDevice::Handle _deviceHandle, Surface::Handle _surfaceHandle);

			ExtensionIdentifierList GetRequiredExtensions();

			Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
			CStrArray> GetRequiredVulkanAppExtensions(uint32 _numExtensions);

			void InitializeVulkanApplication(RoCStr _appName, AppVersion _version);

			void PopulateDebugMessengerCreateInfo(Messenger::CreateInfo& _msngrCreateInfo);

			void SetupDebugMessenger();
		}
	}

#endif

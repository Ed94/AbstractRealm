/*
Triangle Test

Last Modified: 5/19/2020

https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code
*/



#pragma once


//#include <vulkan/vulkan.h>

#include "LAL.hpp"
#include "Core/Memory/MemTypes.hpp"
#include "Core/Meta/EngineInfo.hpp"
#include "Core/Meta/Config/HAL_Flags.hpp"
#include "PAL/HAL/Vulkan/Vulkan.hpp"
#include "PAL/SAL/GLFW_SAL.hpp"



namespace Debug
{
	namespace Vulkan_TriangleTest
	{
		using namespace Vulkan;
		using namespace LAL;
		using namespace Core::Meta;

		using AppInfo       = AppInstance::AppInfo   ;
		using AppCreateInfo = AppInstance::CreateInfo;
		using AppHandle     = AppInstance::Handle    ;

		using SAL::GLFW::Window;
		data< ptr<Window> > TriangleWindow = nullptr;

		data<int> Width = 1280, Height = 720;

		bss<AppHandle> App;

		bss<SwapChain::Handle> SwapChain;
		
		using ImageList = std::vector<Image::Handle>;

		ImageList SwapChain_Images;

		EImageFormat SwapChain_ImageFormat;
		Extent2D SwapChain_Extent;

		using ValidationLayerList = std::vector< RoCStr >;

		ValidationLayerList ValidationLayerIdentifiers =
		{
			ValidationLayer_Khronos
		};


		using ExtensionIdentifierList = std::vector< RoCStr >;

		ExtensionIdentifierList DeviceExtensions =
		{
			Swapchain_ExtensionName
		};

		ExtensionIdentifierList GetRequiredExtensions()
		{
			stack<uint32> numExtensions;

			stack<CStrArray> extensionsRequired = CStrArray(SAL::GLFW::GetRequiredVulkanAppExtensions(numExtensions) );

			ExtensionIdentifierList extensions(extensionsRequired, extensionsRequired + numExtensions);

			if (Vulkan_ValidationLayersEnabled)
			{
				extensions.push_back(Vulkan::Debug::Extension_DebugUtility);
			}

			return extensions;
		}

		Where<Vulkan_ValidationLayersEnabled,
		bool> CheckValidationLayerSupport()
		{
			uInt32&& layerCount = GetNumOf_AvailableGlobalLayerProperties();

			using LayerPropertyList = std::vector<LayerProperties>;

			stack<LayerPropertyList> availableLayers(layerCount);

			GetAvailableGlobalLayerProperties(availableLayers.data() );

			stack<bool> layerFound = false;

			for (auto validationLayerName : ValidationLayerIdentifiers)
			{
				for (const auto& layer : availableLayers)
				{
					if (strcmp(validationLayerName, layer.LayerName) == 0)
					{
						layerFound = true;

						break;
					}
				}
			}

			if (!layerFound)
			{
				return false;
			}

			return true;
		}

		using namespace Vulkan::Debug;

		Bool DebugCallback
		(
			      MessageServerityFlags   _messageServerity, 
			      MessageTypeFlags        _messageType     ,
			const Messenger::CallbackData _callbackData    , 
			      ptr<void>               _userData
		)
		{
			std::cerr << "Vulkan: Validation Layer: " << _callbackData.Message << std::endl;

			using Vulkan::EBool;

			return EBool::True;
		}





		void PopulateDebugMessengerCreateInfo(Messenger::CreateInfo& _msngrCreateInfo)
		{
			_msngrCreateInfo.SType = EStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

			using EMaskS = decltype(_msngrCreateInfo.Serverity)::Enum;

			_msngrCreateInfo.Serverity.Set(EMaskS::Verbose, EMaskS::Warning, EMaskS::Error);

			using EMaskT = decltype(_msngrCreateInfo.Type)::Enum;

			_msngrCreateInfo.Type.Set(EMaskT::General, EMaskT::Validation, EMaskT::Performance);

			using addrs = decltype(&DebugCallback);

			_msngrCreateInfo.UserCallback = EnforceConvention(Vulkan::EnforcerID_Vulkan, DebugCallback);

			_msngrCreateInfo.UserData = nullptr;
		}


		bss<Surface::Handle> SurfaceHandle;

		void CreateSurface()
		{
			Surface::CreateInfo createInfo {};

			createInfo.SType = OS_SurfaceCreateInfoEnum;

			createInfo.OSWinHandle = SAL::GLFW::GetOS_WindowHandle(TriangleWindow);

			createInfo.OSAppHandle = GetOS_AppHandle();

			//if (Vulkan::CreateSurface(App, createInfo, nullptr, SurfaceHandle) != EResult::Success) 
			//{
				//throw std::runtime_error("Vulkan, TriangleTest: Failed to create window surface!");
			//}

			if (EResult(SAL::GLFW::CreateWindowSurface(App, TriangleWindow, nullptr, SurfaceHandle)) != EResult::Success)
			{
				throw std::runtime_error("Vulkan, TriangleTest: Failed to create window surface!");
			}
		}

		bss<Messenger::CreateInfo> NotSureYet;
		
		void CreateInstance()
		{
			stack<AppInfo      > appSpec {};
			stack<AppCreateInfo> appCreateSpec {};

			if (Vulkan_ValidationLayersEnabled)
			{
				if (!CheckValidationLayerSupport())
					throw std::runtime_error("Validation layers requested, but are not available!");
			}

			//AppSpec.SType         = EStructureType::ApplicationInformation;   // Use later...
			appSpec.SType         = EStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appSpec.AppName       = "Triangle Test"                       ;
			appSpec.AppVersion    = MakeVersion(1, 0, 0)                  ;
			appSpec.EngineName    = "AnEngine"                            ;
			appSpec.EngineVersion = MakeVersion
			(
				EEngineVersion::Major, 
				EEngineVersion::Minor, 
				EEngineVersion::Patch
			)                                                             ;
			appSpec.API_Version   = EAPI_Version::_1_0                    ;

			//AppCreateSpec.SType   = EStructureType::InstanceCreateInfo;
			appCreateSpec.SType   = EStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			appCreateSpec.AppInfo = getAddress(appSpec)       ;

			/*AppCreateSpec.EnabledExtensionNames = GetRequiredVulkanAppExtensions
			(
				getAddress(AppCreateSpec.EnabledExtensionCount)
			);*/

			ExtensionIdentifierList extensions = GetRequiredExtensions();

			appCreateSpec.EnabledExtensionCount = SCast<uint32>(extensions.size());
			appCreateSpec.EnabledExtensionNames = extensions.data();


			if (Vulkan_ValidationLayersEnabled)
			{
				appCreateSpec.EnabledLayerCount = SCast<uint32>(ValidationLayerIdentifiers.size());

				appCreateSpec.EnabledLayerNames = ValidationLayerIdentifiers.data();

				PopulateDebugMessengerCreateInfo(NotSureYet);

				appCreateSpec.Extension = ptr<Messenger::CreateInfo>(getAddress(NotSureYet));

				//AppCreateSpec.Extension = nullptr;
			}
			else
			{
				appCreateSpec.EnabledLayerCount = 0;

				appCreateSpec.Extension = nullptr;
			}

			EResult&& creationResult = AppInstance_Create(appCreateSpec, nullptr, App);
			//EResult&& creationResult = EResult(vkCreateInstance(&VkInstanceCreateInfo(appCreateSpec), nullptr, getAddress(App)));

			if (creationResult != EResult::Success) 
				throw std::runtime_error("Triangle Test: Failed to create Vulkan app instance.");
		}



		data<PhysicalDevice::Handle> PhysicalDevice = PhysicalDevice::NullHandle();

		struct QueueFamilyIndices
		{
			Maybe<uint32> GraphicsFamily    ;
			Maybe<uint32> PresentationFamily;


			bool IsComplete()
			{
				return
					GraphicsFamily.has_value() &&
					PresentationFamily.has_value()
				;
			}
		};

		using SurfaceFormatList           = std::vector<Surface::Format  >;
		using SurfacePresentationModeList = std::vector<EPresentationMode>;

		struct SwapChainSupportDetails
		{
			Surface::Capabilities       Capabilities     ;
			SurfaceFormatList           Formats          ;
			SurfacePresentationModeList PresentationModes;
		};

		SwapChainSupportDetails QuerySwapChainSupport(PhysicalDevice::Handle _deviceHandle)
		{
			SwapChainSupportDetails details;

			Surface_GetPhysicalDeviceCapabilities(_deviceHandle, SurfaceHandle, details.Capabilities);

			uint32 formatCount;

			formatCount = Surface_GetNumOf_AvailableFormats(_deviceHandle, SurfaceHandle);

			if (formatCount > 0)
			{
				details.Formats.resize(formatCount);

				Surface_GetAvailableFormats(_deviceHandle, SurfaceHandle, details.Formats.data());
			}

			uint32 presentationModeCount;

			presentationModeCount = Surface_GetNumOf_SupportedPresentationModes(_deviceHandle, SurfaceHandle);

			if (presentationModeCount > 0)
			{
				details.PresentationModes.resize(presentationModeCount);

				Surface_GetSupportedPresentationModes(_deviceHandle, SurfaceHandle, details.PresentationModes.data());
			}

			return details;
		}

		QueueFamilyIndices FindQueueFamilies(PhysicalDevice::Handle _deviceHandle)
		{
			QueueFamilyIndices indices{};

			using QueueFamilyPropertiesListing = std::vector<PhysicalDevice::QueueFamilyProperties>;
				
			uint32 queueFamilyCount = GetPhysicalDevice_NumOf_QueueFamilyProperties(_deviceHandle);

			QueueFamilyPropertiesListing queueFamilies(queueFamilyCount);

			GetPhysicalDevice_QueueFamilyProperties(_deviceHandle, queueFamilies.data());

			int index = 0;

			for (const auto& queueFamily : queueFamilies) 
			{
				if ( queueFamily.QueueFlags.Has(decltype(queueFamily.QueueFlags)::Enum::Graphics) )
				{
					indices.GraphicsFamily = index;
				}

				Bool presentationSupport = EBool::False;

				Surface_CheckPhysicalDeviceSupport(_deviceHandle, index, SurfaceHandle, presentationSupport);

				if (presentationSupport)
				{
					indices.PresentationFamily = index;
				}

				if (indices.IsComplete())
				{
					break;
				}

				index++;

				if (index == queueFamilyCount)
				{
					break;
				}
			}

			return indices;
		}

		bool PhysicalDevice_CheckExtensionSupport(PhysicalDevice::Handle _handle)
		{
			using ExtensionPropertiesList = std::vector<ExtensionProperties>;

			ExtensionPropertiesList availableExtensions(PhysicalDevice_GetNumOfAvailableExtensions(_handle));

			PhysicalDevice_GetAvailableExtensions(_handle, availableExtensions.data());

			using ExtensionNameSet = std::set<std::string>;

			ExtensionNameSet requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

			for (const auto& extension : availableExtensions)
			{
				requiredExtensions.erase(extension.ExtensionName);
			}

			bool&& isSupported = requiredExtensions.empty();

			return isSupported;
		}


		bool IsDeviceSuitable(PhysicalDevice::Handle _deviceHandle)
		{
			stack<PhysicalDevice::Properties> deviceProperties;

			GetPhysicalDevice_Properties(_deviceHandle, deviceProperties);

			stack<PhysicalDevice::Features> deviceFeatures;

			GetPhysicalDevice_Features(_deviceHandle, deviceFeatures);

			QueueFamilyIndices indices = FindQueueFamilies(_deviceHandle);

			bool extensionsSupported = PhysicalDevice_CheckExtensionSupport(_deviceHandle);

			bool swapChainAdequate = false;

			if (extensionsSupported)
			{
				SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_deviceHandle);

				swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentationModes.empty();
			}

			bool&& result = 
				bool(deviceFeatures.GeometryShader) &&
				indices.IsComplete()                &&
				extensionsSupported                 &&
				swapChainAdequate                     ;

			return result;
		}

		int RateDeviceSuitability(PhysicalDevice::Handle _deviceHandle)
		{
			stack<PhysicalDevice::Properties> deviceProperties;

			GetPhysicalDevice_Properties(_deviceHandle, deviceProperties);

			stack<PhysicalDevice::Features> deviceFeatures;

			GetPhysicalDevice_Features(_deviceHandle, deviceFeatures);

			int score = 0;

			deviceProperties.Type == EPhysicalDeviceType::DiscreteGPU ? score += 1000 : NULL;

			score += deviceProperties.LimitsSpec.MaxImageDimension2D;

			if (!deviceFeatures.GeometryShader) 
			{
				return 0;
			}

			return score;
		}

		void PickPhysicalDevice()
		{
			stack<uint32> numDevices = GetNumOf_PhysicalDevices(App);

			if (numDevices == 0)
				throw std::runtime_error("Physical device count 0. No GPUs found with vulkan support.");

			using PhysicalDeviceList = std::vector<PhysicalDevice::Handle>;

			PhysicalDeviceList physicalDevices(numDevices);

			GetAvailablePhysicalDevices(App, physicalDevices.data());

			for (const auto& device : physicalDevices)
			{
				if (IsDeviceSuitable(device))
				{
					PhysicalDevice = device;

					break;
				}
			}

			if (PhysicalDevice == PhysicalDevice::NullHandle())
			{
				throw std::runtime_error("Not able to find suitable Vulkan supported GPU.");
			}
		}

		LogicalDevice::Handle LogicalDevice;
		LogicalDevice::Queue::Handle GraphicsQueue;
		LogicalDevice::Queue::Handle PresentationQueue;

		void CreateLogicalDevice()	
		{
			QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);

			using LogicalDevice_QueueCreateInfoList = std::vector<LogicalDevice::Queue::CreateInfo>;

			LogicalDevice_QueueCreateInfoList queueCreateInfos;

			using IndiceSet = std::set<uint32_t>;

			IndiceSet queueFamiliesToCreate =
			{
				indices.GraphicsFamily.value(),
				indices.PresentationFamily.value()
			};

			float queuePriority = 1.0f;

			for (uint32_t queueFamily : queueFamiliesToCreate)
			{
				LogicalDevice::Queue::CreateInfo queueCreateInfo{};

				queueCreateInfo.SType = EStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

				queueCreateInfo.QueueFamilyIndex = queueFamily;

				queueCreateInfo.QueueCount = 1; 

				queueCreateInfo.QueuePriorities = &queuePriority;

				queueCreateInfos.push_back(queueCreateInfo);
			}

			//queueCreateInfo.QueuePriorities = &queuePriority;

			PhysicalDevice::Features physDeviceFeatures{};

			LogicalDevice::CreateInfo createInfo{};

			createInfo.SType = EStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

			createInfo.QueueCreateInfoCount = queueCreateInfos.size();

			createInfo.QueueCreateInfos = queueCreateInfos.data();

			createInfo.EnabledFeatures = &physDeviceFeatures;

			createInfo.EnabledExtensionNames = DeviceExtensions.data();

			createInfo.EnabledExtensionCount = DeviceExtensions.size();

			if (Vulkan_ValidationLayersEnabled)
			{
				createInfo.EnabledLayerCount = SCast<uint32>(ValidationLayerIdentifiers.size());

				createInfo.EnabledLayerNames = ValidationLayerIdentifiers.data();
			}
			else
			{
				createInfo.EnabledLayerCount = 0;
			}

			EResult&& result = LogicalDevice_CreateDevice(PhysicalDevice, createInfo, nullptr, LogicalDevice);

			if (result != EResult::Success)
			{
				if (result != EResult::Success)
				{
					throw std::runtime_error("Vulkan, TraingleTest: Failed to create logical device!");
				}
			}

			LogicalDevice_GetQueue(LogicalDevice, indices.GraphicsFamily    .value(), 0, GraphicsQueue    );
			LogicalDevice_GetQueue(LogicalDevice, indices.PresentationFamily.value(), 0, PresentationQueue);
		}

		bss<Messenger::Handle> MsngrHandle;

		void SetupDebugMessenger()
		{
			if (!Vulkan_ValidationLayersEnabled) return;

			stack<Messenger::CreateInfo> msngrCreateSpec {};

			PopulateDebugMessengerCreateInfo(msngrCreateSpec);

			EResult&& creationResult = CreateMessenger(App, msngrCreateSpec, nullptr, MsngrHandle);
			//EResult&& creationResult = EResult(CreateDebugUtilsMessengerEXT(App, &VkDebugUtilsMessengerCreateInfoEXT(msngrCreateSpec), nullptr, &MsngrHandle));

			if (creationResult != EResult::Success) throw std::runtime_error("Failed to setup debug messenger!");
		}

		Surface::Format Surface_SwapChain_ChooseFormat(const SurfaceFormatList& _availableFormats)
		{
			for (const auto& availableFormat : _availableFormats)
			{
				if 
				(
					availableFormat.Format     == EImageFormat::VK_FORMAT_B8G8R8A8_SRGB &&
					availableFormat.ColorSpace == EColorSpace::KHR_SRGB_NonLinear         
				)
				{
					return availableFormat;
				}
			}

			// Just pick the first format...
			return _availableFormats[0];
		}

		EPresentationMode Surface_SwapChain_ChoosePresentationMode(const SurfacePresentationModeList _surfacePresentationModes)
		{
			for (const auto& availablePresentationMode : _surfacePresentationModes)
			{
				if (availablePresentationMode == EPresentationMode::KHR_Mailbox)
				{
					return availablePresentationMode;
				}
			}

			return EPresentationMode::KHR_FIFO;
		}

		Extent2D Surface_SwapChain_ChooseExtent(const Surface::Capabilities& _capabilities)
		{
			if (_capabilities.CurrentExtent.Width != UInt32Max)
			{
				return _capabilities.CurrentExtent;
			}
			else
			{
				Extent2D actualExtent = { Width, Height };

				actualExtent.Width  = std::clamp(actualExtent.Width , _capabilities.MinImageExtent.Width , _capabilities.MaxImageExtent.Width );
				actualExtent.Height = std::clamp(actualExtent.Height, _capabilities.MinImageExtent.Height, _capabilities.MaxImageExtent.Height);

				return actualExtent;
			}
		}

		void CreateSwapChain()
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(PhysicalDevice);

			Surface::Format surfaceFormat = Surface_SwapChain_ChooseFormat(swapChainSupport.Formats);

			EPresentationMode presentationMode = Surface_SwapChain_ChoosePresentationMode(swapChainSupport.PresentationModes);

			Extent2D extent = Surface_SwapChain_ChooseExtent(swapChainSupport.Capabilities);

			uint32 numImagesDesired = swapChainSupport.Capabilities.MinImageCount;

			numImagesDesired += 1;

			if (swapChainSupport.Capabilities.MaxImageCount > 0 && numImagesDesired > swapChainSupport.Capabilities.MaxImageCount)
			{
				numImagesDesired = swapChainSupport.Capabilities.MaxImageCount;
			}

			SwapChain::CreateInfo creationSpec {};

			creationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			creationSpec.Surface = SurfaceHandle;
			creationSpec.MinImageCount = numImagesDesired;
			creationSpec.ImageFormat = surfaceFormat.Format;
			creationSpec.ImageColorSpace = surfaceFormat.ColorSpace;
			creationSpec.ImageExtent = extent;
			creationSpec.ImageArrayLayers = 1;
			creationSpec.ImageUsage.Set(EImageUsage::Color_Attachment);

			QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);

			uint32_t queueFamilyIndices[] = 
			{
				indices.GraphicsFamily    .value(), 
				indices.PresentationFamily.value() 
			};

			if (indices.GraphicsFamily != indices.PresentationFamily) 
			{
				creationSpec.ImageSharingMode      = ESharingMode::Concurrent;
				creationSpec.QueueFamilyIndexCount = 2;
				creationSpec.QueueFamilyIndices    = queueFamilyIndices;
			}
			else 
			{
				creationSpec.ImageSharingMode      = ESharingMode::Excusive;
				creationSpec.QueueFamilyIndexCount = 0                     ; // Optional
				creationSpec.QueueFamilyIndices    = nullptr               ; // Optional
			}

			creationSpec.PreTransform = swapChainSupport.Capabilities.CurrentTransform;

			creationSpec.CompositeAlpha = ECompositeAlpha::Opaque;

			creationSpec.PresentationMode = presentationMode;

			creationSpec.Clipped = true;

			creationSpec.OldSwapchain = SwapChain::NullHandle();

			EResult&& creationResult = CreateSwapChain(LogicalDevice, creationSpec, nullptr, SwapChain);

			if (creationResult != EResult::Success)
			{
				throw std::runtime_error("Failed to create the swap chain!");
			}

			SwapChain_GetImages(LogicalDevice, SwapChain, SwapChain_Images.data());

			SwapChain_ImageFormat = surfaceFormat.Format;


		}

		void InitWindow()
		{
			using namespace SAL::GLFW;

			Initalize();

			using SAL::GLFW::EBool;

			SetWindowCreationParameter(EWindowCreationParameter::ClientAPI, EClient_API::No_API);
			SetWindowCreationParameter(EWindowCreationParameter::Resizable, EBool      ::False );

			TriangleWindow =
				MakeWindow(Width, Height, "AnEngine: Triangle Test", WindowedMode(), NotShared());
		}

		void InitVulkan()
		{
			CreateInstance();

			SetupDebugMessenger();

			CreateSurface();

			PickPhysicalDevice();

			CreateLogicalDevice();

			CreateSwapChain();
		}

		void MainLoop()
		{
			using namespace SAL::GLFW;

			while (! CanClose(TriangleWindow))
			{
				PollEvents();
			}
		}

		void Cleanup()
		{
			using namespace SAL::GLFW;

			DestroySwapChain(LogicalDevice, SwapChain, nullptr);

			LogicalDevice_Destory(LogicalDevice, nullptr);

			if (Vulkan_ValidationLayersEnabled) DestroyMessenger(App, MsngrHandle, nullptr);

			DestroySurface(App, SurfaceHandle, nullptr);

			AppInstance_Destory(App, nullptr);

			DestroyWindow(TriangleWindow);

			Terminate();
		}

		int Run()
		{
			try
			{
				InitWindow();

				InitVulkan();

				MainLoop();

				Cleanup();
			}
			catch (const std::exception& error)
			{
				std::cerr << error.what() << std::endl;

				getchar();

				return EXIT_FAILURE;
			}
		}
	}
}

/*
Triangle Test

Last Modified: 5/19/2020

https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code
*/



#pragma once


#include <vulkan/vulkan.h>

#include "LAL.hpp"
#include "Core/Memory/MemTypes.hpp"
#include "Core/Meta/EngineInfo.hpp"
#include "Core/Meta/Config/HAL_Flags.hpp"
#include "PAL/SAL/GLFW_SAL.hpp"
#include "PAL/HAL/Vulkan/Vulkan.hpp"



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


		using ValidationLayerList = std::vector< RoCStr >;

		ValidationLayerList ValidationLayerIdentifiers = 
		{
			ValidationLayer_Khronos
		};


		using ExtensionIdentifierList = std::vector< RoCStr >;

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

			_msngrCreateInfo.UserCallback = EnforceConvention(Vulkan, DebugCallback);

			_msngrCreateInfo.UserData = nullptr;
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

			if (creationResult != EResult::Sucess) 
				throw std::runtime_error("Triangle Test: Failed to create Vulkan app instance.");
		}


		data<PhysicalDevice::Handle> PhysicalDevice = PhysicalDevice::NullHandle();

		struct QueueFamilyIndices
		{
			Maybe<uint32> GraphicsFamily;


			bool IsComplete()
			{
				return
					GraphicsFamily.has_value()
					;
			}
		};



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


		bool IsDeviceSuitable(PhysicalDevice::Handle _deviceHandle)
		{
			stack<PhysicalDevice::Properties> deviceProperties;

			GetPhysicalDevice_Properties(_deviceHandle, deviceProperties);

			stack<PhysicalDevice::Features> deviceFeatures;

			GetPhysicalDevice_Features(_deviceHandle, deviceFeatures);

			QueueFamilyIndices indices = FindQueueFamilies(_deviceHandle);

			bool&& result = 
				bool(deviceFeatures.GeometryShader) &&
				indices.GraphicsFamily.has_value()    ;

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
				throw std::runtime_error("Not able to find stuitable Vulkan supported GPU.");
			}
		}


		LogicalDevice::Handle LogicalDevice;
		LogicalDevice::Queue::Handle GraphicsQueue;

		void CreateLogicalDevice()
		{
			QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);

			LogicalDevice::Queue::CreateInfo queueCreateInfo {};

			queueCreateInfo.SType = EStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

			queueCreateInfo.QueueFamilyIndex = indices.GraphicsFamily.value();

			queueCreateInfo.QueueCount = 1;

			float queuePriority = 1.0f;

			queueCreateInfo.QueuePriorities = &queuePriority;

			PhysicalDevice::Features physDeviceFeatures {};

			LogicalDevice::CreateInfo createInfo {};

			createInfo.SType = EStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

			createInfo.QueueCreateInfos = &queueCreateInfo;

			createInfo.QueueCreateInfoCount = 1;

			createInfo.EnabledFeatures = &physDeviceFeatures;

			createInfo.EnabledExtensionCount = 0;

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

			if (result != EResult::Sucess)
			{
				throw std::runtime_error("Vulkan, TraingleTest: Failed to create logical device!");
			}
		}

		

		


		bss<Messenger::Handle> MsngrHandle;

		void SetupDebugMessenger()
		{
			if (!Vulkan_ValidationLayersEnabled) return;

			stack<Messenger::CreateInfo> msngrCreateSpec {};

			PopulateDebugMessengerCreateInfo(msngrCreateSpec);

			EResult&& creationResult = CreateMessenger(App, msngrCreateSpec, nullptr, MsngrHandle);
			//EResult&& creationResult = EResult(CreateDebugUtilsMessengerEXT(App, &VkDebugUtilsMessengerCreateInfoEXT(msngrCreateSpec), nullptr, &MsngrHandle));

			if (creationResult != EResult::Sucess) throw std::runtime_error("Failed to setup debug messenger!");
		}

		void InitWindow()
		{
			using namespace SAL::GLFW;

			Initalize();

			using SAL::GLFW::EBool;

			SetWindowCreationParameter(EWindowCreationParameter::ClientAPI, EClient_API::No_API);
			SetWindowCreationParameter(EWindowCreationParameter::Resizable, EBool      ::False );

			TriangleWindow =
				CreateWindow(Width, Height, "AnEngine: Triangle Test", WindowedMode(), NotShared());
		}

		void InitVulkan()
		{
			CreateInstance();

			SetupDebugMessenger();

			PickPhysicalDevice();

			CreateLogicalDevice();
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

			LogicalDevice_Destory(LogicalDevice, nullptr);

			if (Vulkan_ValidationLayersEnabled) DestroyMessenger(App, MsngrHandle, nullptr);

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

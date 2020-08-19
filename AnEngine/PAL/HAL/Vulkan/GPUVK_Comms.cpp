// Parent Header
#include "GPUVK_Comms.hpp"


// Engine
#include "OSAL_Platform.hpp"
#include "OSAL_Windowing.hpp"
#include <map>



namespace HAL::GPU::Vulkan
{
	// Forwards

	void AquireSupportedValidationLayers();

	bool CheckLayerSupport(DynamicArray<RoCStr> _layersSpecified);

	void DetermineRequiredExtensions();

	Bool DebugCallback_Internal
	(
			  MessageServerityFlags             _messageServerity, 
			  MessageTypeFlags                  _messageType     ,
		const V1::DebugMessenger::CallbackData  _callbackData, 
			  ptr<void>                         _userData
	);

	void SetupDebugMessenger();



	// Class Implementation

#pragma region PhysicalDevice

	EResult PhysicalDevice::GetAvailableExtensions()
	{
		layersAndExtensions = AppLayersAndExtensions;

		EResult result = EResult::Incomplete;

		for (auto& layerAndExtensions : layersAndExtensions)
		{
			result = Parent::GetAvailableExtensions(layerAndExtensions.Layer.Name, layerAndExtensions.Extensions);

			if (result != EResult::Success) return result;
		}

		return result;
	}

	void PhysicalDevice::GetAvailableQueueFamilies()
	{
		queueFamilies = Parent::GetAvailableQueueFamilies();
	}

#pragma endregion PhysicalDevice

#pragma region AppInstance

	EResult AppInstance::GetAvailablePhysicalDevices(DynamicArray<PhysicalDevice>& _deviceListing) const
	{
		uint32 count; std::vector<PhysicalDevice::Handle> handleList;

		EResult returnCode = QueryPhysicalDeviceListing(&count, nullptr);

		if (returnCode != EResult::Success) return returnCode;

		handleList    .resize(count);
		_deviceListing.resize(count);

		returnCode = QueryPhysicalDeviceListing(&count, handleList.data());

		for (DeviceSize index = 0; index < count; index++)
		{
			_deviceListing[index].AssignHandle(handleList[index]);
		}

		return returnCode;
	}

#pragma endregion AppInstance

#pragma region LogicalDevice

	// Public

	void LogicalDevice::AssignPhysicalDevice(PhysicalDevice& _physicalDevice)
	{ 
		physicalDevice = &_physicalDevice; 

		info.EnabledFeatures = &physicalDevice->GetFeatures();

		PrepareQueues();

		ProcessExtensionSupport();

		ProcessLayerSupport();
	}

	EResult LogicalDevice::Create()
	{
		V4::PhysicalDevice& ref = *(V4::PhysicalDevice*)(physicalDevice);

		EResult result = Parent::Parent::Create(physicalDevice->GetHandle(), info, handle);

		if (result != EResult::Success) return result;

		if (graphicsQueue.FamilySpecified()) graphicsQueue.Retrieve();
		if (computeQueue .FamilySpecified()) computeQueue .Retrieve();
		if (transferQueue.FamilySpecified()) transferQueue.Retrieve();

		return result;
	}

	bool LogicalDevice::ExtensionsEnabled(DynamicArray<RoCStr> _extensions)
	{
		for (auto& enabledExtension : _extensions)
		{
			DataSize index = 0;

			for (auto& extension : _extensions)
			{
				if (CStr_Compare(enabledExtension, extension) == 0)
				{
					_extensions.erase(_extensions.begin() + index);
				}

				index++;
			}
		}

		if (_extensions.size() > 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	const PhysicalDevice& LogicalDevice::GetPhysicalDevice() const
	{
		PhysicalDevice* intermed = SCast<PhysicalDevice*>(physicalDevice);

		return *intermed;
	}

	const LogicalDevice::Queue& LogicalDevice::GetQueue(EQueueFlag _type) const
	{
		switch (_type)
		{
			case EQueueFlag::Graphics:
			{
				return graphicsQueue;
			}
			case EQueueFlag::Transfer:
			{
				return transferQueue;
			}
			default:
				throw RuntimeError("Attempted to queue that is not supported by this logical device.");
		}
	}

	const LogicalDevice::Queue& LogicalDevice::GetGraphicsQueue() const
	{
		return graphicsQueue;
	}

	// Protected

	void LogicalDevice::PrepareQueues()
	{
		// Queue Family Assignment

		uint32 familyIndex = 0;

		for (auto& queueFamily : physicalDevice->GetAvailableQueueFamilies())
		{
			uint32 queueCount = 0;

			QueueFamily       family {};
			Queue::CreateInfo info   {};

			family.Priority = 1.0f;

			info.QueueFamilyIndex = familyIndex;
			info.QueueCount       = 1          ;   // For now only getting one queue of any type.

			if (queueFamily.QueueFlags.Has(EQueueFlag::Graphics))
			{
				queueFamilyInfos.push_back(info);

				family.Info = getAddress(queueFamilyInfos.back());

				family.Assignment = EQueueFlag::Graphics;

				queueFamilies.push_back(family);
			}

			if (queueFamily.QueueFlags == QueueMask::ComputeAsync)
			{
				queueFamilyInfos.push_back(info);

				family.Info = getAddress(queueFamilyInfos.back());

				family.Assignment = EQueueFlag::Compute;

				queueFamilies.push_back(family);
			}

			if (queueFamily.QueueFlags == QueueMask::TransferOnly)
			{
				queueFamilyInfos.push_back(info);

				family.Info = getAddress(queueFamilyInfos.back());

				family.Assignment = EQueueFlag::Transfer;

				queueFamilies.push_back(family);
			}

			familyIndex++;
		}

		for (DataSize index = 0; index < queueFamilyInfos.size(); index++)
		{
			queueFamilyInfos[index].QueuePriorities = getAddress(queueFamilies[index].Priority);	

			switch (queueFamilies[index].Assignment)
			{
				case EQueueFlag::Graphics:
				{
					graphicsQueue.Assign(dref(this), queueFamilyInfos[index], 0, EQueueFlag::Graphics);

					break;
				}
				case EQueueFlag::Compute:
				{
					computeQueue.Assign(dref(this), queueFamilyInfos[index], 0, EQueueFlag::Compute);

					break;
				}
				case EQueueFlag::Transfer:
				{
					transferQueue.Assign(dref(this), queueFamilyInfos[index], 0, EQueueFlag::Transfer);

					break;
				}
			}
		}

		info.QueueCreateInfoCount = queueFamilyInfos.size();
		info.QueueCreateInfos     = queueFamilyInfos.data();
	}

	void LogicalDevice::ProcessExtensionSupport()
	{
		if (physicalDevice->CheckExtensionSupport(DesiredDeviceExts))
		{
			extensionsEnabled = DesiredDeviceExts;
		}
		else
		{
			for (auto& extensionName : DesiredDeviceExts)
			{
				if (physicalDevice->CheckExtensionSupport(extensionName))
				{
					extensionsEnabled.push_back(extensionName);

				}
			}			
		}

		info.EnabledExtensionCount = extensionsEnabled.size();
		info.EnabledExtensionNames = extensionsEnabled.data();
	}

	void LogicalDevice::ProcessLayerSupport()
	{
		if (Meta::Vulkan::EnableLayers)
		{
			info.EnabledLayerCount = SCast<uint32>(DesiredLayers.size());
			info.EnabledLayerNames = DesiredLayers.data();
		}
		else
		{
			info.EnabledLayerCount = 0;
		}
	}

#pragma endregion LogicalDevice



	StaticData
	(
		AppInstance            AppGPU_Comms          ;
		LayerandExtensionsList AppLayersAndExtensions;
		DynamicArray<RoCStr>   DesiredLayers         ;
		DynamicArray<RoCStr>   DesiredInstanceExts   ;
		DynamicArray<RoCStr>   DesiredDeviceExts     ;

		V4::DebugMessenger DebugMessenger;

		PhysicalDeviceList PhysicalGPUs;
		LogicalDeviceList  LogicalGPUs ;

		ptr<LogicalDevice> DeviceEngaged = nullptr;
	)



#pragma region Public

	void AcquirePhysicalDevices()
	{
		stack<EResult> result = AppGPU_Comms.GetAvailablePhysicalDevices(PhysicalGPUs);

		if (result != EResult::Success)
			throw RuntimeError("Failed to get the physical GPUs.");

		for (auto& gpu : PhysicalGPUs)
		{
			gpu.GetAvailableExtensions   ();
			gpu.GetAvailableQueueFamilies();
		}
	}

	void AppGPU_Comms_Initialize(RoCStr _appName, Meta::AppVersion _version)
	{
		Stack
		(
			AppInstance::AppInfo    spec       {};
			AppInstance::CreateInfo createSpec {};
		)

		AppInstance::GetAvailableLayersAndExtensions(AppLayersAndExtensions);

		if (Meta::Vulkan::EnableLayers)
		{
			// In order to process receive layer messages, need the debugger.
			DesiredInstanceExts.push_back(InstanceExt::DebugUtility);

			if (Meta::Vulkan::Enable_API_Dump)
				DesiredLayers.push_back(Layer::LunarG_API_Dump);

			if (Meta::Vulkan::Enable_FPSMonitor)
				DesiredLayers.push_back(Layer::LunarG_Monitor);

			if (Meta::Vulkan::Enable_Validation)
				AquireSupportedValidationLayers();
				
			if (!CheckLayerSupport(DesiredLayers))
			{
				throw std::runtime_error("Layers requested, but are not available!");
			}
		}

		spec.AppName       = _appName                                                   ;
		spec.AppVersion    = MakeVersion(_version.Major, _version.Minor, _version.Patch);
		spec.EngineName    = Meta::EngineName                                           ;
		spec.EngineVersion = MakeVersion
		(
			Meta::EEngineVersion::Major, 
			Meta::EEngineVersion::Minor, 
			Meta::EEngineVersion::Patch
		);         

		spec.API_Version = EAPI_Version::_1_0;

		createSpec.AppInfo = getAddress(spec);

		DetermineRequiredExtensions();

		createSpec.EnabledExtensionCount = SCast<uint32>(DesiredInstanceExts.size());
		createSpec.EnabledExtensionNames = DesiredInstanceExts.data()               ;

		if (Meta::Vulkan::EnableLayers)
		{
			createSpec.EnabledLayerCount = SCast<uint32>(DesiredLayers.size());
			createSpec.EnabledLayerNames = DesiredLayers.data();

			SetupDebugMessenger();

			createSpec.Next = getAddress(DebugMessenger.GetInfo());
		}
		else
		{
			createSpec.EnabledLayerCount = 0;

			createSpec.Next = nullptr;
		}

		stack<EResult> creationResult = AppGPU_Comms.Create(spec, createSpec);  

		if (creationResult != EResult::Success) 
			throw std::runtime_error("Triangle Test: Failed to create Vulkan app instance.");

		if (Meta::Vulkan::EnableLayers)
		{
			creationResult = DebugMessenger.Create(AppGPU_Comms);

			if (creationResult != EResult::Success)
				throw std::runtime_error("Failed to setup debug messenger!");
		}
	}

	void EngageMostSuitableDevice()
	{
		ptr<OSAL::Window> testWindow;

		OSAL::WindowInfo windowSpec = {};

		windowSpec.WindowTitle = "GPU_Vulkan: SurfaceSupportTest";
		windowSpec.WindowSize  = { 10, 10 }                      ;
		windowSpec.Windowed    = OSAL::WindowInfo ::WindowedMode ;
		windowSpec.Resizable   = OSAL::WinBool::True             ;

		testWindow = OSAL::Create_Window(windowSpec);

		Surface testSurface;

		Surface::CreateInfo createInfo{};

		createInfo.OSWinHandle = OSAL::GetOS_WindowHandle(testWindow);
		createInfo.OSAppHandle = Surface::GetOS_AppHandle();

		if (testSurface.Create(AppGPU_Comms, createInfo) != EResult::Success)
		{
			throw std::runtime_error("Vulkan, TriangleTest: Failed to create window surface!");
		}

		for (auto& device : LogicalGPUs)
		{
			bool HasExtensions = device.ExtensionsEnabled(DesiredDeviceExts);

			Bool PresentationSupport; 

			testSurface.CheckPhysicalDeviceSupport
			(
				device.GetPhysicalDevice().GetHandle(),
				device.GetQueue(EQueueFlag::Graphics).GetFamilyIndex(),
				PresentationSupport
			);

			if (HasExtensions && PresentationSupport) 
			{
				DeviceEngaged = getAddress(device);
			}
		}

		if (DeviceEngaged == nullptr)
		{
			throw RuntimeError("Failed to engage a suitable logical device.");
		}

		testSurface.Destroy();

		OSAL::Destroy_Window(testWindow);
	}

	void GenerateLogicalDevices()
	{
		LogicalGPUs.resize(PhysicalGPUs.size());

		uint32 gpuIndex = 0;

		for (auto& device : LogicalGPUs)
		{
			// Assigns the physical device,
			// prepares the queues,
			// sets the features to be enabled,
			// specifies extensions to support (from those desired),
			// and processes layer support.
			device.AssignPhysicalDevice(PhysicalGPUs[gpuIndex]);

			// Creates the logical device, and the queues retrieve their respective data.
			EResult result = device.Create();

			if (result != EResult::Success)
			{
				throw RuntimeError("Failed to create logical device!");
			}
		}
	}

	const LogicalDevice& GetEngagedDevice()
	{
		return dref(DeviceEngaged);
	}

	const PhysicalDevice& GetEngagedPhysicalGPU()
	{
		return DeviceEngaged->GetPhysicalDevice();
	}

#pragma endregion Public

#pragma region Private

	void AquireSupportedValidationLayers()
	{
		bool found = false;

		// Ideal

		for (const auto& layerAndExtenions : AppLayersAndExtensions)
		{
			if (CStr_Compare(Layer::Khronos_Validation, layerAndExtenions.Layer.Name) == 0)
			{
				DesiredLayers.push_back(Layer::Khronos_Validation);

				found = true;

				break;
			}
		}

		// Fallback 1

		if (!found)
		{
			for (const auto& layerAndExtenions : AppLayersAndExtensions)
			{
				if (CStr_Compare(Layer::LunarG_StandardValidation, layerAndExtenions.Layer.Name) == 0)
				{
					DesiredLayers.push_back(Layer::LunarG_StandardValidation);

					found = true;

					break;
				}
			}
		}

		// Fallback 2

		if (!found)
		{
			StaticArray<RoCStr, 4> Fallback2Layers 
			{
				Layer::LunarG_ParameterValidation,
				Layer::LunarG_ObjectTracker      ,
				Layer::Google_Threading          ,
				Layer::Google_UniqueObjedcts
			};

			DataSize layersFound = 0;

			for (auto validationLayerName : Fallback2Layers)
			{
				for (const auto& layerAndExtenions : AppLayersAndExtensions)
				{
					if (CStr_Compare(validationLayerName, layerAndExtenions.Layer.Name) == 0)
					{
						layersFound++;

						break;
					}
				}
			}

			if (layersFound == Fallback2Layers.size()) found = true;
		}

		// Fallback 3

		if (!found)
		{
			for (const auto& layerAndExtenions : AppLayersAndExtensions)
			{
				if (CStr_Compare(Layer::LunarG_CoreValidation, layerAndExtenions.Layer.Name) == 0)
				{
					DesiredLayers.push_back(Layer::LunarG_CoreValidation);

					found = true;

					break;
				}
			}
		}
	}

	bool CheckLayerSupport(DynamicArray<RoCStr> _layersSpecified)
	{
		stack<DataSize> layersFound = 0;

		for (auto validationLayerName : _layersSpecified)
		{
			for (const auto& layerAndExtenions : AppLayersAndExtensions)
			{
				if (CStr_Compare(validationLayerName, layerAndExtenions.Layer.Name) == 0)
				{
					layersFound++;

					break;
				}
			}
		}

		if (!layersFound == _layersSpecified.size())
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	/*
	I had to make this to resolve an issue with a decltype function resolve error on EnforceConvention.
	*/
	Bool DebugCallback_Internal
	(
		      MessageServerityFlags            _messageServerity, 
		      MessageTypeFlags                 _messageType     ,
		const V1::DebugMessenger::CallbackData _callbackData    , 
		      ptr<void>                        _userData
	)
	{
		std::cerr << "Vulkan: Validation Layer: " << _callbackData.Message << std::endl;

		//Dev::CLog("Vulkan Validation Layer: " + String(_callbackData.Message));

		return EBool::True;
	}

	void DetermineRequiredExtensions()
	{
		switch (OSAL::WindowingPlatform)
		{
			case Meta::EWindowingPlatform::GLFW:
			{
				stack<uint32> numExtensions;

				stack<CStrArray> extensions = CStrArray(SAL::GLFW::GetRequiredVulkanAppExtensions(numExtensions));

				for (uint32 index = 0; index < numExtensions; index++)
				{
					if (std::find(DesiredInstanceExts.begin(), DesiredInstanceExts.end(), extensions[index]) == DesiredInstanceExts.end())
					{
						DesiredInstanceExts.push_back(extensions[index]);
					}
				}

				break;
			}
			case Meta::EWindowingPlatform::OSAL:
			{
				// A GPU must be available with ability to render to a surface.
				DesiredInstanceExts.push_back(InstanceExt::Surface);

				// Surface OS platform extension.
				DesiredInstanceExts.push_back(Surface::OSSurface);

				break;
			}
		}

		// Engine needs swap chains.
		DesiredDeviceExts.push_back(DeviceExt::Swapchain);

		if (Meta::Vulkan::EnableLayers)
		{
			DesiredInstanceExts.push_back(InstanceExt::DebugUtility);
		}
	}

	void SetupDebugMessenger()
	{
		stack<DebugMessenger::CreateInfo> info{};

		using EMaskS = decltype(info.Serverity)::Enum;

		info.Serverity.Set(EMaskS::Verbose, EMaskS::Warning, EMaskS::Error);

		using EMaskT = decltype(info.Type)::Enum;

		info.Type.Set(EMaskT::General, EMaskT::Validation, EMaskT::Performance);

		info.UserCallback = EnforceConvention(EnforcerID_Vulkan, DebugCallback_Internal);

		info.UserData = nullptr;

		DebugMessenger.AssignInfo(info);
	}
}

#pragma endregion Private

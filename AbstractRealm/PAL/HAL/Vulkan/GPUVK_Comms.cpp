// Parent Header
#include "GPUVK_Comms.hpp"


#include "OSAL/OSAL.hpp"


namespace HAL::GPU::Vulkan
{
#pragma region StaticData

	AppInstance            AppGPU_Comms          ;
	LayerandExtensionsList AppLayersAndExtensions;
	DynamicArray<RoCStr>   DesiredLayers         ;
	DynamicArray<RoCStr>   DesiredInstanceExts   ;
	DynamicArray<RoCStr>   DesiredDeviceExts     ;

	DebugUtils::Messenger GPU_Messenger_Verbose;
	DebugUtils::Messenger GPU_Messenger_Info;
	DebugUtils::Messenger GPU_Messenger_Warning;
	DebugUtils::Messenger GPU_Messenger_Error;

	PhysicalDeviceList PhysicalGPUs;
	LogicalDeviceList  LogicalGPUs ;

	/*  Currently the design of the Vulkan backend is monolithic.
		Only one device of those available is engaged at a time */
	ptr<LogicalDevice> DeviceEngaged = nullptr;
	
#pragma endregion StaticData


	// Forwards

	//void AquireSupportedValidationLayers();

	//bool CheckLayerSupport(DynamicArray<RoCStr> _layersSpecified);

	//void DetermineRequiredExtensions();

	//void SetupDebugMessenger();



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

	const LayerandExtensionsList& PhysicalDevice::GetLayersAndExtensions() const
	{
		return layersAndExtensions;
	}

	PhysicalDevice::operator Parent& ()
	{
		return *static_cast<Parent*>(this);
	}

	PhysicalDevice::operator const Parent& () const
	{
		return *static_cast<const Parent*>(this);
	}

#pragma endregion PhysicalDevice

#pragma region DebugUtils

	void DebugUtils::Messenger::AssignInfo(const CreateInfo& _info) 
	{ 
		info = _info; 
	}

	EResult DebugUtils::Messenger::Create(const AppInstance& _app)
	{
		return Parent::Create(_app, info);
	}

	EResult DebugUtils::Messenger::Create(const AppInstance& _app, const Memory::AllocationCallbacks& _allocator)
	{
		return Parent::Create(_app, info, _allocator);
	}

	const DebugUtils::Messenger::CreateInfo& DebugUtils::Messenger::GetInfo() const 
	{ 
		return info; 
	}

#pragma endregion DebugUtils

#pragma region AppInstance

	EResult AppInstance::GetAvailablePhysicalDevices(DynamicArray<PhysicalDevice>& _deviceListing) const
	{
		ui32 count; DynamicArray<PhysicalDevice::Handle> handleList;

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
		EResult result = Parent::Parent::Create(*physicalDevice, info, handle);

		if (result != EResult::Success) return result;

		if (graphicsQueue.FamilySpecified()) graphicsQueue.Retrieve();
		if (computeQueue .FamilySpecified()) computeQueue .Retrieve();
		if (transferQueue.FamilySpecified()) transferQueue.Retrieve();

		return result;
	}

	bool LogicalDevice::ExtensionsEnabled(DynamicArray<RoCStr> _extensions)
	{
		uDM extensionsLeft = _extensions.size();

		for (auto& enabledExtension : _extensions)
		{
			uDM index = 0;

			for (auto& extension : _extensions)
			{
				if (CStrCompare(enabledExtension, extension) == 0)
				{
					extensionsLeft--;
				}

				index++;
			}
		}

		if (extensionsLeft > 0)
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
		ptr<const PhysicalDevice> intermed = SCast<const PhysicalDevice>(physicalDevice);

		return dref(intermed);
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
				Exception::Fatal::Throw("Attempted to use queue that is not supported by this logical device.");
		}
	}

	const LogicalDevice::Queue& LogicalDevice::GetGraphicsQueue() const
	{
		return graphicsQueue;
	}

	const LogicalDevice::Queue& LogicalDevice::GetComputeQueue() const
	{
		return computeQueue;
	}

	const LogicalDevice::Queue& LogicalDevice::GetTransferQueue() const
	{
		return transferQueue;
	}

	const String LogicalDevice::GetSig() const
	{
		StringStream handleStr; handleStr << handle;

		return physicalDevice->GetProperties().Name + String(" Handle: ") + handleStr.str();
	}

	LogicalDevice::operator Handle()
	{
		return handle;
	}

	LogicalDevice::operator const Handle& () const
	{
		return handle;
	}

	LogicalDevice::operator Parent& ()
	{
		return *reinterpret_cast<Parent*>(this);
	}

	LogicalDevice::operator const Parent& () const
	{
		return *reinterpret_cast<const Parent*>(this);
	}

	// Protected

	void LogicalDevice::PrepareQueues()
	{
		// Queue Family Assignment

		u32 familyIndex = 0;

		for (auto& queueFamily : physicalDevice->GetAvailableQueueFamilies())
		{
			QueueFamily       family    {};
			Queue::CreateInfo queueInfo {};

			family.Priority = 1.0f;

			queueInfo.QueueFamilyIndex = familyIndex;
			queueInfo.QueueCount       = 1          ;   // For now only getting one queue of any type.

			if (queueFamily.QueueFlags.HasFlag(EQueueFlag::Graphics))
			{
				queueFamilyInfos.push_back(queueInfo);

				family.Info = getPtr(queueFamilyInfos.back());

				family.Assignment = EQueueFlag::Graphics;

				queueFamilies.push_back(family);
			}

			if (queueFamily.QueueFlags == QueueMask::ComputeAsync)
			{
				queueFamilyInfos.push_back(queueInfo);

				family.Info = getPtr(queueFamilyInfos.back());

				family.Assignment = EQueueFlag::Compute;

				queueFamilies.push_back(family);
			}

			if (queueFamily.QueueFlags == QueueMask::TransferOnly)
			{
				queueFamilyInfos.push_back(queueInfo);

				family.Info = getPtr(queueFamilyInfos.back());

				family.Assignment = EQueueFlag::Transfer;

				queueFamilies.push_back(family);
			}

			familyIndex++;
		}

		for (uDM index = 0; index < queueFamilyInfos.size(); index++)
		{
			queueFamilyInfos[index].QueuePriorities = getPtr(queueFamilies[index].Priority);	

			switch (queueFamilies[index].Assignment)
			{
				case EQueueFlag::Graphics:
				{
					graphicsQueue.Assign(dref(this), queueFamilyInfos[index].QueueFamilyIndex, 0, EQueueFlag::Graphics);

					break;
				}
				case EQueueFlag::Compute:
				{
					computeQueue.Assign(dref(this), queueFamilyInfos[index].QueueFamilyIndex, 0, EQueueFlag::Compute);

					break;
				}
				case EQueueFlag::Transfer:
				{
					transferQueue.Assign(dref(this), queueFamilyInfos[index].QueueFamilyIndex, 0, EQueueFlag::Transfer);

					break;
				}
				default:
				{
					graphicsQueue.Assign(dref(this), queueFamilyInfos[index].QueueFamilyIndex, 0, EQueueFlag::Graphics);

					break;
				}
			}
		}

		info.QueueCreateInfoCount = SCast<u32>(queueFamilyInfos.size());
		info.QueueCreateInfos     = queueFamilyInfos.data()               ;
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

		info.EnabledExtensionCount = SCast<u32>(extensionsEnabled.size());
		info.EnabledExtensionNames = extensionsEnabled.data()               ;
	}

	void LogicalDevice::ProcessLayerSupport()
	{
		if (Meta::Vulkan::EnableLayers())
		{
			info.EnabledLayerCount = SCast<u32>(DesiredLayers.size());
			info.EnabledLayerNames = DesiredLayers.data();
		}
		else
		{
			info.EnabledLayerCount = 0;
		}
	}

#pragma endregion LogicalDevice



#pragma region Public

	const AppInstance& GPU_Comms::GetAppInstance()
	{
		return AppGPU_Comms;
	}

	void GPU_Comms_Maker<Meta::EGPU_Engage::Single>::AcquirePhysicalDevices()
	{
		EResult result = AppGPU_Comms.GetAvailablePhysicalDevices(PhysicalGPUs);

		if (result != EResult::Success)
			Exception::Fatal::Throw("Failed to get the physical GPUs.");

		Log("Physical GPUs acquired:");

		StringStream gpuHandle;

		for (auto& gpu : PhysicalGPUs)
		{
			gpu.GetAvailableExtensions   ();
			gpu.GetAvailableQueueFamilies();

			gpuHandle.str(String());

			gpuHandle << gpu;

			Log(gpu.GetProperties().Name + String(" Handle: ") + gpuHandle.str());
		}
	}

	void GPU_Comms_Maker<Meta::EGPU_Engage::Single>::Cease()
	{
		if (Meta::Vulkan::Enable_LogError  ()) GPU_Messenger_Error  .Destroy();
		if (Meta::Vulkan::Enable_LogWarning()) GPU_Messenger_Warning.Destroy();
		if (Meta::Vulkan::Enable_LogInfo   ()) GPU_Messenger_Info   .Destroy();
		if (Meta::Vulkan::Enable_LogVerbose()) GPU_Messenger_Verbose.Destroy();

		DeviceEngaged = nullptr;

		for (auto& device : LogicalGPUs)
		{
			device.Destroy(); 
		}

		Log("Device disengaged and logical devices destroyed");

		AppGPU_Comms.Destroy(); 
		
		Log("GPU communications ceased");
	}

	void GPU_Comms_Maker<Meta::EGPU_Engage::Single>::Initialize(RoCStr _appName, Meta::AppVersion _version)
	{
		AppInstance::AppInfo    spec       {};
		AppInstance::CreateInfo createSpec {};

		AppInstance::GetAvailableLayersAndExtensions(AppLayersAndExtensions);

		if (Meta::Vulkan::EnableLayers())
		{
			Log("EnableLayers specified");

			Log("Available layers: ");

			for (auto& layerAndExtensions : AppLayersAndExtensions)
			{
				Log(layerAndExtensions.Layer.Name);
			}

			// In order to process receive layer messages, need the debugger.
			//DesiredInstanceExts.push_back(InstanceExt::DebugUtility);

			if (Meta::Vulkan::Enable_API_Dump())
			{
				DesiredLayers.push_back(Layer::LunarG_API_Dump);

				Log("Layer Selected: LunarG API Dump");
			}

			if (Meta::Vulkan::Enable_FPSMonitor())
			{
				DesiredLayers.push_back(Layer::LunarG_Monitor);

				Log("Layer Selected: LunarG Monitor (FPS on window");
			}

			if (Meta::Vulkan::Enable_Validation())
			{
				AquireSupportedValidationLayers();
			}
				
			if (!CheckLayerSupport(DesiredLayers))
			{
				Exception::Fatal::Throw("Layers requested, but are not available!");
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

		createSpec.AppInfo = getPtr(spec);

		DetermineRequiredExtensions();

		createSpec.EnabledExtensionCount = SCast<u32>(DesiredInstanceExts.size());
		createSpec.EnabledExtensionNames = DesiredInstanceExts.data()               ;

		DynamicArray<DebugUtils::Messenger::CreateInfo> debugInfos;

		if (Meta::Vulkan::EnableLayers())
		{
			createSpec.EnabledLayerCount = SCast<u32>(DesiredLayers.size());
			createSpec.EnabledLayerNames = DesiredLayers.data();

			SetupDebugMessenger();

			if (Meta::Vulkan::Enable_LogError  ()) debugInfos.push_back(GPU_Messenger_Error  .GetInfo());
			if (Meta::Vulkan::Enable_LogWarning()) debugInfos.push_back(GPU_Messenger_Warning.GetInfo());
			if (Meta::Vulkan::Enable_LogInfo   ()) debugInfos.push_back(GPU_Messenger_Info   .GetInfo());
			if (Meta::Vulkan::Enable_LogVerbose()) debugInfos.push_back(GPU_Messenger_Verbose.GetInfo());

			createSpec.Next = debugInfos.data();
		}
		else
		{
			createSpec.EnabledLayerCount = 0;
			createSpec.Next              = nullptr;
		}

		EResult creationResult = AppGPU_Comms.Create(createSpec);  

		if (creationResult != EResult::Success) 
			Exception::Fatal::Throw("Failed to create Vulkan app instance.");

		Log("Application handshake complete.");

		if (Meta::Vulkan::EnableLayers())
		{
			if (Meta::Vulkan::Enable_LogError())
			{
				creationResult = GPU_Messenger_Error.Create(AppGPU_Comms);

				if (creationResult != EResult::Success)
					Exception::Fatal::Throw("Failed to setup error debug messenger.");
			}

			if (Meta::Vulkan::Enable_LogWarning())
			{
				creationResult = GPU_Messenger_Warning.Create(AppGPU_Comms);

				if (creationResult != EResult::Success)
					Exception::Fatal::Throw("Failed to setup warning debug messenger.");
			}
			
			if (Meta::Vulkan::Enable_LogInfo())
			{
				creationResult = GPU_Messenger_Info.Create(AppGPU_Comms);

				if (creationResult != EResult::Success)
					Exception::Fatal::Throw("Failed to setup info debug messenger.");
			}

			if (Meta::Vulkan::Enable_LogVerbose())
			{
				creationResult = GPU_Messenger_Verbose.Create(AppGPU_Comms);

				if (creationResult != EResult::Success)
					Exception::Fatal::Throw("Failed to setup verbose debug messenger.");
			}

			Log("Debug messenger created");
		}
	}

	void GPU_Comms_Maker<Meta::EGPU_Engage::Single>::EngageMostSuitableDevice()
	{
		Log("Determining most suitable device to engage.");

		Log("Setting up a dummy test window and surface to evaluate devices...");

		ptr<OSAL::Window> testWindow;

		OSAL::WindowInfo windowSpec = {};

		windowSpec.WindowTitle = "GPU_Vulkan: SurfaceSupportTest";
		windowSpec.WindowSize  = { 10, 10 }                      ;
		windowSpec.Windowed    = OSAL::WindowInfo ::WindowedMode ;
		windowSpec.Resizable   = OSAL::WinBool::True             ;

		testWindow = OSAL::Create_Window(windowSpec);

		Surface testSurface;

		if (testSurface.Create(AppGPU_Comms, OSAL::GetOS_WindowHandle(testWindow)) != EResult::Success)
		{
			Exception::Fatal::Throw("Failed to create window surface!");
		}

		for (auto& device : LogicalGPUs)
		{
			bool HasExtensions = device.ExtensionsEnabled(DesiredDeviceExts);

			Bool PresentationSupport; 

			testSurface.AssignPhysicalDevice(device.GetPhysicalDevice());

			// Currently the graphics queue created must be the same one able to present.
			// This will most likely be changed later...
			testSurface.CheckPhysicalDeviceSupport
			(
				device.GetQueue(EQueueFlag::Graphics).GetFamilyIndex(),
				PresentationSupport
			);

			if (HasExtensions && PresentationSupport) 
			{
				DeviceEngaged = getPtr(device);

				Log("Device engaged: " + DeviceEngaged->GetSig());
			}
		}

		if (DeviceEngaged == nullptr)
		{
			Exception::Fatal::Throw("Failed to engage a suitable logical device.");
		}

		testSurface.Destroy();

		OSAL::Destroy_Window(testWindow);
	}

	void GPU_Comms_Maker<Meta::EGPU_Engage::Single>::GenerateLogicalDevices()
	{
		LogicalGPUs.resize(PhysicalGPUs.size());

		u32 gpuIndex = 0;

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
				Exception::Fatal::Throw("Failed to create logical device!");
			}
		}

		Log("Logical devices generated");
	}

	const LogicalDevice& GPU_Comms_Maker<Meta::EGPU_Engage::Single>::GetEngagedDevice()
	{
		return dref(DeviceEngaged);
	}

	const PhysicalDevice& GPU_Comms_Maker<Meta::EGPU_Engage::Single>::GetEngagedPhysicalGPU()
	{
		return DeviceEngaged->GetPhysicalDevice();
	}

	const LogicalDevice::Queue& GPU_Comms_Maker<Meta::EGPU_Engage::Single>::GetGraphicsQueue()
	{
		return DeviceEngaged->GetGraphicsQueue();
	}

	const LogicalDevice::Queue& GPU_Comms_Maker<Meta::EGPU_Engage::Single>::GetComputeQueue()
	{
		return DeviceEngaged->GetComputeQueue();
	}

	const LogicalDevice::Queue& GPU_Comms_Maker<Meta::EGPU_Engage::Single>::GetTransferQueue()
	{
		return DeviceEngaged->GetTransferQueue();
	}

#pragma endregion Public

#pragma region Protected

	void GPU_Comms_Maker<Meta::EGPU_Engage::Single>::AquireSupportedValidationLayers()
	{
		bool found = false;

		// Ideal

		for (const auto& layerAndExtenions : AppLayersAndExtensions)
		{
			if (CStrCompare(Layer::Khronos_Validation, layerAndExtenions.Layer.Name) == 0)
			{
				DesiredLayers.push_back(Layer::Khronos_Validation);

				Log("Validation Layer Enabled: Khronos");

				found = true;

				break;
			}
		}

		// Fallback 1

		if (!found)
		{
			for (const auto& layerAndExtenions : AppLayersAndExtensions)
			{
				if (CStrCompare(Layer::LunarG_StandardValidation, layerAndExtenions.Layer.Name) == 0)
				{
					DesiredLayers.push_back(Layer::LunarG_StandardValidation);

					Log("Validation Layer Enabled: LunarG Standard");

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

			uDM layersFound = 0;

			for (auto validationLayerName : Fallback2Layers)
			{
				for (const auto& layerAndExtenions : AppLayersAndExtensions)
				{
					if (CStrCompare(validationLayerName, layerAndExtenions.Layer.Name) == 0)
					{
						layersFound++;

						break;
					}
				}
			}

			if (layersFound == Fallback2Layers.size()) 
			{
				for (auto validationLayerName : Fallback2Layers)
				{
					DesiredLayers.push_back(validationLayerName);

					Log("Validation Layer Enabled: " + String(validationLayerName));
				}

				found = true;
			}
		}

		// Fallback 3

		if (!found)
		{
			for (const auto& layerAndExtenions : AppLayersAndExtensions)
			{
				if (CStrCompare(Layer::LunarG_CoreValidation, layerAndExtenions.Layer.Name) == 0)
				{
					DesiredLayers.push_back(Layer::LunarG_CoreValidation);

					Log("Validation Layer Enabled: LunarG Core");

					found = true;

					break;
				}
			}
		}
	}

	bool GPU_Comms_Maker<Meta::EGPU_Engage::Single>::CheckLayerSupport(DynamicArray<RoCStr> _layersSpecified)
	{
		uDM layersFound = 0;

		for (auto validationLayerName : _layersSpecified)
		{
			for (const auto& layerAndExtenions : AppLayersAndExtensions)
			{
				if (CStrCompare(validationLayerName, layerAndExtenions.Layer.Name) == 0)
				{
					layersFound++;

					break;
				}
			}
		}

		if (!(layersFound == _layersSpecified.size()))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	using Messenger = DebugUtils::Messenger;

	Bool DebugCallback_Verbose
	(
		Messenger::ServerityFlags /*_messageServerity*/,
		Messenger::TypeFlags      /*_messageType*/,
		const Messenger::CallbackData   _callbackData,
		void*                     /*_userData*/
	)
	{
		String formattedMessage("Verbose: \n");

		formattedMessage.append(_callbackData.MesssageIDName);

		formattedMessage.append(": " + String(_callbackData.Message));

		Log(formattedMessage);

		return EBool::True;
	}

	Bool DebugCallback_Info
	(
		Messenger::ServerityFlags /*_messageServerity*/,
		Messenger::TypeFlags      /*_messageType*/,
		const Messenger::CallbackData   _callbackData,
		void*                     /*_userData*/
	)
	{
		String formattedMessage("Info: \n");

		formattedMessage.append(_callbackData.MesssageIDName);

		formattedMessage.append(": " + String(_callbackData.Message));

		Log(formattedMessage);

		return EBool::True;
	}

	Bool DebugCallback_Warning
	(
		Messenger::ServerityFlags /*_messageServerity*/,
		Messenger::TypeFlags      /*_messageType*/,
		const Messenger::CallbackData   _callbackData,
		void*                     /*_userData*/
	)
	{
		String formattedMessage(_callbackData.MesssageIDName);
		
		formattedMessage.append(": " + String(_callbackData.Message));

		Log_Error(formattedMessage);

		return EBool::True;
	}

	Bool DebugCallback_Error
	(
		      Messenger::ServerityFlags /*_messageServerity*/,
		      Messenger::TypeFlags      /*_messageType*/,
		const Messenger::CallbackData   _callbackData    , 
		      void*                     /*_userData*/
	)
	{
		String formattedMessage(_callbackData.MesssageIDName);

		formattedMessage.append(": " + String(_callbackData.Message));

		Log_Error(formattedMessage);

		return EBool::True;
	}

	void GPU_Comms_Maker<Meta::EGPU_Engage::Single>::DetermineRequiredExtensions()
	{
		switch (OSAL::WindowingPlatform)
		{
			case Meta::EWindowingPlatform::GLFW:
			{
				u32 numExtensions = 0;

				CStrArray extensions = CStrArray(SAL::GLFW::GetRequiredVulkanAppExtensions(numExtensions));

				for (u32 index = 0; index < numExtensions; index++)
				{
					if (std::find(DesiredInstanceExts.begin(), DesiredInstanceExts.end(), extensions[index]) == DesiredInstanceExts.end())
					{
						DesiredInstanceExts.push_back(extensions[index]);

						Log("Added GLFW desired instance extension: " + String(extensions[index]));
					}
				}

				break;
			}
			case Meta::EWindowingPlatform::OSAL:
			{
				// A GPU must be available with ability to render to a surface.
				DesiredInstanceExts.push_back(InstanceExt::Surface);

				Log("Added desired instance extension: " + String(InstanceExt::Surface));

				// Surface OS platform extension.
				DesiredInstanceExts.push_back(Surface::OS_Extension);

				Log("Added desired instance extension: " + String(Surface::OS_Extension));

				break;
			}
		}

		// Engine needs swap chains.
		DesiredDeviceExts.push_back(DeviceExt::Swapchain);

		Log("Added desired device extension: " + String(DeviceExt::Swapchain));

		if (Meta::Vulkan::EnableLayers())
		{
			DesiredInstanceExts.push_back(InstanceExt::DebugUtility);

			Log("Added desired instance extension: " + String(InstanceExt::DebugUtility));
		}
	}

	void GPU_Comms_Maker<Meta::EGPU_Engage::Single>::SetupDebugMessenger()
	{
		Messenger::CreateInfo info{};

		using EMaskS = decltype(info.Serverity)::Enum;
		using EMaskT = decltype(info.Type)::Enum;

		info.Type.Set(EMaskT::General, EMaskT::Validation, EMaskT::Performance);

		info.UserData = nullptr;

		if (Meta::Vulkan::Enable_LogError())
		{
			info.Serverity.Set(EMaskS::Error);

			info.UserCallback = GetVTAPI_Call(DebugCallback_Error);

			GPU_Messenger_Error.AssignInfo(info);
		}
		
		if (Meta::Vulkan::Enable_LogWarning())
		{
			info.Serverity.Set(EMaskS::Warning);

			info.UserCallback = GetVTAPI_Call(DebugCallback_Warning);

			GPU_Messenger_Warning.AssignInfo(info);
		}

		if (Meta::Vulkan::Enable_LogInfo())
		{
			info.Serverity.Set(EMaskS::Info);

			info.UserCallback = GetVTAPI_Call(DebugCallback_Info);

			GPU_Messenger_Info.AssignInfo(info);
		}

		if (Meta::Vulkan::Enable_LogVerbose())
		{
			info.Serverity.Set(EMaskS::Verbose);

			info.UserCallback = GetVTAPI_Call(DebugCallback_Verbose);

			GPU_Messenger_Verbose.AssignInfo(info);
		}
	}
}

#pragma endregion Protected

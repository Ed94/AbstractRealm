#pragma once



#include "Vulkan_API.hpp"


#include "LAL.hpp"
#include "Meta/AppInfo.hpp"



namespace HAL::GPU::Vulkan
{
	using namespace VT;
	using namespace VT::Corridors;
	using namespace VT::V4;

	using namespace LAL;



	struct QueueFamilyIndices
	{
		std::optional<uint32> Graphics;
		std::optional<uint32> Compute;
		std::optional<uint32> Transfer;
		std::optional<uint32> SparseBinding;

		std::optional<uint32> PresentationSupported;

		std::set<uint32> GetSet()
		{
			std::set<uint32> queueIndices =
			{
				Graphics.value(),
				Compute.value(),
				Transfer     .value(),
				SparseBinding.value(),
				PresentationSupported.value()
			};

			return queueIndices;
		}
	};

	using LayerandExtensionsList = DynamicArray<LayerAndExtensionProperties>;

	class PhysicalDevice : public V4::PhysicalDevice
	{
	public:

		using Parent = V4::PhysicalDevice;

		EResult GetAvailableExtensions();

		void GetAvailableQueueFamilies();

		const LayerandExtensionsList& GetLayersAndExtensions() const
		{ return layersAndExtensions; }

		operator V4::PhysicalDevice& ()
		{
			return *static_cast<V4::PhysicalDevice*>(this);
		}

		operator const Parent&() const
		{
			return *static_cast<const Parent*>(this);	
		}

	protected:

		DynamicArray<QueueFamilyProperties> queueFamilies;

		LayerandExtensionsList layersAndExtensions;
	};

	using PhysicalDeviceList = DynamicArray<PhysicalDevice>;

	class AppInstance : public V4::AppInstance
	{
	public:

		using Parent = V4::AppInstance;

		/** 
		@brief Provides the handles of all available physical devices.

		@todo Make the device listing container type specifiable using an interface.
		*/
		EResult GetAvailablePhysicalDevices(DynamicArray<PhysicalDevice>& _deviceListing) const;
	};

	class LogicalDevice : public V4::LogicalDevice
	{
	public:

		using Parent = V4::LogicalDevice;

		void AssignPhysicalDevice(PhysicalDevice& _physicalDevice);

		EResult Create();

		bool ExtensionsEnabled(DynamicArray<RoCStr> _extensions);
		
		const PhysicalDevice& GetPhysicalDevice() const;

		const Queue& GetQueue(EQueueFlag _type) const;

		const Queue& GetGraphicsQueue() const;

		operator Handle()
		{
			return handle;
		}

		operator Handle() const
		{
			return handle;
		}

		operator const Handle& () const
		{
			return handle;
		}

		operator V4::LogicalDevice& ()
		{
			return *reinterpret_cast<V4::LogicalDevice*>(this);
		}

		operator const V4::LogicalDevice&() const
		{
			return *reinterpret_cast<const V4::LogicalDevice*>(this);

		}		

	protected:

		void PrepareQueues();

		void ProcessExtensionSupport();

		void ProcessLayerSupport();

		// An object used to organize
		struct QueueFamily
		{
			//DynamicArray<Queue> queues;   // Not used at this level for now.

			Queue::CreateInfo* Info = nullptr;   // Stores a reference from the queue info container.

			float32 Priority = 1.0F;   // Later on will need to be a dynamic array of each queue's priority.

			EQueueFlag Assignment;
		};

		// Used by the logical device creation parameter to allocate the queues.
		DynamicArray<Queue::CreateInfo> queueFamilyInfos;

		DynamicArray<QueueFamily> queueFamilies;   // Queue families and thier queues* (later addition).

		// Currently the engine only assigns itself one queue from each family.
		// For now graphics is only used.
		Queue graphicsQueue;
		Queue computeQueue ;
		Queue transferQueue;

		DynamicArray<RoCStr> extensionsEnabled;
	};

	using LogicalDeviceList = DynamicArray<LogicalDevice>;



	BSS
	(
		eGlobal AppInstance            AppGPU_Comms          ;
		eGlobal LayerandExtensionsList AppLayersAndExtensions;
		eGlobal DynamicArray<RoCStr>   DesiredLayers         ;
		eGlobal DynamicArray<RoCStr>   DesiredInstanceExts   ;  
		eGlobal DynamicArray<RoCStr>   DesiredDeviceExts     ;

		eGlobal V4::DebugMessenger DebugMessenger;

		eGlobal PhysicalDeviceList PhysicalGPUs;
		eGlobal LogicalDeviceList  LogicalGPUs ;
	)
	


	void AcquirePhysicalDevices();

	void AppGPU_Comms_Initialize(RoCStr _appName, Meta::AppVersion _version);

	/*
	Checks to see if the desired layers specified are supported.
	*/
	bool CheckLayerSupport(DynamicArray<RoCStr> _layersSpecified);

	void DetermineRequiredExtensions();

	void EngageMostSuitableDevice();

	void GenerateLogicalDevices();

	const LogicalDevice& GetEngagedDevice();
	const PhysicalDevice& GetEngagedPhysicalGPU();

	void SetupDebugMessenger();
}

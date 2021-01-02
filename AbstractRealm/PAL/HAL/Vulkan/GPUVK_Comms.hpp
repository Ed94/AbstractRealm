#pragma once



#include "Vulkan_API.hpp"

#include "Core/Memory/MemTracking.hpp"
#include "LAL.hpp"
#include "Meta/AppInfo.hpp"



namespace HAL::GPU::Vulkan
{
	//Usings

	using namespace Core::Memory;

	using namespace VT;
	using namespace VT::Corridors;
	using namespace VT::V3;

	using namespace LAL;

	using LAL::DynamicArray;
	using LAL::Deque;

	using LayerandExtensionsList = DynamicArray<LayerAndExtensionProperties>;


	
	// Classes

	class PhysicalDevice : public V3::PhysicalDevice
	{
	public:

		using Parent = V3::PhysicalDevice;

		EResult GetAvailableExtensions();

		void GetAvailableQueueFamilies();

		const LayerandExtensionsList& GetLayersAndExtensions() const;

		operator Parent&();

		operator const Parent& () const;

	protected:

		DynamicArray<QueueFamilyProperties> queueFamilies;

		LayerandExtensionsList layersAndExtensions;
	};

	class AppInstance : public V3::AppInstance
	{
	public:

		using Parent = V3::AppInstance;

		/** 
		@brief Provides the handles of all available physical devices.

		@todo Make the device listing container type specifiable using an interface.
		*/
		EResult GetAvailablePhysicalDevices(DynamicArray<PhysicalDevice>& _deviceListing) const;

	protected:

		AppInfo    appInfo   ;
		CreateInfo createInfo;


	};

	struct DebugUtils : public V3::DebugUtils
	{
		using Parent = V3::DebugUtils;

		class Messenger : public Parent::Messenger
		{
		public:
			using Parent = V3::DebugUtils::Messenger;

			void AssignInfo(const CreateInfo& _info);

			EResult Create(const AppInstance& _app);

			EResult Create(const AppInstance& _app, const Memory::AllocationCallbacks& _allocator);

			const CreateInfo& GetInfo() const;

		protected:

			CreateInfo info;
		};
	};

	class LogicalDevice : public V3::LogicalDevice
	{
	public:

		using Parent = V3::LogicalDevice;

		void AssignPhysicalDevice(PhysicalDevice& _physicalDevice);

		EResult Create();

		bool ExtensionsEnabled(DynamicArray<RoCStr> _extensions);
		
		const PhysicalDevice& GetPhysicalDevice() const;

		const Queue& GetQueue(EQueueFlag _type) const;

		const Queue& GetGraphicsQueue() const;
		const Queue& GetComputeQueue () const;
		const Queue& GetTransferQueue() const;

		const String GetSig() const;

		operator       Handle ();
		operator const Handle&() const;
		operator       Parent&();
		operator const Parent&() const;

	protected:

		void PrepareQueues          ();
		void ProcessExtensionSupport();
		void ProcessLayerSupport    ();

		// An object used to organize
		struct QueueFamily
		{
			//DynamicArray<Queue> queues;   // Not used at this level for now.

			Queue::CreateInfo* Info = nullptr;   // Stores a reference from the queue info container.

			f32 Priority = 1.0F;   // Later on will need to be a dynamic array of each queue's priority.

			EQueueFlag Assignment;
		};

		CreateInfo info;

		//DynamicArray<CommandPool> commandPools;   // CommandPools are assigned per device. (There as many as there are working cpu threads for the gpu).

		// Used by the logical device creation parameter to allocate the queues.
		DynamicArray<Queue::CreateInfo> queueFamilyInfos;

		DynamicArray<QueueFamily> queueFamilies;   // Queue families and thier queues* (later addition).

		// Currently the device only assigns itself one queue from each family.
		// For now graphics is only used.
		Queue graphicsQueue;
		Queue computeQueue ;
		Queue transferQueue;

		DynamicArray<RoCStr> extensionsEnabled;
	};


	
	using PhysicalDeviceList = DynamicArray<PhysicalDevice>;
	using LogicalDeviceList = DynamicArray<LogicalDevice>;

	

	const AppInstance& GetAppInstance();

	void AcquirePhysicalDevices();

	void AppGPU_Comms_Cease();

	void AppGPU_Comms_Initialize(RoCStr _appName, Meta::AppVersion _version);

	void EngageMostSuitableDevice();

	void GenerateLogicalDevices();

	const LogicalDevice& GetEngagedDevice();

	const PhysicalDevice& GetEngagedPhysicalGPU();

	// Engaged queues. (Since only one device is engaged in design, these are easy references to its queues).

	const LogicalDevice::Queue& GetGraphicsQueue();   // Provides a reference to the engaged device's graphics queue.
	const LogicalDevice::Queue& GetComputeQueue ();   // Provides a reference to the engaged device's compute queue.
	const LogicalDevice::Queue& GetTransferQueue();   // Provides a reference to the engaged device's transfer queue.
}

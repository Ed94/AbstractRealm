// Not used yet.


#pragma once




#include "Vulkan_API.hpp"




namespace HAL::GPU::Vulkan
{
	//Usings

	using namespace VT;
	using namespace VT::Corridors;
	using namespace VT::V3;

	using namespace LAL;

	using LAL::DynamicArray;
	using LAL::Deque;

	using LayerandExtensionsList = DynamicArray<LayerAndExtensionProperties>;



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
}

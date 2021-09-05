// Parent Header
#include "GPUVK_PhysicalDevice.hpp"


//namespace HAL::GPU::Vulkan
//{
//	EResult PhysicalDevice::GetAvailableExtensions()
//	{
//		layersAndExtensions = AppLayersAndExtensions;
//
//		EResult result = EResult::Incomplete;
//
//		for (auto& layerAndExtensions : layersAndExtensions)
//		{
//			result = Parent::GetAvailableExtensions(layerAndExtensions.Layer.Name, layerAndExtensions.Extensions);
//
//			if (result != EResult::Success) return result;
//		}
//
//		return result;
//	}
//
//	void PhysicalDevice::GetAvailableQueueFamilies()
//	{
//		queueFamilies = Parent::GetAvailableQueueFamilies();
//	}
//
//	const LayerandExtensionsList& PhysicalDevice::GetLayersAndExtensions() const
//	{
//		return layersAndExtensions;
//	}
//
//	PhysicalDevice::operator Parent& ()
//	{
//		return dref(SCast<Parent>(this));
//	}
//
//	PhysicalDevice::operator const Parent& () const
//	{
//		return dref(SCast<const Parent>(this));
//	}
//}

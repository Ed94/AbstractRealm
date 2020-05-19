#pragma once



// Parent Header
#include "Vulkan_HAL.hpp"



#include "LAL/LAL.hpp"



namespace HAL
{
	namespace Vulkan
	{
		ApplicationInstance::AppInfo::operator VkApplicationInfo() const
		{
			stack<VkApplicationInfo> converted;

			converted.sType              = VkStructureType(SType);
			converted.pNext              = Extension             ;
			converted.pApplicationName   = AppName               ;
			converted.applicationVersion = AppVersion            ;
			converted.pEngineName        = EngineName            ;
			converted.apiVersion         = uInt32(API_Version)   ;

			return converted;
		}

		ApplicationInstance::CreateInfo::operator VkInstanceCreateInfo() const
		{
			stack<VkInstanceCreateInfo> converted;

			converted.sType = VkStructureType(SType);
			converted.pNext = Extension             ;
			converted.flags = Flags                 ;

			// TODO: Confirm correct usage of R-Value reference.
			VkApplicationInfo&& tempRRef = VkApplicationInfo(dref(AppInfo));

			converted.pApplicationInfo = getAddress(tempRRef);   
			// I had to pass app info via ref otherwise template would not work.. 
			// TODO: Find out why

			converted.enabledLayerCount       = EnabledLayerCount    ;
			converted.ppEnabledLayerNames     = EnabledLayerNames    ;
			converted.enabledExtensionCount   = EnabledExtensionCount;
			converted.ppEnabledExtensionNames = EnabledExtensionNames;

			return converted;
		}

		EResult CreateApplicationInstance
		(
			    const ApplicationInstance::CreateInfo& AppSpec        , 
			ptr<const AllocationCallbacks            > CustomAllocator, 
			ptr<      ApplicationInstance::Handle    > HandleContainer
		)
		{
			const VkInstanceCreateInfo&& tempRRef = VkInstanceCreateInfo(AppSpec);

			return EResult(vkCreateInstance(getAddress(tempRRef), CustomAllocator, HandleContainer));
			// I had to pass app info via ref otherwise template would not work.. 
			// TODO: Find out why
		}

		uInt32 MakeVersion(uInt32 _major, uInt32 _minor, uInt32 _patch)
		{
			return VK_MAKE_VERSION(_major, _minor, _patch);
		}
	}
}

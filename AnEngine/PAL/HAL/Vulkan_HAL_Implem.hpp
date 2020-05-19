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

			converted.sType              = VkStructureType(SType) ;
			converted.pNext              = Extension              ;
			converted.pApplicationName   = AppName                ;
			converted.applicationVersion = AppVersion             ;
			converted.pEngineName        = EngineName             ;
			converted.apiVersion         = (uint32_t)(API_Version);

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

		LayerProperties::operator VkLayerProperties() const
		{
			VkLayerProperties converted;

			strcpy_s(converted.layerName, LayerName);

			converted.specVersion           = SpecVersion          ;
			converted.implementationVersion = ImplementationVersion;

			strcpy_s(converted.description, Descrption);

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

		EResult GenerateGlobalLayerProperties(ptr<uint32> _numContainer, ptr<LayerProperties> _propertiesContainer)
		{
			return EResult(vkEnumerateInstanceLayerProperties(_numContainer, (ptr<VkLayerProperties>)(_propertiesContainer)));
		}

		uint32 GetNumberOfAvailableGlobalLayerProperties()
		{
			stack<uint32> layerCount;

			vkEnumerateInstanceLayerProperties( getAddress(layerCount), nullptr);

			return layerCount;
		}

		EResult GetAvailableGlobalLayerProperties(ptr<LayerProperties> _container)
		{
			stack<uint32> layerCount = GetNumberOfAvailableGlobalLayerProperties();

			return EResult(vkEnumerateInstanceLayerProperties(getAddress(layerCount), (ptr<VkLayerProperties>)(_container)));
		}

		uInt32 MakeVersion(uInt32 _major, uInt32 _minor, uInt32 _patch)
		{
			return VK_MAKE_VERSION(_major, _minor, _patch);
		}
	}
}

#pragma once



// Parent Header
#include "Vulkan_HAL.hpp"



#include "LAL/LAL.hpp"



namespace HAL
{
	namespace Vulkan
	{
		AppInstance::AppInfo::operator VkApplicationInfo() const
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

		AppInstance::CreateInfo::operator VkInstanceCreateInfo() const
		{
			// The struct data layout is exactly the same, so there should be no issue doing an raw cast...
			return *(VkInstanceCreateInfo*)(this);
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

		EResult CreateAppInstance
		(
			    const AppInstance::CreateInfo& _appSpec        , 
			ptr<const AllocationCallbacks    > _customAllocator, 
			          AppInstance::Handle&     _handleContainer
		)
		{
			return EResult(vkCreateInstance(getAddress((const VkInstanceCreateInfo)(_appSpec)), _customAllocator, getAddress(_handleContainer)));
		}

		EResult GenerateGlobalLayerProperties(ptr<uint32> _numContainer, ptr<LayerProperties> _propertiesContainer)
		{
			return EResult(vkEnumerateInstanceLayerProperties(_numContainer, (ptr<VkLayerProperties>)(_propertiesContainer)));
		}

		template<typename ReturnType> Where<IsFunctionPtr<ReturnType>(),
		ReturnType> GetProcedureAddress(AppInstance::Handle& _appInstance, ptr<const char> _procedureName)
		{
			return ReturnType(vkGetInstanceProcAddr(_appInstance, _procedureName));
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

		namespace Extensions
		{
			VkResult CreateDebugUtilsMessengerEXT
			(
				          VkInstance                          _appInstance   ,
				ptr<const VkDebugUtilsMessengerCreateInfoEXT> _createSpec    ,
				ptr<const VkAllocationCallbacks             > _allocator     ,
				ptr<      VkDebugUtilsMessengerEXT          > _debugMessenger
			)
			{
				PFN_vkCreateDebugUtilsMessengerEXT delegate = PFN_vkCreateDebugUtilsMessengerEXT(vkGetInstanceProcAddr(_appInstance, "vkCreateDebugUtilsMessengerEXT"));

				if (delegate != nullptr)
				{
					return delegate(_appInstance, _createSpec, _allocator, _debugMessenger);
				}
				else
				{
					return VK_ERROR_EXTENSION_NOT_PRESENT;
				}
			}
		}

		namespace Debug
		{
			Label::operator VkDebugUtilsLabelEXT()
			{
				VkDebugUtilsLabelEXT converted;

				converted.sType      = SType    ;
				converted.pNext      = Extension;
				converted.pLabelName = Name     ;

				converted.color[0] = Color[0];
				converted.color[1] = Color[1];
				converted.color[2] = Color[2];
				converted.color[3] = Color[3];

				return converted;
			}

			ObjectInfo::operator VkDebugUtilsObjectNameInfoEXT()
			{
				VkDebugUtilsObjectNameInfoEXT converted;

				converted.sType        = SType;
				converted.pNext        = Extension;
				converted.objectType   = VkObjectType(Type);
				converted.objectHandle = Handle;
				converted.pObjectName  = Name;

				return converted;
			}

			Messenger::CallbackData::operator VkDebugUtilsMessengerCallbackDataEXT()
			{
				return dref(ptr< VkDebugUtilsMessengerCallbackDataEXT>(this));
			}

			Messenger::CreateInfo::operator VkDebugUtilsMessengerCreateInfoEXT()
			{
				// The struct data layout is exactly the same, so there should be no issue doing an raw cast...
				return dref(ptr<VkDebugUtilsMessengerCreateInfoEXT>(this));
			}


			using FPtr_CreateMessenger = PFN_vkCreateDebugUtilsMessengerEXT;

			EResult CreateMessenger
			(
				AppInstance::Handle            _appInstance,
				const Messenger::CreateInfo&   _createSpec ,
				ptr<const AllocationCallbacks> _allocator  ,
				Messenger::Handle&             _messenger
			)
			{
				FPtr_CreateMessenger delegate = GetProcedureAddress<FPtr_CreateMessenger>(_appInstance, "vkCreateDebugUtilsMessengerEXT");

				if (delegate != nullptr)
				{
					return EResult
					(
						delegate
						(
							_appInstance                                                          , 
							ptr<const VkDebugUtilsMessengerCreateInfoEXT>(getAddress(_createSpec)), 
							ptr<const VkAllocationCallbacks             >(           _allocator  ), 
							ptr<      VkDebugUtilsMessengerEXT          >(getAddress(_messenger ))
						)
					);
				}
				else
				{
					return EResult::Error_ExtensionNotPresent;
				}
			}

			void DestroyMessenger
			(
				AppInstance::Handle            _appInstance,
				Messenger::Handle              _messenger  ,
				ptr<const AllocationCallbacks> _allocator
			)
			{
				using FPtr_DestroyMessenger = PFN_vkDestroyDebugUtilsMessengerEXT;

				FPtr_DestroyMessenger delegate = GetProcedureAddress<FPtr_DestroyMessenger>(_appInstance, "vkDestroyDebugUtilsMessengerEXT");

				if (delegate != nullptr)
				{
					delegate(_appInstance, _messenger, _allocator);
				}
			}
		}
	}
}

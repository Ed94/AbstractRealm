/*
Vulkan Hardware Abstraction Layer

Last Modified: 5/19/2020


*/



#pragma once



#include "LAL/LAL.hpp"
#include "Vulkan_HAL_Enums.hpp"



//VKAPI_ATTR
//VKAPI_CALL

namespace HAL
{
	using namespace LAL;

	namespace Vulkan
	{
		using Bool = VkBool32;


		// Experimental
		// Reference: https://stackoverflow.com/questions/40326733/changing-calling-convention

		template<typename FunctionType, ptr<FunctionType> >
		struct EnforcedSignatureCaller;

		template
		<
			typename ReturnType,
			typename... ParameterTypes,
			ReturnType(*FunctionType)(ParameterTypes...)
			//FPtr<ReturnType, FunctionType, ParameterTypes...>
		>
		// Pointer to function type determined at compile time.
		struct EnforcedSignatureCaller<ReturnType(ParameterTypes...), FunctionType>
		{
			unbound VKAPI_ATTR ReturnType VKAPI_CALL Call(ParameterTypes... _parameters)
			{
				return FunctionType(std::forward<ParameterTypes>(_parameters)...);
			}
		};

		template<typename FunctionType, FunctionType* _callback>
		auto EnforceSignature()
		{
			return getAddress(EnforcedSignatureCaller<FunctionType, _callback>::Call);
		}


		// Constants

		constexpr 
		ptr<const char> ValidationLayer_Khronos = "VK_LAYER_KHRONOS_validation";

		constexpr 
		ptr<const char> Extension_DebugUtility = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;


		// Types

		// Will most likely make a class wrap, doing this for now...
		using AllocationCallbacks = VkAllocationCallbacks;

		// TODO: Strict definition of an application in reguards to this api.

		/*
		A handle to an object that manages the application state.

		Other Name: Application State Container

		Vulkan has no global state reference: 
		Every application must keep track of their state using an instance object.

		https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkInstance.html
		*/
		class ApplicationInstance
		{
		public:   // Types
			using Handle = VkInstance;

			using CreateFlags = VkInstanceCreateFlags;

			/*
			A structure that specifies to the vulkan driver information about an
			application that will run an instance.

			https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkApplicationInfo.html
			*/
			//using ApplicationInformation = VkApplicationInfo;
			class AppInfo
			{
			public:
				EStructureType  SType        ;
				ptr<const void> Extension    ;
				ptr<const char> AppName      ;
				uint32          AppVersion   ;
				ptr<const char> EngineName   ;
				uint32          EngineVersion;
				EAPI_Version    API_Version  ;

				operator VkApplicationInfo() const;
			};

			class CreateInfo
			{
			public:
				using CStrArray = ptr< const ptr<const char> >;

			public:
				      EStructureType     SType                ;
				      ptr<const void>    Extension            ;
				      CreateFlags        Flags                ;
				      ptr<const AppInfo> AppInfo              ;
				      uint32             EnabledLayerCount    ;
				      CStrArray          EnabledLayerNames    ;
				      uint32             EnabledExtensionCount;
				      CStrArray          EnabledExtensionNames;

				operator VkInstanceCreateInfo() const;
			};

		public:

			//ApplicationInstance();
			//ApplicationInstance(CreateInfo CreationSpecToUse, AppInfo AppInfoToUse);

			// EResult Create();

			//AppInfo    appSpec     ;
			//CreateInfo creationSpec;
			//Handle     ID          ;
		};

		constexpr int ExtensionName_MaxSize = VK_MAX_EXTENSION_NAME_SIZE;
		constexpr int Description_MaxSize   = VK_MAX_DESCRIPTION_SIZE   ;

		using ExtensionNameStr = char[ExtensionName_MaxSize];
		using DescrptionStr    = char[Description_MaxSize  ];

		class LayerProperties
		{
		public:
			ExtensionNameStr LayerName            ;
			uint32           SpecVersion          ;
			uint32           ImplementationVersion;
			DescrptionStr    Descrption           ;

			operator VkLayerProperties() const;
		};

		// Debug



		using DebugMessageServerityFlags = bitmask<EDebugUtilities_MessageSeverityFlags>;
		using DebugMessageType           = bitmask<EDebugUtilities_MessageTypeFlags    >;


		// Functions

		EResult CreateApplicationInstance
		(
			    const ApplicationInstance::CreateInfo& AppSpec        , 
			ptr<const AllocationCallbacks            > CustomAllocator, 
			ptr<      ApplicationInstance::Handle    > HandleContainer
		);

		/*
		Wrap of vkEnumerateInstanceLayerProperties.
		*/
		EResult GenerateGlobalLayerProperties(ptr<uint32> NumPropertiesContainer, ptr<LayerProperties> PropertiesListing);

		uint32 GetNumberOfAvailableGlobalLayerProperties();

		EResult GetAvailableGlobalLayerProperties(ptr<LayerProperties> ContainerForResult);

		uInt32 MakeVersion(uInt32 Major, uInt32 Minor, uInt32 Patch);
	}
}	



#include "Vulkan_HAL_Implem.hpp"

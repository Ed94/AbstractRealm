/*
Vulkan Hardware Abstraction Layer

Last Modified: 5/19/2020


*/



#pragma once



#include "LAL/LAL.hpp"
#include "Vulkan_HAL_Enums.hpp"



namespace HAL
{
	using namespace LAL;

	namespace Vulkan
	{
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
				uInt32          AppVersion   ;
				ptr<const char> EngineName   ;
				uInt32          EngineVersion;
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
				      uInt32             EnabledLayerCount    ;
				      CStrArray          EnabledLayerNames    ;
				      uInt32             EnabledExtensionCount;
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


		// Functions

		EResult CreateApplicationInstance
		(
			    const ApplicationInstance::CreateInfo& AppSpec        , 
			ptr<const AllocationCallbacks            > CustomAllocator, 
			ptr<      ApplicationInstance::Handle    > HandleContainer
		);

		uInt32 MakeVersion(uInt32 Major, uInt32 Minor, uInt32 Patch);
	}
}	



#include "Vulkan_HAL_Implem.hpp"

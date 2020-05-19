/*
Vulkan Hardware Abstraction Layer

Last Modified: 5/19/2020


*/



#pragma once



#include <vulkan/vulkan.h>
#include "LAL/LAL.hpp"



namespace HAL
{
	using namespace LAL;

	namespace Vulkan
	{
		// Enums

		enum class EStructureType
		{
			ApplicationInformation = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			CreateInfo = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		};

		enum class EAPI_Version
		{
			_1_0 = VK_API_VERSION_1_0,
		};


		// Structs


		// Types

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

			//AppInfo    appSpec     ;
			//CreateInfo creationSpec;
			//Handle     ID          ;
		};


		// Functions

		uInt32 MakeVersion(uInt32 Major, uInt32 Minor, uInt32 Patch);
	}
}	



#include "Vulkan_HAL.hppi"

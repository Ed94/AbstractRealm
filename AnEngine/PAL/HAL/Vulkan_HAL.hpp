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

		using Bool = VkBool32;

		using EStructureType = VkStructureType;

		using Flags = VkFlags;

		// Experimental
		// Reference: https://stackoverflow.com/questions/40326733/changing-calling-convention
		MakeEnforcementSet(Vulkan, VKAPI_ATTR, VKAPI_CALL);

		template<typename ReturnType, typename... ParameterTypes>                  
		using Enforced_FPtr = ReturnType(VKAPI_PTR*)(ParameterTypes...);

		// Equivalent to Enforced_FPtr<void, void>.
		using FPtr_Void = PFN_vkVoidFunction;


		// Constants

		constexpr 
		ptr<const char> ValidationLayer_Khronos = "VK_LAYER_KHRONOS_validation";

		
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
		class AppInstance
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
			struct AppInfo
			{
				EStructureType  SType        ;
				ptr<const void> Extension    ;
				ptr<const char> AppName      ;
				uint32          AppVersion   ;
				ptr<const char> EngineName   ;
				uint32          EngineVersion;
				EAPI_Version    API_Version  ;

				operator VkApplicationInfo() const;
			};

			struct CreateInfo
			{
				using CStrArray = ptr< const ptr<const char> >;

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

		struct LayerProperties
		{
			ExtensionNameStr LayerName            ;
			uint32           SpecVersion          ;
			uint32           ImplementationVersion;
			DescrptionStr    Descrption           ;

			operator VkLayerProperties() const;
		};
		
		// Functions

		EResult CreateAppInstance
		(
			    const AppInstance::CreateInfo& AppSpec        , 
			ptr<const AllocationCallbacks    > CustomAllocator, 
			          AppInstance::Handle&     HandleContainer
		);

		/*
		Wrap of vkEnumerateInstanceLayerProperties.
		*/
		EResult GenerateGlobalLayerProperties(ptr<uint32> NumPropertiesContainer, ptr<LayerProperties> PropertiesListing);

		template<typename ReturnType> Where<IsFunctionPtr<ReturnType>(),
		ReturnType> GetProcedureAddress(AppInstance::Handle& _appInstance, ptr<const char> _procedureName);

		uint32 GetNumberOfAvailableGlobalLayerProperties();

		EResult GetAvailableGlobalLayerProperties(ptr<LayerProperties> ContainerForResult);

		uInt32 MakeVersion(uInt32 Major, uInt32 Minor, uInt32 Patch);

		namespace Extensions
		{
			VkResult CreateDebugUtilsMessengerEXT
			(
				          VkInstance                          AppInstance   ,
				ptr<const VkDebugUtilsMessengerCreateInfoEXT> CreateSpec    ,
				ptr<const VkAllocationCallbacks             > Allocator     ,
				ptr<      VkDebugUtilsMessengerEXT          > DebugMessenger
			);
		}

		namespace Debug
		{
			constexpr 
			ptr<const char> Extension_DebugUtility = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

			struct Label
			{
				EStructureType  SType    ;
				ptr<const void> Extension;
				ptr<const char> Name     ;
				float32         Color[4] ;

				operator VkDebugUtilsLabelEXT();
			};

			struct ObjectInfo
			{
				EStructureType  SType    ;
				ptr<const void> Extension;
				EObjectType     Type     ;
				uInt64          Handle   ;
				ptr<const char> Name     ;

				operator VkDebugUtilsObjectNameInfoEXT();
			};

			using MessageServerityFlags = bitmask<EDebugUtilities_MessageSeverityFlags, Flags>;
			using MessageTypeFlags      = bitmask<EDebugUtilities_MessageTypeFlags    , Flags>;

			class Messenger
			{
			public:
				using Handle = VkDebugUtilsMessengerEXT;

				struct CallbackData
				{
					using CallbackDataFlags = bitmask<EUndefined, Flags>;

					EStructureType        SType               ;
					ptr<const void>       Extension           ;
					CallbackDataFlags     Flags               ;
					ptr<const char>       MesssageIDName      ;
					sint32                MessageIDNumber     ;
					ptr<const char>       Message             ;
					uint32                QueueLabelCount     ;
					ptr<const Label>      QueueLabels         ;
					uint32                CMDBufferLabel_Count;
					ptr<const Label>      CMDBufferLabels     ;
					uint32                ObjectCount         ;
					ptr<const ObjectInfo> Objects             ;

					operator VkDebugUtilsMessengerCallbackDataEXT();
				};

				using CallbackDelegate = Enforced_FPtr<Bool, 
					MessageServerityFlags, MessageTypeFlags, const CallbackData, ptr<void> >;

				struct CreateInfo
				{
					using MessengerCreateFlags = bitmask<EUndefined, Flags>;

					EStructureType        SType;
					ptr<const void>       Extension;
					MessengerCreateFlags  Flags;
					MessageServerityFlags Serverity;
					MessageTypeFlags      Type;
					CallbackDelegate      UserCallback;
					ptr<void>             UserData;

					operator VkDebugUtilsMessengerCreateInfoEXT();
				};
			}; 

			EResult CreateMessenger
			(
				AppInstance::Handle            _appInstance,
				const Messenger::CreateInfo&   _createSpec ,
				ptr<const AllocationCallbacks> _allocator  ,
				Messenger::Handle&             _messenger
			);

			void DestroyMessenger
			(
				AppInstance::Handle            _appInstance,
				Messenger::Handle              _messenger  ,
				ptr<const AllocationCallbacks> _allocator
			);
		}
	}
}	



#include "Vulkan_HAL_Implem.hpp"

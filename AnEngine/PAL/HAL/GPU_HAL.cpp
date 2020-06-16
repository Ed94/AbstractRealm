/*
This is a very rough implementation of a GPU_HAL. 

Its only intended to be used with the current Vulkan HAL used. Although I did mess around with designing for later.

Note: For now this is going to be a big copy and paste to an extent from the triangle test. 
(Only wrapped content though)
*/



// Parent Header
#include "GPU_HAL.hpp"



// Includes

#include "LAL/LAL.hpp"

#include "Meta/EngineInfo.hpp"
#include "Meta/Config/OSAL.hpp"
#include "OSAL/Platform.hpp"

#include "GLFW_SAL.hpp"



namespace HAL
{
	namespace GPU
	{
		using namespace LAL;
		using namespace Meta;

		using Meta::AppVersion;


		namespace Platform_Vulkan
		{
			// Usings

			using namespace Vulkan;

			using ExtensionIdentifierList = std::vector< RoCStr        >;
			using PhysicalDeviceList      = std::vector< PhysicalDevice>;
			using ValidationLayerList     = std::vector< RoCStr        >;


			// Static Data

			BSS
			(
				AppInstance::Handle AppStateRef          ;
				PhysicalDeviceList  PhysicalDevices      ;
				Messenger::Handle   DebugMessenger_Handle;
			);
						
			data< ValidationLayerList> ValidationLayerIdentifiers =
			{
				ValidationLayer_Khronos
			};


			// Forwards
			
			Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
			CStrArray> GetRequiredVulkanAppExtensions(uint32 _numExtensions);

			void PopulateDebugMessengerCreateInfo(Messenger::CreateInfo& _msngrCreateInfo);


			// Functions

			/*
			Checks to see if the validation layers specified are supported.
			*/
			bool CheckValidationLayerSupport(ValidationLayerList& _validationLayersSpecified)
			{
				using LayerPropertyList = std::vector<LayerProperties>;


				uInt32&& layerCount = GetNumOf_AvailableGlobalLayerProperties();

				stack<LayerPropertyList> availableLayers(layerCount);

				GetAvailableGlobalLayerProperties(availableLayers.data());


				stack<bool> layerFound = false;

				for (auto validationLayerName : _validationLayersSpecified)
				{
					for (const auto& layer : availableLayers)
					{
						if (strcmp(validationLayerName, layer.LayerName) == 0)
						{
							layerFound = true;

							break;
						}
					}
				}


				if (!layerFound)
				{
					return false;
				}

				return true;
			}

			ExtensionIdentifierList GetRequiredExtensions()
			{
				Stack
				(
					uint32    numExtensions      = 0; 
					CStrArray extensionsRequired    ;
				);

				extensionsRequired = GetRequiredVulkanAppExtensions(numExtensions);

				stack<ExtensionIdentifierList> extensions(extensionsRequired, extensionsRequired + numExtensions);

				if (Vulkan_ValidationLayersEnabled)
				{
					extensions.push_back(Vulkan::Extension_DebugUtility);
				}

				return extensions;
			}

			Bool DebugCallback
			(
					  MessageServerityFlags   _messageServerity, 
					  MessageTypeFlags        _messageType     ,
				const Messenger::CallbackData _callbackData    , 
					  ptr<void>               _userData
			)
			{
				std::cerr << "Vulkan: Validation Layer: " << _callbackData.Message << std::endl;

				using Vulkan::EBool;

				return EBool::True;
			}

			/*
			GLFW Version of the GetRequiredVulkanAppExtensions.
			*/
			Where<Meta::WindowingPlatform == Meta::EWindowingPlatform::GLFW,
			CStrArray> GetRequiredVulkanAppExtensions(uint32 _numExtensions)
			{
				return CStrArray(SAL::GLFW::GetRequiredVulkanAppExtensions(_numExtensions));
			}

			void InitializeVulkanApplication(RoCStr _appName, AppVersion _version)
			{
				using Meta::EEngineVersion;

				Stack
				(
					AppInstance::AppInfo    appSpec      {};
					AppInstance::CreateInfo appCreateSpec{};
				);

				if (Vulkan_ValidationLayersEnabled)
				{
					if (!CheckValidationLayerSupport(ValidationLayerIdentifiers))
						throw std::runtime_error("Validation layers requested, but are not available!");
				}

				//AppSpec.SType         = EStructureType::ApplicationInformation;   // Use later...
				appSpec.SType         = EStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO         ;
				appSpec.AppName       = _appName                                                   ;
				appSpec.AppVersion    = MakeVersion(_version.Major, _version.Minor, _version.Patch);
				appSpec.EngineName    = Meta::EngineName                                           ;
				appSpec.EngineVersion = MakeVersion
				(
					EEngineVersion::Major, 
					EEngineVersion::Minor, 
					EEngineVersion::Patch
				)                                         ;
				appSpec.API_Version   = EAPI_Version::_1_0;

				//AppCreateSpec.SType   = EStructureType::InstanceCreateInfo;
				appCreateSpec.SType   = EStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
				appCreateSpec.AppInfo = getAddress(appSpec)       ;

				ExtensionIdentifierList extensions = GetRequiredExtensions();

				appCreateSpec.EnabledExtensionCount = SCast<uint32>(extensions.size());
				appCreateSpec.EnabledExtensionNames = extensions.data();


				if (Vulkan_ValidationLayersEnabled)
				{
					appCreateSpec.EnabledLayerCount = SCast<uint32>(ValidationLayerIdentifiers.size());

					appCreateSpec.EnabledLayerNames = ValidationLayerIdentifiers.data();

					Messenger::CreateInfo debugMessenger_CreationSpec;

					PopulateDebugMessengerCreateInfo(debugMessenger_CreationSpec);

					appCreateSpec.Extension = ptr<Messenger::CreateInfo>(getAddress(debugMessenger_CreationSpec));
				}
				else
				{
					appCreateSpec.EnabledLayerCount = 0;

					appCreateSpec.Extension = nullptr;
				}

				EResult&& creationResult = AppInstance_Create(appCreateSpec, nullptr, AppStateRef);

				if (creationResult != EResult::Success) 
					throw std::runtime_error("Triangle Test: Failed to create Vulkan app instance.");
			}


			void PopulateDebugMessengerCreateInfo(Messenger::CreateInfo& _msngrCreateInfo)
			{
				_msngrCreateInfo.SType = EStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

				using EMaskS = decltype(_msngrCreateInfo.Serverity)::Enum;

				_msngrCreateInfo.Serverity.Set(EMaskS::Verbose, EMaskS::Warning, EMaskS::Error);

				using EMaskT = decltype(_msngrCreateInfo.Type)::Enum;

				_msngrCreateInfo.Type.Set(EMaskT::General, EMaskT::Validation, EMaskT::Performance);

				_msngrCreateInfo.UserCallback = EnforceConvention(Vulkan::EnforcerID_Vulkan, DebugCallback);

				_msngrCreateInfo.UserData = nullptr;
			}


			void SetupDebugMessenger()
			{
				if (!Vulkan_ValidationLayersEnabled) return;

				stack<Messenger::CreateInfo> msngrCreateSpec{};

				PopulateDebugMessengerCreateInfo(msngrCreateSpec);

				EResult&& creationResult = CreateMessenger(AppStateRef, msngrCreateSpec, nullptr, DebugMessenger_Handle);

				if (creationResult != EResult::Success) throw std::runtime_error("Failed to setup debug messenger!");
			}
		}


		namespace PlatformBackend
		{
			using Delegate_Initalize_GPUComms = Function<void(RoCStr, AppVersion)>;


			BSS
			(
				Delegate_Initalize_GPUComms Initalize_GPUComms_Bind;
			);


			void Determine_PlatformBindings()
			{
				switch (GPU_API)
				{
					case EGPUPlatformAPI::Vulkan:
					{
						Initalize_GPUComms_Bind = Platform_Vulkan::InitializeVulkanApplication;
					}
				}

			}
		}


		// Functionality

		void Load()
		{
			PlatformBackend::Determine_PlatformBindings();
		}

		void Unload()
		{

		}

		void Initalize_GPUComms(RoCStr _appName, AppVersion _version)
		{
			PlatformBackend::Initalize_GPUComms_Bind(_appName, _version);
		}
	}
}
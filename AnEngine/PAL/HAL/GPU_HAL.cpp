/*
This is a very rough implementation of a GPU_HAL. 

Its only intended to be used with the current Vulkan HAL used.
I did my best for the time making the MVP to make this designed well for expanding later on.

Note: For now this is going to be a big copy and paste to an extent from the triangle test. 
(Only wrapped content though)
*/


// Parent Header
#include "GPU_HAL.hpp"


#include "LAL/LAL.hpp"

#include "Core/Meta/EngineInfo.hpp"
#include "OSAL/OSAL_PlatformBackend.hpp"



namespace HAL
{
	namespace GPU
	{
		using namespace LAL;

		// Static Data

		bss< AppInstance::Handle> AppStateRef;

		bss< PhysicalDeviceList> PhysicalDevices;



		// Functions

		namespace PlatformBackend
		{
			using Core::Meta::Vulkan_ValidationLayersEnabled;


			#if HAL_GPU_PLATFORM == HAL_GPU_PLATFORM_VULKAN

				using namespace Vulkan;

				using ExtensionIdentifierList = std::vector< RoCStr>;
				using ValidationLayerList     = std::vector< RoCStr>;

				data< ValidationLayerList> ValidationLayerIdentifiers =
				{
					ValidationLayer_Khronos
				};

			#endif


			// Forwards
			
			Where<Vulkan_ValidationLayersEnabled,
			void> PopulateDebugMessengerCreateInfo(Debug::Messenger::CreateInfo& _msngrCreateInfo);


			// Functions

			/*
			Checks to see if the validation layers specified are supported.
			*/
			Where<Vulkan_ValidationLayersEnabled,
			bool> CheckValidationLayerSupport(ValidationLayerList& _validationLayersSpecified)
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
				stack<uint32> numExtensions;

				constexpr
				bool IsUsingGLFW = OSAL::PlatformBackend::WindowAL == OSAL::PlatformBackend::WindowAL;


				stack<CStrArray> extensionsRequired;

				if (IsUsingGLFW)
				{
					extensionsRequired = CStrArray(SAL::GLFW::GetRequiredVulkanAppExtensions(numExtensions));
				}
				else
				{
					extensionsRequired = nullptr;
				}

				ExtensionIdentifierList extensions(extensionsRequired, extensionsRequired + numExtensions);


				if (Vulkan_ValidationLayersEnabled)
				{
					extensions.push_back(Vulkan::Debug::Extension_DebugUtility);
				}

				return extensions;
			}

			Where< Vulkan_ValidationLayersEnabled,
			Bool> DebugCallback
			(
					  Debug::MessageServerityFlags   _messageServerity, 
					  Debug::MessageTypeFlags        _messageType     ,
				const Debug::Messenger::CallbackData _callbackData    , 
					  ptr<void>                      _userData
			)
			{
				std::cerr << "Vulkan: Validation Layer: " << _callbackData.Message << std::endl;

				using Vulkan::EBool;

				return EBool::True;
			}

			Where< (GPU_API == EGPUPlatformAPI::Vulkan), 
			void> InitializeVulkanApplication(RoCStr _appName, AppVersion _version)
			{
				using Core::Meta::EEngineVersion;

				stack< AppInstance::AppInfo   > appSpec      {};
				stack< AppInstance::CreateInfo> appCreateSpec{};

				if (Vulkan_ValidationLayersEnabled)
				{
					if (!CheckValidationLayerSupport(ValidationLayerIdentifiers))
						throw std::runtime_error("Validation layers requested, but are not available!");
				}

				//AppSpec.SType         = EStructureType::ApplicationInformation;   // Use later...
				appSpec.SType         = EStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO         ;
				appSpec.AppName       = _appName                                                   ;
				appSpec.AppVersion    = MakeVersion(_version.Major, _version.Minor, _version.Patch);
				appSpec.EngineName    = Core::Meta::EngineName                                     ;
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
					using namespace Vulkan::Debug;

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

			Where<Vulkan_ValidationLayersEnabled,
			void> PopulateDebugMessengerCreateInfo(Debug::Messenger::CreateInfo& _msngrCreateInfo)
			{
				_msngrCreateInfo.SType = EStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

				using EMaskS = decltype(_msngrCreateInfo.Serverity)::Enum;

				_msngrCreateInfo.Serverity.Set(EMaskS::Verbose, EMaskS::Warning, EMaskS::Error);

				using EMaskT = decltype(_msngrCreateInfo.Type)::Enum;

				_msngrCreateInfo.Type.Set(EMaskT::General, EMaskT::Validation, EMaskT::Performance);

				using addrs = decltype(&DebugCallback);

				_msngrCreateInfo.UserCallback = EnforceConvention(Vulkan::EnforcerID_Vulkan, DebugCallback);

				_msngrCreateInfo.UserData = nullptr;
			}
		}


		void InitalizeApplication(RoCStr _appName, AppVersion _version)
		{
		
		}
	}
}
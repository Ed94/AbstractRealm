/*
Triangle Test

Last Modified: 5/19/2020

https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code
*/



#pragma once


//#include <vulkan/vulkan.h>

#include "PAL/HAL/Vulkan/Vulkan.hpp"
#include "Core/IO/Basic_FileIO.hpp"
#include "Core/Memory/MemTypes.hpp"
#include "Core/Meta/EngineInfo.hpp"
#include "Core/Meta/Config/HAL_Flags.hpp"
#include "Renderer/Shader/TriangleShader/TriangleShader.hpp"
#include "PAL/SAL/GLFW_SAL.hpp"
#include "LAL.hpp"



namespace Debug
{
	namespace Vulkan_TriangleTest
	{
		using namespace Vulkan;
		using namespace LAL;
		using namespace Core::Meta;

		using AppInfo       = AppInstance::AppInfo   ;
		using AppCreateInfo = AppInstance::CreateInfo;
		using AppHandle     = AppInstance::Handle    ;

		using SAL::GLFW::Window;
		data< ptr<Window> > TriangleWindow = nullptr;

		data<int> Width = 1280, Height = 720;

		bss<AppHandle> App;

		bss<SwapChain::Handle> SwapChain;
		
		using ImageList = std::vector<Image::Handle>;

		ImageList SwapChain_Images;

		EImageFormat SwapChain_ImageFormat;
		Extent2D SwapChain_Extent;

		using ValidationLayerList = std::vector< RoCStr >;

		ValidationLayerList ValidationLayerIdentifiers =
		{
			ValidationLayer_Khronos
		};


		using ExtensionIdentifierList = std::vector< RoCStr >;

		ExtensionIdentifierList DeviceExtensions =
		{
			Swapchain_ExtensionName
		};

		ExtensionIdentifierList GetRequiredExtensions()
		{
			stack<uint32> numExtensions;

			stack<CStrArray> extensionsRequired = CStrArray(SAL::GLFW::GetRequiredVulkanAppExtensions(numExtensions) );

			ExtensionIdentifierList extensions(extensionsRequired, extensionsRequired + numExtensions);

			if (Vulkan_ValidationLayersEnabled)
			{
				extensions.push_back(Vulkan::Debug::Extension_DebugUtility);
			}

			return extensions;
		}

		Where<Vulkan_ValidationLayersEnabled,
		bool> CheckValidationLayerSupport()
		{
			uInt32&& layerCount = GetNumOf_AvailableGlobalLayerProperties();

			using LayerPropertyList = std::vector<LayerProperties>;

			stack<LayerPropertyList> availableLayers(layerCount);

			GetAvailableGlobalLayerProperties(availableLayers.data() );

			stack<bool> layerFound = false;

			for (auto validationLayerName : ValidationLayerIdentifiers)
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

		using namespace Vulkan::Debug;

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





		void PopulateDebugMessengerCreateInfo(Messenger::CreateInfo& _msngrCreateInfo)
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


		bss<Surface::Handle> SurfaceHandle;

		void CreateSurface()
		{
			Surface::CreateInfo createInfo {};

			createInfo.SType = OS_SurfaceCreateInfoEnum;

			createInfo.OSWinHandle = SAL::GLFW::GetOS_WindowHandle(TriangleWindow);

			createInfo.OSAppHandle = GetOS_AppHandle();

			//if (Vulkan::CreateSurface(App, createInfo, nullptr, SurfaceHandle) != EResult::Success) 
			//{
				//throw std::runtime_error("Vulkan, TriangleTest: Failed to create window surface!");
			//}

			if (EResult(SAL::GLFW::CreateWindowSurface(App, TriangleWindow, nullptr, SurfaceHandle)) != EResult::Success)
			{
				throw std::runtime_error("Vulkan, TriangleTest: Failed to create window surface!");
			}
		}

		bss<Messenger::CreateInfo> NotSureYet;
		
		void CreateInstance()
		{
			stack<AppInfo      > appSpec {};
			stack<AppCreateInfo> appCreateSpec {};

			if (Vulkan_ValidationLayersEnabled)
			{
				if (!CheckValidationLayerSupport())
					throw std::runtime_error("Validation layers requested, but are not available!");
			}

			//AppSpec.SType         = EStructureType::ApplicationInformation;   // Use later...
			appSpec.SType         = EStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appSpec.AppName       = "Triangle Test"                       ;
			appSpec.AppVersion    = MakeVersion(1, 0, 0)                  ;
			appSpec.EngineName    = "AnEngine"                            ;
			appSpec.EngineVersion = MakeVersion
			(
				EEngineVersion::Major, 
				EEngineVersion::Minor, 
				EEngineVersion::Patch
			)                                                             ;
			appSpec.API_Version   = EAPI_Version::_1_0                    ;

			//AppCreateSpec.SType   = EStructureType::InstanceCreateInfo;
			appCreateSpec.SType   = EStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			appCreateSpec.AppInfo = getAddress(appSpec)       ;

			/*AppCreateSpec.EnabledExtensionNames = GetRequiredVulkanAppExtensions
			(
				getAddress(AppCreateSpec.EnabledExtensionCount)
			);*/

			ExtensionIdentifierList extensions = GetRequiredExtensions();

			appCreateSpec.EnabledExtensionCount = SCast<uint32>(extensions.size());
			appCreateSpec.EnabledExtensionNames = extensions.data();


			if (Vulkan_ValidationLayersEnabled)
			{
				appCreateSpec.EnabledLayerCount = SCast<uint32>(ValidationLayerIdentifiers.size());

				appCreateSpec.EnabledLayerNames = ValidationLayerIdentifiers.data();

				PopulateDebugMessengerCreateInfo(NotSureYet);

				appCreateSpec.Extension = ptr<Messenger::CreateInfo>(getAddress(NotSureYet));

				//AppCreateSpec.Extension = nullptr;
			}
			else
			{
				appCreateSpec.EnabledLayerCount = 0;

				appCreateSpec.Extension = nullptr;
			}

			EResult&& creationResult = AppInstance_Create(appCreateSpec, nullptr, App);
			//EResult&& creationResult = EResult(vkCreateInstance(&VkInstanceCreateInfo(appCreateSpec), nullptr, getAddress(App)));

			if (creationResult != EResult::Success) 
				throw std::runtime_error("Triangle Test: Failed to create Vulkan app instance.");
		}



		data<PhysicalDevice::Handle> PhysicalDevice = PhysicalDevice::NullHandle();

		struct QueueFamilyIndices
		{
			Maybe<uint32> GraphicsFamily    ;
			Maybe<uint32> PresentationFamily;


			bool IsComplete()
			{
				return
					GraphicsFamily.has_value() &&
					PresentationFamily.has_value()
				;
			}
		};

		using SurfaceFormatList           = std::vector<Surface::Format  >;
		using SurfacePresentationModeList = std::vector<EPresentationMode>;

		struct SwapChainSupportDetails
		{
			Surface::Capabilities       Capabilities     ;
			SurfaceFormatList           Formats          ;
			SurfacePresentationModeList PresentationModes;
		};

		SwapChainSupportDetails QuerySwapChainSupport(PhysicalDevice::Handle _deviceHandle)
		{
			SwapChainSupportDetails details;

			Surface_GetPhysicalDeviceCapabilities(_deviceHandle, SurfaceHandle, details.Capabilities);

			uint32 formatCount;

			formatCount = Surface_GetNumOf_AvailableFormats(_deviceHandle, SurfaceHandle);

			if (formatCount > 0)
			{
				details.Formats.resize(formatCount);

				Surface_GetAvailableFormats(_deviceHandle, SurfaceHandle, details.Formats.data());
			}

			uint32 presentationModeCount;

			presentationModeCount = Surface_GetNumOf_SupportedPresentationModes(_deviceHandle, SurfaceHandle);

			if (presentationModeCount > 0)
			{
				details.PresentationModes.resize(presentationModeCount);

				Surface_GetSupportedPresentationModes(_deviceHandle, SurfaceHandle, details.PresentationModes.data());
			}

			return details;
		}

		QueueFamilyIndices FindQueueFamilies(PhysicalDevice::Handle _deviceHandle)
		{
			QueueFamilyIndices indices{};

			using QueueFamilyPropertiesListing = std::vector<PhysicalDevice::QueueFamilyProperties>;
				
			uint32 queueFamilyCount = GetPhysicalDevice_NumOf_QueueFamilyProperties(_deviceHandle);

			QueueFamilyPropertiesListing queueFamilies(queueFamilyCount);

			GetPhysicalDevice_QueueFamilyProperties(_deviceHandle, queueFamilies.data());

			int index = 0;

			for (const auto& queueFamily : queueFamilies) 
			{
				if ( queueFamily.QueueFlags.Has(EQueue::Graphics) )
				{
					indices.GraphicsFamily = index;
				}

				Bool presentationSupport = EBool::False;

				Surface_CheckPhysicalDeviceSupport(_deviceHandle, index, SurfaceHandle, presentationSupport);

				if (presentationSupport)
				{
					indices.PresentationFamily = index;
				}

				if (indices.IsComplete())
				{
					break;
				}

				index++;

				if (index == queueFamilyCount)
				{
					break;
				}
			}

			return indices;
		}

		bool PhysicalDevice_CheckExtensionSupport(PhysicalDevice::Handle _handle)
		{
			using ExtensionPropertiesList = std::vector<ExtensionProperties>;

			ExtensionPropertiesList availableExtensions(PhysicalDevice_GetNumOfAvailableExtensions(_handle));

			PhysicalDevice_GetAvailableExtensions(_handle, availableExtensions.data());

			using ExtensionNameSet = std::set<std::string>;

			ExtensionNameSet requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

			for (const auto& extension : availableExtensions)
			{
				requiredExtensions.erase(extension.ExtensionName);
			}

			bool&& isSupported = requiredExtensions.empty();

			return isSupported;
		}


		bool IsDeviceSuitable(PhysicalDevice::Handle _deviceHandle)
		{
			stack<PhysicalDevice::Properties> deviceProperties;

			GetPhysicalDevice_Properties(_deviceHandle, deviceProperties);

			stack<PhysicalDevice::Features> deviceFeatures;

			GetPhysicalDevice_Features(_deviceHandle, deviceFeatures);

			QueueFamilyIndices indices = FindQueueFamilies(_deviceHandle);

			bool extensionsSupported = PhysicalDevice_CheckExtensionSupport(_deviceHandle);

			bool swapChainAdequate = false;

			if (extensionsSupported)
			{
				SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_deviceHandle);

				swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentationModes.empty();
			}

			bool&& result = 
				bool(deviceFeatures.GeometryShader) &&
				indices.IsComplete()                &&
				extensionsSupported                 &&
				swapChainAdequate                     ;

			return result;
		}

		int RateDeviceSuitability(PhysicalDevice::Handle _deviceHandle)
		{
			stack<PhysicalDevice::Properties> deviceProperties;

			GetPhysicalDevice_Properties(_deviceHandle, deviceProperties);

			stack<PhysicalDevice::Features> deviceFeatures;

			GetPhysicalDevice_Features(_deviceHandle, deviceFeatures);

			int score = 0;

			deviceProperties.Type == EPhysicalDeviceType::DiscreteGPU ? score += 1000 : NULL;

			score += deviceProperties.LimitsSpec.MaxImageDimension2D;

			if (!deviceFeatures.GeometryShader) 
			{
				return 0;
			}

			return score;
		}

		void PickPhysicalDevice()
		{
			stack<uint32> numDevices = GetNumOf_PhysicalDevices(App);

			if (numDevices == 0)
				throw std::runtime_error("Physical device count 0. No GPUs found with vulkan support.");

			using PhysicalDeviceList = std::vector<PhysicalDevice::Handle>;

			PhysicalDeviceList physicalDevices(numDevices);

			GetAvailablePhysicalDevices(App, physicalDevices.data());

			for (const auto& device : physicalDevices)
			{
				if (IsDeviceSuitable(device))
				{
					PhysicalDevice = device;

					break;
				}
			}

			if (PhysicalDevice == PhysicalDevice::NullHandle())
			{
				throw std::runtime_error("Not able to find suitable Vulkan supported GPU.");
			}
		}

		LogicalDevice::Handle LogicalDevice;
		LogicalDevice::Queue::Handle GraphicsQueue;
		LogicalDevice::Queue::Handle PresentationQueue;

		void CreateLogicalDevice()	
		{
			QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);

			using LogicalDevice_QueueCreateInfoList = std::vector<LogicalDevice::Queue::CreateInfo>;

			LogicalDevice_QueueCreateInfoList queueCreateInfos;

			using IndiceSet = std::set<uint32_t>;

			IndiceSet queueFamiliesToCreate =
			{
				indices.GraphicsFamily.value(),
				indices.PresentationFamily.value()
			};

			float queuePriority = 1.0f;

			for (uint32_t queueFamily : queueFamiliesToCreate)
			{
				LogicalDevice::Queue::CreateInfo queueCreateInfo{};

				queueCreateInfo.SType = EStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

				queueCreateInfo.QueueFamilyIndex = queueFamily;

				queueCreateInfo.QueueCount = 1; 

				queueCreateInfo.QueuePriorities = &queuePriority;

				queueCreateInfos.push_back(queueCreateInfo);
			}

			//queueCreateInfo.QueuePriorities = &queuePriority;

			PhysicalDevice::Features physDeviceFeatures{};

			LogicalDevice::CreateInfo createInfo{};

			createInfo.SType = EStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

			createInfo.QueueCreateInfoCount = queueCreateInfos.size();

			createInfo.QueueCreateInfos = queueCreateInfos.data();

			createInfo.EnabledFeatures = &physDeviceFeatures;

			createInfo.EnabledExtensionNames = DeviceExtensions.data();

			createInfo.EnabledExtensionCount = DeviceExtensions.size();

			if (Vulkan_ValidationLayersEnabled)
			{
				createInfo.EnabledLayerCount = SCast<uint32>(ValidationLayerIdentifiers.size());

				createInfo.EnabledLayerNames = ValidationLayerIdentifiers.data();
			}
			else
			{
				createInfo.EnabledLayerCount = 0;
			}

			EResult&& result = LogicalDevice_CreateDevice(PhysicalDevice, createInfo, nullptr, LogicalDevice);

			if (result != EResult::Success)
			{
				if (result != EResult::Success)
				{
					throw std::runtime_error("Vulkan, TraingleTest: Failed to create logical device!");
				}
			}

			LogicalDevice_GetQueue(LogicalDevice, indices.GraphicsFamily    .value(), 0, GraphicsQueue    );
			LogicalDevice_GetQueue(LogicalDevice, indices.PresentationFamily.value(), 0, PresentationQueue);
		}

		bss<Messenger::Handle> MsngrHandle;

		void SetupDebugMessenger()
		{
			if (!Vulkan_ValidationLayersEnabled) return;

			stack<Messenger::CreateInfo> msngrCreateSpec {};

			PopulateDebugMessengerCreateInfo(msngrCreateSpec);

			EResult&& creationResult = CreateMessenger(App, msngrCreateSpec, nullptr, MsngrHandle);
			//EResult&& creationResult = EResult(CreateDebugUtilsMessengerEXT(App, &VkDebugUtilsMessengerCreateInfoEXT(msngrCreateSpec), nullptr, &MsngrHandle));

			if (creationResult != EResult::Success) throw std::runtime_error("Failed to setup debug messenger!");
		}

		Surface::Format Surface_SwapChain_ChooseFormat(const SurfaceFormatList& _availableFormats)
		{
			for (const auto& availableFormat : _availableFormats)
			{
				if 
				(
					availableFormat.Format     == EImageFormat::VK_FORMAT_B8G8R8A8_SRGB &&
					availableFormat.ColorSpace == EColorSpace::KHR_SRGB_NonLinear         
				)
				{
					return availableFormat;
				}
			}

			// Just pick the first format...
			return _availableFormats[0];
		}

		EPresentationMode Surface_SwapChain_ChoosePresentationMode(const SurfacePresentationModeList _surfacePresentationModes)
		{
			for (const auto& availablePresentationMode : _surfacePresentationModes)
			{
				if (availablePresentationMode == EPresentationMode::KHR_Mailbox)
				{
					return availablePresentationMode;
				}
			}

			return EPresentationMode::KHR_FIFO;
		}

		Extent2D Surface_SwapChain_ChooseExtent(const Surface::Capabilities& _capabilities)
		{
			if (_capabilities.CurrentExtent.Width != UInt32Max)
			{
				return _capabilities.CurrentExtent;
			}
			else
			{
				Extent2D actualExtent = { Width, Height };

				actualExtent.Width  = std::clamp(actualExtent.Width , _capabilities.MinImageExtent.Width , _capabilities.MaxImageExtent.Width );
				actualExtent.Height = std::clamp(actualExtent.Height, _capabilities.MinImageExtent.Height, _capabilities.MaxImageExtent.Height);

				return actualExtent;
			}
		}

		void CreateSwapChain()
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(PhysicalDevice);

			Surface::Format surfaceFormat = Surface_SwapChain_ChooseFormat(swapChainSupport.Formats);

			EPresentationMode presentationMode = Surface_SwapChain_ChoosePresentationMode(swapChainSupport.PresentationModes);

			Extent2D extent = Surface_SwapChain_ChooseExtent(swapChainSupport.Capabilities);

			uint32 numImagesDesired = swapChainSupport.Capabilities.MinImageCount;

			numImagesDesired += 1;

			if (swapChainSupport.Capabilities.MaxImageCount > 0 && numImagesDesired > swapChainSupport.Capabilities.MaxImageCount)
			{
				numImagesDesired = swapChainSupport.Capabilities.MaxImageCount;
			}

			SwapChain::CreateInfo creationSpec {};

			creationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			creationSpec.Surface = SurfaceHandle;
			creationSpec.MinImageCount = numImagesDesired;
			creationSpec.ImageFormat = surfaceFormat.Format;
			creationSpec.ImageColorSpace = surfaceFormat.ColorSpace;
			creationSpec.ImageExtent = extent;
			creationSpec.ImageArrayLayers = 1;
			creationSpec.ImageUsage.Set(EImageUsage::Color_Attachment);

			QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);

			uint32_t queueFamilyIndices[] = 
			{
				indices.GraphicsFamily    .value(), 
				indices.PresentationFamily.value() 
			};

			if (indices.GraphicsFamily != indices.PresentationFamily) 
			{
				creationSpec.ImageSharingMode      = ESharingMode::Concurrent;
				creationSpec.QueueFamilyIndexCount = 2;
				creationSpec.QueueFamilyIndices    = queueFamilyIndices;
			}
			else 
			{
				creationSpec.ImageSharingMode      = ESharingMode::Excusive;
				creationSpec.QueueFamilyIndexCount = 0                     ; // Optional
				creationSpec.QueueFamilyIndices    = nullptr               ; // Optional
			}

			creationSpec.PreTransform = swapChainSupport.Capabilities.CurrentTransform;

			creationSpec.CompositeAlpha = ECompositeAlpha::Opaque;

			creationSpec.PresentationMode = presentationMode;

			creationSpec.Clipped = true;

			creationSpec.OldSwapchain = SwapChain::NullHandle();

			EResult&& creationResult = CreateSwapChain(LogicalDevice, creationSpec, nullptr, SwapChain);

			if (creationResult != EResult::Success)
			{
				throw std::runtime_error("Failed to create the swap chain!");
			}

			SwapChain_GetImages(LogicalDevice, SwapChain, SwapChain_Images.data());

			SwapChain_ImageFormat = surfaceFormat.Format;
		}

		using ImageViewList = std::vector<ImageView::Handle>;

		ImageViewList swapChainImageViews;

		void CreateImageViews()
		{
			swapChainImageViews.resize(SwapChain_Images.size());

			for (DataSize index = 0; index < SwapChain_Images.size(); index++)
			{
				ImageView::CreateInfo creationSpec {};

				creationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

				creationSpec.Image = SwapChain_Images[index];

				creationSpec.ViewType = EImageViewType::_2D;

				creationSpec.Format = SwapChain_ImageFormat;

				creationSpec.Components.R = EComponentSwizzle::Identitity;
				creationSpec.Components.G = EComponentSwizzle::Identitity;
				creationSpec.Components.B = EComponentSwizzle::Identitity;
				creationSpec.Components.A = EComponentSwizzle::Identitity;

				creationSpec.SubresourceRange.AspectMask.Set(EImageAspect::Color);

				creationSpec.SubresourceRange.BaseMipLevel   = 0;
				creationSpec.SubresourceRange.LevelCount     = 1;
				creationSpec.SubresourceRange.BaseArrayLayer = 0;
				creationSpec.SubresourceRange.LayerCount     = 1;

				EResult&& creationResult = CreateImageView(LogicalDevice, creationSpec, nullptr, &swapChainImageViews[index]);

				if (creationResult != EResult::Success)
				{
					throw std::runtime_error("Failed to create image views!");
				}
			}
		}


		using Bytecode_Buffer = DynamicArray<SPIR_V::Bytecode>;

		ShaderModule::Handle CreateTriShaderModule(const IO::FileBuffer& code)
		{
			using ByteCode = uint32;

			ShaderModule::CreateInfo creationSpec{};

			creationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

			creationSpec.Extension = NULL;

			creationSpec.CodeSize = code.size();

			creationSpec.Code = RCast<const ByteCode>(code.data());

			ShaderModule::Handle createdModule;

			EResult&& creationResult = ShaderModule_Create(LogicalDevice, creationSpec, nullptr, createdModule);

			if (creationResult != EResult::Success)
			{
				throw RuntimeError("Failed to create TriShader module!");
			}

			return createdModule;
		}

		Pipeline::Layout::Handle	 PipelineLayout;

		void CreateGraphicsPipeline()
		{
			// Shader setup

			using namespace Renderer::Shader;

			auto triShader_VertCode = IO::BufferFile(String(Paths::TriangleShader) + "TriangleShader_Vert.spv");
			auto triShader_FragCode = IO::BufferFile(String(Paths::TriangleShader) + "TriangleShader_Frag.spv");

			//TODO: FIGURE THIS OUT.
			Bytecode_Buffer triShader_Vert_Bytecode(triShader_VertCode.begin(), triShader_VertCode.end());
			Bytecode_Buffer triShader_Frag_Bytecode(triShader_FragCode.begin(), triShader_FragCode.end());

			ShaderModule::Handle triShaderModule_Vert = CreateTriShaderModule(triShader_VertCode);
			ShaderModule::Handle triShaderModule_Frag = CreateTriShaderModule(triShader_FragCode);

			Pipeline::ShaderStage::CreateInfo triShaderStage_Vert_CreationSpec{};
			Pipeline::ShaderStage::CreateInfo triShaderStage_Frag_CreationSpec{};

			triShaderStage_Vert_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			triShaderStage_Vert_CreationSpec.Stage = EShaderStageFlag::Vertex;

			triShaderStage_Vert_CreationSpec.Module = triShaderModule_Vert;
			triShaderStage_Vert_CreationSpec.Name = "main";

			triShaderStage_Frag_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			triShaderStage_Frag_CreationSpec.Stage = EShaderStageFlag::Fragment;

			triShaderStage_Frag_CreationSpec.Module = triShaderModule_Vert;
			triShaderStage_Frag_CreationSpec.Name = "main";

			Pipeline::ShaderStage::CreateInfo shaderStages[] = { triShaderStage_Vert_CreationSpec, triShaderStage_Frag_CreationSpec };

			// Fixed Function

			Pipeline::VertexInputState::CreateInfo vertexInputState_CreationSpec{};

			vertexInputState_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

			vertexInputState_CreationSpec.VertexBindingDescriptionCount = 0;
			vertexInputState_CreationSpec.BindingDescriptions = nullptr;

			vertexInputState_CreationSpec.AttributeDescriptionCount = 0;
			vertexInputState_CreationSpec.AttributeDescription = nullptr;

			Pipeline::InputAssembly::CreateInfo inputAssembly_CreationSpec{};

			inputAssembly_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

			inputAssembly_CreationSpec.Topology = EPrimitiveTopology::TriangleList;
			inputAssembly_CreationSpec.PrimitiveRestartEnable = false;

			Viewport viewport{};

			viewport.X = 0.0f;
			viewport.Y = 0.0f;
			viewport.Width = float(SwapChain_Extent.Width);
			viewport.Height = float(SwapChain_Extent.Height);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;

			Rect2D scissor{};

			scissor.Offset = { 0,0 };
			scissor.Extent = SwapChain_Extent;

			Pipeline::RasterizationState::CreateInfo rasterizer{};

			rasterizer.SType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.EnableDepthClamp = EBool::False;
			rasterizer.EnableRasterizerDiscard = EBool::False;
			rasterizer.PolygonMode = EPolygonMode::Fill;
			rasterizer.LineWidth = 1.0f;
			rasterizer.CullMode.Set(ECullModeFlag::Back);
			rasterizer.FrontFace = EFrontFace::Clockwise;
			rasterizer.EnableDepthBias = EBool::False;
			rasterizer.DepthBiasConstantFactor = 0.0f;
			rasterizer.DepthBiasClamp = 0.0f;
			rasterizer.DepthBiasSlopeFactor = 0.0f;

			Pipeline::MultisampleState::CreateInfo multisampling_CreationSpec{};

			multisampling_CreationSpec.SType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling_CreationSpec.EnableSampleShading = EBool::False;
			multisampling_CreationSpec.RasterizationSamples = ESampleCount::_1;
			multisampling_CreationSpec.MinSampleShading = 1.0f;
			multisampling_CreationSpec.SampleMask = nullptr;
			multisampling_CreationSpec.EnableAlphaToCoverage = EBool::False;
			multisampling_CreationSpec.EnableAlphaToOne = EBool::False;

			Pipeline::ColorBlend::AttachmentState colorBlend_Attachment{};

			colorBlend_Attachment.ColorWriteMask.Set(EColorComponentFlag::R, EColorComponentFlag::G, EColorComponentFlag::B, EColorComponentFlag::A);
			colorBlend_Attachment.EnableBlend = EBool::False;
			colorBlend_Attachment.Source_ColorBlendFactor = EBlendFactor::One;
			colorBlend_Attachment.Destination_ColorBlendFactor = EBlendFactor::Zero;
			colorBlend_Attachment.ColorOperation = EBlendOperation::Add;
			colorBlend_Attachment.Source_AlphaBlendFactor = EBlendFactor::One;
			colorBlend_Attachment.Destination_AlphaBlendFactor = EBlendFactor::Zero;
			colorBlend_Attachment.AlphaOperation = EBlendOperation::Add;

			Pipeline::ColorBlend::CreateInfo colorBlending_CreationSpec{};

			colorBlending_CreationSpec.SType = EStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending_CreationSpec.EnableLogicOperations = EBool::False;
			colorBlending_CreationSpec.LogicOperation = ELogicOperation::Copy;
			colorBlending_CreationSpec.AttachmentCount = 1;
			colorBlending_CreationSpec.Attachments = &colorBlend_Attachment;
			colorBlending_CreationSpec.BlendConstants[0] = 0.0f;
			colorBlending_CreationSpec.BlendConstants[1] = 0.0f;
			colorBlending_CreationSpec.BlendConstants[2] = 0.0f;
			colorBlending_CreationSpec.BlendConstants[3] = 0.0f;

			EDynamicState States[] =
			{
				EDynamicState::Viewport,
				EDynamicState::LineWidth
			};

			Pipeline::DynamicState::CreateInfo dynamicState {};

			dynamicState.SType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.StateCount = 2;
			dynamicState.States = States;

			Pipeline::Layout::CreateInfo pipelineLayout_CreationSpec;

			pipelineLayout_CreationSpec.SType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayout_CreationSpec.SetLayoutCount = 0;
			pipelineLayout_CreationSpec.SetLayouts = nullptr;
			pipelineLayout_CreationSpec.PushConstantRangeCount = 0;
			pipelineLayout_CreationSpec.PushConstantRanges = nullptr;

			EResult&& piplineLayout_CreationResult = 
				Pipeline_CreateLayout(LogicalDevice, pipelineLayout_CreationSpec, nullptr, PipelineLayout);

			if (piplineLayout_CreationResult != EResult::Success)
			{
				throw std::runtime_error("Failed to create pipeline layout!");
			}


			ShaderModule_Destory(LogicalDevice, triShaderModule_Vert, nullptr);
			ShaderModule_Destory(LogicalDevice, triShaderModule_Frag, nullptr);
		}	

		void InitWindow()
		{
			using namespace SAL::GLFW;

			Initalize();

			using SAL::GLFW::EBool;

			SetWindowCreationParameter(EWindowCreationParameter::ClientAPI, EClient_API::No_API);
			SetWindowCreationParameter(EWindowCreationParameter::Resizable, EBool      ::False );

			TriangleWindow =
				MakeWindow(Width, Height, "AnEngine: Triangle Test", WindowedMode(), NotShared());
		}

		void InitVulkan()
		{
			CreateInstance();

			SetupDebugMessenger();

			CreateSurface();

			PickPhysicalDevice();

			CreateLogicalDevice();

			CreateSwapChain();

			CreateImageViews();

			CreateGraphicsPipeline();
		}

		void MainLoop()
		{
			using namespace SAL::GLFW;

			while (! CanClose(TriangleWindow))
			{
				PollEvents();
			}
		}

		void Cleanup()
		{
			using namespace SAL::GLFW;

			Pipeline_DestroyLayout(LogicalDevice, PipelineLayout, nullptr);

			for (auto iamgeView : swapChainImageViews)
			{
				DestroyImageView(LogicalDevice, iamgeView, nullptr);
			}

			DestroySwapChain(LogicalDevice, SwapChain, nullptr);

			LogicalDevice_Destory(LogicalDevice, nullptr);

			if (Vulkan_ValidationLayersEnabled) DestroyMessenger(App, MsngrHandle, nullptr);

			DestroySurface(App, SurfaceHandle, nullptr);

			AppInstance_Destory(App, nullptr);

			DestroyWindow(TriangleWindow);

			Terminate();
		}

		int Run()
		{
			try
			{
				InitWindow();

				InitVulkan();

				MainLoop();

				Cleanup();
			}
			catch (const std::exception& error)
			{
				std::cerr << error.what() << std::endl;

				getchar();

				return EXIT_FAILURE;
			}
		}
	}
}

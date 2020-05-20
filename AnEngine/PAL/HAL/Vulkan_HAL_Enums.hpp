#pragma once



#include <vulkan/vulkan.h>



namespace HAL
{
	namespace Vulkan
	{
		enum class EUndefined
		{};


		enum class EAPI_Version
		{
			_1_0 = VK_API_VERSION_1_0,
			_1_1 = VK_API_VERSION_1_1,
			_1_2 = VK_API_VERSION_1_2
		};

		enum EBool
		{
			True  = VK_TRUE ,
			False = VK_FALSE
		};

		enum class EObjectType
		{
			Unknown                        = VK_OBJECT_TYPE_UNKNOWN                        ,
			Instance                       = VK_OBJECT_TYPE_INSTANCE                       ,
			PhysicalDevice                 = VK_OBJECT_TYPE_PHYSICAL_DEVICE                ,
			Device                         = VK_OBJECT_TYPE_DEVICE                         ,
			Queue                          = VK_OBJECT_TYPE_QUEUE                          ,
			Semaphore                      = VK_OBJECT_TYPE_SEMAPHORE                      ,
			CommandBuffer                  = VK_OBJECT_TYPE_COMMAND_BUFFER                 ,
			Fence                          = VK_OBJECT_TYPE_FENCE                          ,
			DeviceMemory                   = VK_OBJECT_TYPE_DEVICE_MEMORY                  ,
			Buffer                         = VK_OBJECT_TYPE_BUFFER                         ,
			Image                          = VK_OBJECT_TYPE_IMAGE                          ,
			Event                          = VK_OBJECT_TYPE_EVENT                          ,
			QueryPool                      = VK_OBJECT_TYPE_QUERY_POOL                     ,
			BufferView                     = VK_OBJECT_TYPE_BUFFER_VIEW                    ,
			ImageView                      = VK_OBJECT_TYPE_IMAGE_VIEW                     ,
			ShaderModule                   = VK_OBJECT_TYPE_SHADER_MODULE                  ,
			PipelineCache                  = VK_OBJECT_TYPE_PIPELINE_CACHE                 ,
			PipelineLayout                 = VK_OBJECT_TYPE_PIPELINE_LAYOUT                ,
			RenderPass                     = VK_OBJECT_TYPE_RENDER_PASS                    ,
			Pipeline                       = VK_OBJECT_TYPE_PIPELINE                       ,
			DescriptorSetLayout            = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT          ,
			Sampler                        = VK_OBJECT_TYPE_SAMPLER                        ,
			DescriptorPool                 = VK_OBJECT_TYPE_DESCRIPTOR_POOL                ,
			DescriptorSet                  = VK_OBJECT_TYPE_DESCRIPTOR_SET                 ,
			FrameBuffer                    = VK_OBJECT_TYPE_FRAMEBUFFER                    ,
			CommandPool                    = VK_OBJECT_TYPE_COMMAND_POOL                   ,
			Sampler_YCBCR_Conversion       = VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION       ,
			DescriptorUpdateTemplate       = VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE     ,
			KHR_Surface                    = VK_OBJECT_TYPE_SURFACE_KHR                    ,
			KHR_Swapchain                  = VK_OBJECT_TYPE_SWAPCHAIN_KHR                  ,
			KHR_Display                    = VK_OBJECT_TYPE_DISPLAY_KHR                    ,
			KHR_DisplayMode                = VK_OBJECT_TYPE_DISPLAY_MODE_KHR               ,
			Extension_DebugReportCallback  = VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT      ,
			Extension_DebugUtilsMessenger  = VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT      ,
			KHR_AccelerationStructure      = VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR     ,
			Extension_ValidationCache      = VK_OBJECT_TYPE_VALIDATION_CACHE_EXT           ,
			Intel_PerformanceConfiguration = VK_OBJECT_TYPE_PERFORMANCE_CONFIGURATION_INTEL,
			KHR_DeferredOperation          = VK_OBJECT_TYPE_DEFERRED_OPERATION_KHR         ,
			IndirectCommandsLayout         = VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV    ,
			//Extension_PrivateDataSlot = VK_OBJECT_TYPE_PRIVATE_DATA_SLOT_EXT,   // TODO: is this ok?
			Extension_PrivateDataSlot = 1000295000,
			KHR_DescriptorUpdateTemplate   = VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_KHR ,
			KHR_Sampler_YCBCR_Conversion   = VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION_KHR   ,
		 	AccelerationStructure          = VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV
		};

		enum class EResult
		{
			Sucess                                                = VK_SUCCESS                                           ,
			Not_Ready                                             = VK_NOT_READY                                         ,
			Timeout                                               = VK_TIMEOUT                                           ,
			Event_Set                                             = VK_EVENT_SET                                         ,
			Event_Reset                                           = VK_EVENT_RESET                                       ,
			Incomplete                                            = VK_INCOMPLETE                                        ,
			Error_OutOfHostMemory                                 = VK_ERROR_OUT_OF_HOST_MEMORY                          ,
			Error_OutOfDeviceMemory                               = VK_ERROR_OUT_OF_DEVICE_MEMORY                        ,
			Error_InitalizationFailed                             = VK_ERROR_INITIALIZATION_FAILED                       ,
			Error_DeviceLost                                      = VK_ERROR_DEVICE_LOST                                 ,
			Error_MemoryMap_Failed                                = VK_ERROR_MEMORY_MAP_FAILED                           ,
			Error_LayerNotPresent                                 = VK_ERROR_LAYER_NOT_PRESENT                           ,
			Error_ExtensionNotPresent                             = VK_ERROR_EXTENSION_NOT_PRESENT                       ,
			Error_FeatureNotPresent                               = VK_ERROR_FEATURE_NOT_PRESENT                         ,
			Error_IncompatibleDriver                              = VK_ERROR_INCOMPATIBLE_DRIVER                         ,
			Error_TooManyObjects                                  = VK_ERROR_TOO_MANY_OBJECTS                            ,
			Error_FormatNotSupported                              = VK_ERROR_FORMAT_NOT_SUPPORTED                        ,
			Error_FragmentedPool                                  = VK_ERROR_FRAGMENTED_POOL                             ,
			Error_Unknown                                         = VK_ERROR_UNKNOWN                                     ,
			Error_OutOfPoolMemory                                 = VK_ERROR_OUT_OF_POOL_MEMORY                          ,
			Error_InvalidExternalHandle                           = VK_ERROR_INVALID_EXTERNAL_HANDLE                     ,
			Error_Fragmentation                                   = VK_ERROR_FRAGMENTATION                               ,
			Error_InvalidOpaqueCaptureAddress                     = VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS              ,
			Error_KHR_SurfaceLost                                 = VK_ERROR_SURFACE_LOST_KHR                            ,
			Error_KHR_NativeWindowInUse                           = VK_ERROR_NATIVE_WINDOW_IN_USE_KHR                    ,
			KHR_Suboptimal                                        = VK_SUBOPTIMAL_KHR                                    ,
			Error_KHR_OutOfDate                                   = VK_ERROR_OUT_OF_DATE_KHR                             ,
			Error_KHR_IncompatbileDisplay                         = VK_ERROR_INCOMPATIBLE_DISPLAY_KHR                    ,
			Error_Extension_ValidationFailed                      = VK_ERROR_VALIDATION_FAILED_EXT                       ,
			Error_InvalidShaderNV                                 = VK_ERROR_INVALID_SHADER_NV                           ,
			Error_KHR_IncompatibleVersion                         = VK_ERROR_INCOMPATIBLE_VERSION_KHR                    ,
			Error_Extension_Invalid_DRMFormat_ModifierPlaneLayout = VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT,
			Error_Extension_NotPermitted                          = VK_ERROR_NOT_PERMITTED_EXT                           ,
			Error_Extension_FullScreenExculsiveMode_Lost          = VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT         ,
			KHR_Thread_Idle                                       = VK_THREAD_IDLE_KHR                                   ,
			KHR_Thread_Done                                       = VK_THREAD_DONE_KHR                                   ,
			KHR_OperationDeferred                                 = VK_OPERATION_DEFERRED_KHR                            ,
			KHR_OperationNotDeferred                              = VK_OPERATION_NOT_DEFERRED_KHR                        ,
			Extension_PipelineCompileRequired                     = VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT               ,
			Error_KHR_OutOfPoolMemory                             = VK_ERROR_OUT_OF_POOL_MEMORY_KHR                      ,
			Error_KHR_InvalidExternalHandle                       = VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR                 ,
			Error_Extension_Fragmentation                         = VK_ERROR_FRAGMENTATION_EXT                           ,
			Error_Extension_InvalidDeviceAddress                  = VK_ERROR_INVALID_DEVICE_ADDRESS_EXT                  ,
			Error_KHR_InvalidOpaqueCaptureAddress                 = VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR          ,
			Error_Extension_PipelineCompileRequired               = VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT
		};


		namespace TODO
		{
			// TODO: Finish this.
			enum class EStructureType
			{
				ApplicationInformation = VK_STRUCTURE_TYPE_APPLICATION_INFO,
				Instance_CreateInfo = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				DeviceQueue_CreateInfo = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				Device_CreateInfo = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				SubmitInfo = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				MemoryAllocateInfo = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				MappedMemoryRange = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
				BindSparseInfo = VK_STRUCTURE_TYPE_BIND_SPARSE_INFO,
				Fence_CreateInfo = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				Semaphore_CreateInfo = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				Event_CreateInfo = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
				QueryPool_CreateInfo = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
				Buffer_CreateInfo = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				BufferView_CreateInfo = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
				Image_CreateInfo = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				ImageView_CreateInfo = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				ShaderModule_CreateInfo = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				Pipeline_Cache_CreateInfo = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
				Pipeline_ShaderState_CreateInfo = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				Pipeline_VertexInputState_CreateInfo = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				Pipeline_InputAssemblyState_CreateInfo = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
				Pipeline_TessellationState_CreateInfo = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
				Pipeline_ViewportState_CreateInfo = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
				Pipeline_RasterizationState_CreateInfo = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
				Pipeline_MultisampleState_CreateInfo = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				Pipeline_DepthStencilState_CreateInfo = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
				Pipeline_ColorBlendState_CreateInfo = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				Pipeline_DynamicState_CreateInfo = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				GraphicsPipeline_CreateInfo = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
				ComputePipeline_CreateInfo = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
				Pipeline_Layout_CreateInfo = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
				Sampler_CreateInfo = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO
			};
		}

		

		// Bitmaskable
		Bitmaskable EDebugUtilities_MessageSeverityFlags
		{
			Verbose = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
			Info    = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT   ,
			Warning = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
			Error   = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
		};

		// Bitmaskable
		Bitmaskable EDebugUtilities_MessageTypeFlags
		{
			General     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    ,
			Validation  = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT ,
			Performance = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
		};
	}
}

#pragma once



#include <vulkan/vulkan.h>



namespace HAL
{
	namespace Vulkan
	{
		enum class EAPI_Version
		{
			_1_0 = VK_API_VERSION_1_0,
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

		enum class EStructureType
		{
			ApplicationInformation = VK_STRUCTURE_TYPE_APPLICATION_INFO    ,
			CreateInfo             = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		};

		// Bitmask
		Bitmask EDebugUtilities_MessageSeverityFlags
		{
			Verbose = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
			Info    = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT   ,
			Warning = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
			Error   = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
		};

		Bitmask EDebugUtilities_MessageTypeFlags
		{
			General     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    ,
			Validation  = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT ,
			Performance = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
		};
	}
}

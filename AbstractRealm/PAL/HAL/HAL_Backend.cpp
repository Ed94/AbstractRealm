// Parent Header
#include "HAL_Backend.hpp"

Meta_SetupEngineModule_Implementation(HAL);


namespace HAL
{
	using namespace LAL;

	void Load_Backend()
	{
		Meta_EngineModule_Init();
	}

	namespace GPU
	{
		void Log(String _info)
		{
			SubLogger.Record(Dev::ESeverity::Info, "GPU: " + _info);	
		}

		void Log_Error(String _info)
		{
			SubLogger.Record(Dev::ESeverity::Error, "GPU: " + _info);	
		}

		namespace Vulkan
		{
			void Log(String _info)
			{
				SubLogger.Record(Dev::ESeverity::Info, "GPU-Vulkan: " + _info);	
			}

			void Log_Error(String _info)
			{
				SubLogger.Record(Dev::ESeverity::Error, "GPU-Vulkan: " + _info);	
			}
		}
	}
}
// Parent Header
#include "HAL_Backend.hpp"

#include "Dev/Log.hpp"



namespace HAL
{
	using namespace LAL;

	Dev_Declare_Log(HAL);

	void Load_Backend()
	{
		SubLogger.Init();
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
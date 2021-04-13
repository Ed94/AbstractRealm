// Parent Header
#include "HAL_Backend.hpp"

#include "Dev/Log.hpp"



namespace HAL
{
	using namespace LAL;

	Dev::Log log("HAL");

	void Log(String _info)
	{
		CLog(_info);

		log.Record(Dev::Severity::Info , _info);
	}

	using Dev::Severity;

	void Log(Severity _severity, String _info)
	{
		log.Record(_severity, _info);
	}

	// Dev Console Logging

	void CLog(String _info)
	{
		Dev::CLog("HAL: " + _info);
	}

	void CLog_Error(String _info)
	{
		Dev::CLog_Error("HAL: " + _info);
	}

	namespace GPU
	{
		void CLog(String _info)
		{
			Dev::CLog("GPU HAL: " + _info);	
		}

		void CLog_Error(String _info)
		{
			Dev::CLog_Error("GPU HAL: " + _info);	
		}

		namespace Vulkan
		{
			void CLog(String _info)
			{
				Dev::CLog("GPU Vulkan: " + _info);	
			}

			void CLog_Error(String _info)
			{
				Dev::CLog("GPU Vulkan: " + _info);	
			}
		}
	}
}
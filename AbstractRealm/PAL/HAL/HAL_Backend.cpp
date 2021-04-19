// Parent Header
#include "HAL_Backend.hpp"

#include "Dev/Log.hpp"



namespace HAL
{
	using namespace LAL;

	Dev::Log log("HAL");

	/*void Log(String _info)
	{
		log.Record(Dev::Severity::Info , _info);
	}

	//using Dev::Severity;

	void Log(Severity _severity, String _info)
	{
		log.Record(_severity, _info);
	}*/

	// Dev Console Logging

	void Log(String _info)
	{
		log.Record(Dev::Severity::Info , _info);
	}

	void Log_Error(String _info)
	{
		log.Record(Dev::Severity::Error , _info);
	}

	namespace GPU
	{
		void Log(String _info)
		{
			log.Record(Dev::Severity::Info, "GPU: " + _info);	
		}

		void Log_Error(String _info)
		{
			log.Record(Dev::Severity::Error, "GPU: " + _info);	
		}

		namespace Vulkan
		{
			void Log(String _info)
			{
				log.Record(Dev::Severity::Info, "GPU Vulkan: " + _info);	
			}

			void Log_Error(String _info)
			{
				log.Record(Dev::Severity::Error, "GPU Vulkan: " + _info);	
			}
		}
	}
}
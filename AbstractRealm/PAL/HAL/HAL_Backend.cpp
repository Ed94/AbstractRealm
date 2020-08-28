// Parent Header
#include "HAL_Backend.hpp"



namespace HAL
{
	using namespace LAL;

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
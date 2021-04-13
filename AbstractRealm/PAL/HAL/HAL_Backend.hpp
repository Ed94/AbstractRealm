#pragma once



#include "Console.hpp"
#include "Dev/Log.hpp"



namespace HAL
{
	using namespace LAL;

	void Log(String _info);

	using Dev::Severity;

	void Log(Severity _severity, String _info);

	void CLog      (String _info);
	void CLog_Error(String _info);
	
	namespace GPU
	{
		void CLog      (String _info);
		void CLog_Error(String _info);

		namespace Vulkan
		{
			void CLog      (String _info);
			void CLog_Error(String _info);
		}
	}
}

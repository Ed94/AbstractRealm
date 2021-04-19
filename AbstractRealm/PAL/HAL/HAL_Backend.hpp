#pragma once



#include "Console.hpp"
#include "Dev/Log.hpp"



namespace HAL
{
	using namespace LAL;

	void Log(String _info);

	using Dev::Severity;

	void Log(Severity _severity, String _info);

	void Log      (String _info);
	void Log_Error(String _info);
	
	namespace GPU
	{
		void Log      (String _info);
		void Log_Error(String _info);

		namespace Vulkan
		{
			void Log      (String _info);
			void Log_Error(String _info);
		}
	}
}

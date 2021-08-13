#pragma once



#include "Console.hpp"
#include "Dev/Log.hpp"



namespace HAL
{
	using namespace LAL;

	Dev_Declare_LogAPI();

	void Load_Backend();
	
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

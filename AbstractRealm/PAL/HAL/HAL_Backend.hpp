#pragma once


#include "Meta/Meta.hpp"

Meta_SetupEngineModule(HAL);

#include "Console.hpp"
#include "Dev/Log.hpp"


namespace HAL
{
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

#pragma once



#include "Console.hpp"




namespace HAL
{
	using namespace LAL;

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

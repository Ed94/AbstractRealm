// Parent Header
#include "OSAL_Platform.hpp"



// Engine
#include "LAL.hpp"
#include "OSAL_Backend.hpp"

// Infoware
#include "infoware/system.hpp"



namespace OSAL
{
	using namespace LAL;

	String OS_Name;

	struct OS_Version
	{
		uint32 Major;
		uint32 Minor;
		uint32 Patch;

		UINT32 Build;

		String Str()
		{
			StringStream stream; 

			stream << Major << "." << Minor << "." << Patch << " Build: " << Build;

			return stream.str();
		}
	};

	OS_Version OS_Ver;

	void Load_Platform()
	{
		auto osInfo = iware::system::OS_info();

		OS_Name = osInfo.full_name;

		OS_Ver.Major = osInfo.major       ;
		OS_Ver.Minor = osInfo.minor       ;
		OS_Ver.Patch = osInfo.patch       ;
		OS_Ver.Build = osInfo.build_number;

		CLog("OS: " + OS_Name + " Version: " + OS_Ver.Str());
	}
}
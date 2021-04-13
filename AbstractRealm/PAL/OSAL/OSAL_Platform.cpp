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

	const String& Get_OSName() { return OS_Name; }

	const OS_Version& Get_OSVersion() { return OS_Ver; }
}
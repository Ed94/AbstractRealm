#pragma once


#include "OSAL_Backend.hpp"
#include "OSAL/Platform.hpp"


namespace OSAL
{
	namespace PlatformBackend
	{
		template<OSAL::EOS>
		struct TimingAPI_Maker;

		template<>
		struct TimingAPI_Maker<EOS::Windows>
		{
			static constexpr auto GetTime_Local = localtime_s;
		};

		using TimingAPI = TimingAPI_Maker<OSAL::OS>;
	}

	using PlatformBackend::TimingAPI;

	constexpr auto GetTime_Local = TimingAPI::GetTime_Local;

	void Record_EntryPoint_StartExecution();

	void Load_Timing();

	CalendarDate GetTime_UTC();
}

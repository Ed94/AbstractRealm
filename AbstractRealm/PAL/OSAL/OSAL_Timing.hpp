#pragma once


#include "OSAL_Backend.hpp"
#include "OSAL_Platform.hpp"


namespace OSAL
{
	namespace PlatformBackend
	{
		template<OSAL::EOS>
		struct TimingAPI_Maker;

		template<>
		struct TimingAPI_Maker<EOS::Windows>
		{
			static constexpr auto TimeLocal = localtime_s;
		};

		using TimingAPI = TimingAPI_Maker<OSAL::OS>;
	}

	using PlatformBackend::TimingAPI;

	constexpr auto TimeLocal = TimingAPI::TimeLocal;

	void Record_EntryPoint_StartExecution() noexcept;

	const SysTimePoint& GetExecutionStart();

	const CalendarDate& GetExecutionStartDate();

	void Load_Timing();

	CalendarDate& GetTime_Local();

	CalendarDate& GetTime_UTC();
}

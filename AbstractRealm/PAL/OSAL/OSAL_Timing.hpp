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

	template<typename Period> using NanoPeriod = std::ratio_multiply<Period, std::giga>;
	template<typename Period> using MiliPeriod = std::ratio_multiply<Period, std::kilo>;

	struct SystemTimeInfo
	{
		MiliPeriod<SystemTimePeriod> Period;

		f64 Precison;
	};

	struct SteadyTimeInfo
	{
		MiliPeriod<SteadyTimePeriod> Period;

		f64 Precison;
	};

	struct HighResTimeInfo
	{
		MiliPeriod<HighResTimePeriod> Period;

		f64 Precison;
	};

	using PlatformBackend::TimingAPI;

	constexpr auto TimeLocal = TimingAPI::TimeLocal;

	const SystemTimeInfo& Get_SystemTimeInfo();
	const SteadyTimeInfo& Get_SteadyTimeInfo();
	const HighResTimeInfo& Get_HighResTimeInfo();

	void Record_EntryPoint_StartExecution() noexcept;

	const SysTimePoint& GetExecutionStart();

	const CalendarDate& GetExecutionStartDate();

	void Load_Timing();

	CalendarDate& GetTime_Local();

	CalendarDate& GetTime_UTC();
}

// Parent Header
#include "Timing.hpp"

#include "OSAL_Backend.hpp"

#include "Dev/Dev.hpp"


namespace OSAL
{
	template<typename Period>
	using NanoPeriod = std::ratio_multiply<Period, std::giga>;

	template<typename Period>
	using MiliPeriod = std::ratio_multiply<Period, std::kilo>;

	struct SystemTimeInfo
	{
		MiliPeriod<SystemTimePeriod> Period;

		float64 Precison;
	};

	struct SteadyTimeInfo
	{
		MiliPeriod<SteadyTimePeriod> Period;

		float64 Precison;
	};

	struct HiighResTimeInfo
	{
		MiliPeriod<HighResTimePeriod> Period;

		float64 Precison;
	};


	SystemTimeInfo   SysTimeStatus    ;
	SteadyTimeInfo   SteadyTimeStatus ;
	HiighResTimeInfo HighResTimeStatus;

	SysTimePoint EntryPoint_StartExecution;	

	CalendarDate TimeUTC_Buffer;
	CalendarDate TimeLocal_Buffer;



	void GetClock_Accuracies()
	{
		SysTimeStatus    .Precison = float64(SysTimeStatus    .Period.num) / float64(SysTimeStatus    .Period.den);
		SteadyTimeStatus .Precison = float64(SteadyTimeStatus .Period.num) / float64(SteadyTimeStatus .Period.den);
		HighResTimeStatus.Precison = float64(HighResTimeStatus.Period.num) / float64(HighResTimeStatus.Period.den);

		OS_CLog("System Time Accuracy         : " + ToString(SysTimeStatus    .Precison) + " milliseconds");
		OS_CLog("Steady Time Accuracy         : " + ToString(SteadyTimeStatus .Precison) + " milliseconds");
		OS_CLog("High Resolution Time Accuracy: " + ToString(HighResTimeStatus.Precison) + " milliseconds");
	}

	void Load_Timing()
	{
		GetClock_Accuracies();
	}

	void Record_EntryPoint_StartExecution()
	{
		EntryPoint_StartExecution = SystemClock::now();
	}

	CalendarDate& GetTime_UTC()
	{
		const Time timeSnap = SystemClock::to_time_t(SystemClock::now());;

		TimeUTC_Buffer = *TimeUTC(&timeSnap);

		return TimeUTC_Buffer;
	}

	CalendarDate& GetTime_Local()
	{
		const Time timeSnap = SystemClock::to_time_t(SystemClock::now());;

		TimeLocal(&TimeLocal_Buffer, &timeSnap);

		return TimeLocal_Buffer;
	}
}
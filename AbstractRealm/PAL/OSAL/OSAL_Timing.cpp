// Parent Header
#include "OSAL_Timing.hpp"



#include "OSAL_Backend.hpp"



namespace OSAL
{
	SystemTimeInfo  SysTimeStatus    ;
	SteadyTimeInfo  SteadyTimeStatus ;
	HighResTimeInfo HighResTimeStatus;

	SysTimePoint EntryPoint_StartExecution;	
	CalendarDate EntryPoint_StartDate;

	CalendarDate TimeLocal_Buffer;
	CalendarDate TimeUTC_Buffer  ;


	void GetClock_Accuracies()
	{
		SysTimeStatus    .Precison = f64(SysTimeStatus    .Period.num) / f64(SysTimeStatus    .Period.den);
		SteadyTimeStatus .Precison = f64(SteadyTimeStatus .Period.num) / f64(SteadyTimeStatus .Period.den);
		HighResTimeStatus.Precison = f64(HighResTimeStatus.Period.num) / f64(HighResTimeStatus.Period.den);

		CLog("System Time Accuracy         : " + ToString(SysTimeStatus    .Precison) + " milliseconds");
		CLog("Steady Time Accuracy         : " + ToString(SteadyTimeStatus .Precison) + " milliseconds");
		CLog("High Resolution Time Accuracy: " + ToString(HighResTimeStatus.Precison) + " milliseconds");
	}

	const SystemTimeInfo& Get_SystemTimeInfo()
	{
		return SysTimeStatus;
	}

	const SteadyTimeInfo& Get_SteadyTimeInfo()
	{
		return SteadyTimeStatus;
	}

	const HighResTimeInfo& Get_HighResTimeInfo()
	{
		return HighResTimeStatus;
	}

	const SysTimePoint& GetExecutionStart()
	{
		return EntryPoint_StartExecution;
	}

	const CalendarDate& GetExecutionStartDate()
	{
		return EntryPoint_StartDate;
	}

	CalendarDate& GetTime_Local()
	{
		const Time timeSnap = SystemClock::to_time_t(SystemClock::now());;

		TimeLocal(&TimeLocal_Buffer, &timeSnap);

		return TimeLocal_Buffer;
	}

	CalendarDate& GetTime_UTC()
	{
		const Time timeSnap = SystemClock::to_time_t(SystemClock::now());;

		TimeUTC_Buffer = *TimeUTC(&timeSnap);

		return TimeUTC_Buffer;
	}

	void Load_Timing()
	{
		GetClock_Accuracies();
	}

	void Record_EntryPoint_StartExecution() noexcept
	{
		EntryPoint_StartExecution = SystemClock::now();

		const Time timeSnap = SystemClock::to_time_t(EntryPoint_StartExecution);

		TimeLocal(&EntryPoint_StartDate, &timeSnap);
	}
}
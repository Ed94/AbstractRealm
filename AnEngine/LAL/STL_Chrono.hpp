#pragma once


#include "Cpp_STL.hpp"


namespace LAL
{
	using SystemClock      = std::chrono::system_clock;
	using SystemTimePeriod = SystemClock::period      ;
	using SysTimePoint     = SystemClock::time_point  ;
	
	using SteadyClock      = std::chrono::steady_clock;
	using SteadyTimePeriod = SteadyClock::period      ;
	using SteadyTimePoint  = SteadyClock::time_point  ;

	using HighResClock      = std::chrono::high_resolution_clock;
	using HighResTimePeriod = HighResClock::period              ;
	using HighResTimePoint  = HighResClock::time_point          ;



	using Time = std::time_t;

	constexpr auto TimeUTC = std::gmtime;

	// Time Local is defined in OSAL.

	using std::put_time;

	using CalendarDate = std::tm;

	using Microseconds = std::chrono::microseconds;
	using Milliseconds = std::chrono::milliseconds;
	using Miliseconds  = std::chrono::nanoseconds ;
}

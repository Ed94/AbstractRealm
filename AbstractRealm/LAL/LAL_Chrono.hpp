#pragma once


#include "LAL_Cpp_STL.hpp"
#include "LAL_Casting.hpp"


namespace LAL
{
	using SystemClock      = STL::chrono::system_clock;
	using SystemTimePeriod = SystemClock::period      ;
	using SysTimePoint     = SystemClock::time_point  ;
	
	using SteadyClock      = STL::chrono::steady_clock;
	using SteadyTimePeriod = SteadyClock::period      ;
	using SteadyTimePoint  = SteadyClock::time_point  ;

	using HighResClock      = STL::chrono::high_resolution_clock;
	using HighResTimePeriod = HighResClock::period              ;
	using HighResTimePoint  = HighResClock::time_point          ;


	template<class RepresentiveType, class Period = std::ratio<1>>
	using Duration = STL::chrono::duration<RepresentiveType, Period>;

	using Duration64 = Duration<f64>;

	//template<class ToDuration>
	//using CastTODuration = std::chrono::duration_cast<ToDuration>;

	template 
	<
		class ToDuration, 
		class RepresentiveType, 
		class Period, 
		std::enable_if_t<std::chrono::_Is_duration_v<ToDuration>, int> EnabledT
	>
	_NODISCARD constexpr 
	ToDuration CastTo_Duration(const Duration<RepresentiveType, Period>& _duration) 
	noexcept
	(   /* strengthened */
		std::is_arithmetic_v<RepresentiveType> && std::is_arithmetic_v<typename ToDuration::rep>
	)
	{
		// convert duration to another duration; truncate
		using CommonFactor     = std::ratio_divide<Period, typename ToDuration::period>;
		using ToRepresentative = typename ToDuration::rep;
		using CommonRep        = std::common_type_t<ToRepresentative, RepresentiveType, intmax_t>;

		constexpr bool _Num_is_one = CommonFactor::num == 1;
		constexpr bool _Den_is_one = CommonFactor::den == 1;

		if (_Den_is_one) 
		{
			if (_Num_is_one) 
			{
				return SCast<ToDuration>(SCast<ToRepresentative>(_duration.count()));
			}
			else 
			{
				return SCast<ToDuration>(SCast<ToRepresentative>(SCast<CommonRep>(_duration.count()) * SCast<CommonRep>(CommonFactor::num)));
			}
		}
		else 
		{
			if (_Num_is_one) 
			{
				return SCast<ToDuration>(SCast<ToRepresentative>(SCast<CommonRep>(_duration.count()) / SCast<CommonRep>(CommonFactor::den)));
			}
			else 
			{
				return SCast<ToDuration>(SCast<ToRepresentative>(SCast<CommonRep>(_duration.count()) * SCast<CommonRep>(CommonFactor::num) / SCast<CommonRep>(CommonFactor::den)));
			}
		}
	}

	using CalendarDate = STL::tm;

	using Seconds      = STL::chrono::seconds;
	using Microseconds = STL::chrono::microseconds;
	using Milliseconds = STL::chrono::milliseconds;
	using Nanoseconds  = STL::chrono::nanoseconds ;

	using Time = std::time_t;

	constexpr auto ToTime  = SystemClock::to_time_t;
	constexpr auto TimeUTC = STL::gmtime;

	// Time Local is defined in OSAL.

	// using PutTime = std::put_time;
	template <class Elem> _NODISCARD 
	STL::_Timeobj<Elem, ptr<const CalendarDate>> PutTime(const CalendarDate& _date, ptr<const Elem> _elem)
	{
		return STL::_Timeobj<Elem, ptr<const CalendarDate>>(getPtr(_date), _elem);
	}
}

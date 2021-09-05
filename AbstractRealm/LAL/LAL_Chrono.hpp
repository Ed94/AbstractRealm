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


	template<class RepresentiveType, class Period = Ratio<1>>
	using Duration = STL::chrono::duration<RepresentiveType, Period>;

	using Duration32 = Duration<f32>;
	using Duration64 = Duration<f64>;

	// Fails, enable if doesn't work here.
	template<class ToDuration, class RepresentiveType, class Period, STL::enable_if_t<STL::chrono::_Is_duration_v<ToDuration>, int> _Enabled>
	_NODISCARD constexpr 
	ToDuration CastTo_Duration(const Duration<RepresentiveType, Period>& _duration) 
	noexcept
	(   /* strengthened */
		STL::is_arithmetic_v<RepresentiveType> && STL::is_arithmetic_v<typename ToDuration::rep>
	)
	{
		// convert duration to another duration; truncate
		using CommonFactor     = STL::ratio_divide<Period, typename ToDuration::period>;
		using ToRepresentative = typename ToDuration::rep;
		using CommonRep        = STL::common_type_t<ToRepresentative, RepresentiveType, IntMaxType>;

		constexpr bool NumIsOne = CommonFactor::num == 1;
		constexpr bool DenIsOne = CommonFactor::den == 1;

		if (DenIsOne)
		{
			if (NumIsOne)
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
			if (NumIsOne)
			{
				return SCast<ToDuration>(SCast<ToRepresentative>(SCast<CommonRep>(_duration.count()) / SCast<CommonRep>(CommonFactor::den)));
			}
			else 
			{
				return SCast<ToDuration>(SCast<ToRepresentative>(SCast<CommonRep>(_duration.count()) * SCast<CommonRep>(CommonFactor::num) / SCast<CommonRep>(CommonFactor::den)));
			}
		}
	}

	// Fails, template argument deduction doesn't work.
	template<class _To, class _Rep, class _Period>
	constexpr fnPtr<_To, const Duration<_Rep, _Period>&> CastToDuration = STL::chrono::duration_cast<_To, _Rep, _Period>;

	// This thing is cursed.
	using STL::chrono::duration_cast;

	using CalendarDate = STL::tm;

	using Seconds      = STL::chrono::seconds;
	using Microseconds = STL::chrono::microseconds;
	using Milliseconds = STL::chrono::milliseconds;
	using Nanoseconds  = STL::chrono::nanoseconds ;

	using Time = STL::time_t;

	constexpr auto ToTime  = SystemClock::to_time_t;
	constexpr auto TimeUTC = STL::gmtime;

	// Time Local is defined in OSAL.

	// using PutTime = STL::put_time;
	template <class Elem> _NODISCARD 
	STL::_Timeobj<Elem, ptr<const CalendarDate>> PutTime(const CalendarDate& _date, ptr<const Elem> _elem)
	{
		return STL::_Timeobj<Elem, ptr<const CalendarDate>>(getPtr(_date), _elem);
	}
}

#pragma once


#include "LAL_Cpp_STL.hpp"
#include "LAL_FundamentalLimits.hpp"


namespace LAL
{
	template
	<
		IntMaxType Numerator, 
		IntMaxType Denominator = 1
	>
	using Ratio = STL::ratio<Numerator, Denominator>;

	template
	<
		class TermTypeA, 
		class TermTypeB
	>
	using RatioMultiply = STL::ratio_multiply<TermTypeA, TermTypeB>;

	using Kilo = STL::kilo;
	using Giga = STL::giga;

	using STL::floor;
	using STL::log2;
}

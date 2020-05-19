/*
Specification

Last Modified: 5/18/2020
*/



#pragma once



namespace Meta
{
	enum class BitAccuracy
	{
		_8__Bit            = 0,
		_16_Bit            = 1,
		_32_Bit            = 2,
		_64_Bit            = 3,
		_ExtendedPrecision = 4,
	};

	template<BitAccuracy _accuracyT>
	constexpr bool Is(BitAccuracy _accuracy)
	{
		return _accuracy == _accuracyT ? true : false;
	}
}

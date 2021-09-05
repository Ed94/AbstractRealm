#pragma once


#include "LAL_FundamentalTypes.hpp"


namespace TP_API
{
	#include "magic_enum.hpp"

	using namespace magic_enum;
}


namespace LAL
{
	template<typename EnumType, Where<IsEnum<EnumType>(), bool> = true>
	constexpr 
	auto nameOf(EnumType _type) 
	{
		return TP_API::enum_name(_type); 
	}
}

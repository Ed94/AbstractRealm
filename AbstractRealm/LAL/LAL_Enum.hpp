#pragma once



#include "magic_enum.hpp"



namespace LAL
{
	using namespace magic_enum;

	template<typename EnumType, Where<IsEnum<EnumType>(), bool> = true>
	constexpr auto nameOf(EnumType _type) { return enum_name(_type); }
}

#pragma once


#include "LAL_Reflection.hpp"


namespace LAL
{
	template<typename TextType>
	Where<IsStringRelated<TextType>(), RoCStr> ToRoCStr(TextType _type)
	{
		if constexpr (IsSameTypeCV<TextType, RoCStr>())
		{
			return _type;
		}

		if constexpr (IsSameTypeCV<TextType, StringView>())
		{
			return _type.data();
		}

		if constexpr (IsSameTypeCV<TextType, String>())
		{
			return _type.c_str();
		}
	}
}

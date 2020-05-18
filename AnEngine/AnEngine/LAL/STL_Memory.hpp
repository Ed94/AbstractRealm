/*
STL Memory

Last Modified: 5/18/2020
*/



#pragma once

#include "Cpp_STL.hpp"
#include "Types/STL_BasicTypes.hpp"


namespace LAL
{
	template<typename TypeS, typename TypeR>
	DataSize OffsetOf(TypeS _subject, TypeR _reference)
	{
		return offsetof(_subject, _reference);
	}
}

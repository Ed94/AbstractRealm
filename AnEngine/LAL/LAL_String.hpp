#pragma once



#include "LAL_Cpp_STL.hpp"



namespace LAL
{
	using String       = std::string      ;
	using String16     = std::wstring     ;
	using Stringstream = std::stringstream;
	
	inline String ToString(int                _val) { return std::to_string(_val); };
	inline String ToString(long               _val) { return std::to_string(_val); };
	inline String ToString(long long          _val) { return std::to_string(_val); };
	inline String ToString(unsigned           _val) { return std::to_string(_val); };
	inline String ToString(unsigned long      _val) { return std::to_string(_val); };
	inline String ToString(unsigned long long _val) { return std::to_string(_val); };
	inline String ToString(float              _val) { return std::to_string(_val); };
	inline String ToString(double             _val) { return std::to_string(_val); };
	inline String ToString(long double        _val) { return std::to_string(_val); };
}

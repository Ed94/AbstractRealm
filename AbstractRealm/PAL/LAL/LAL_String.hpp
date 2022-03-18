#pragma once


#include "LAL_Cpp_STL.hpp"


namespace LAL
{
	using String       = STL::string      ;
	using String16     = STL::wstring     ;
	using StringStream = STL::stringstream;
	using StringView   = STL::string_view;
	
	inline String ToString(bool               _val) { return _val ? "True" : "False"; }
	inline String ToString(int                _val) { return STL::to_string(_val); };
	inline String ToString(long               _val) { return STL::to_string(_val); };
	inline String ToString(long long          _val) { return STL::to_string(_val); };
	inline String ToString(unsigned           _val) { return STL::to_string(_val); };
	inline String ToString(unsigned long      _val) { return STL::to_string(_val); };
	inline String ToString(unsigned long long _val) { return STL::to_string(_val); };
	inline String ToString(float              _val) { return STL::to_string(_val); };
	inline String ToString(double             _val) { return STL::to_string(_val); };
	inline String ToString(long double        _val) { return STL::to_string(_val); };
}

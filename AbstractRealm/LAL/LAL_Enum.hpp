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
	constexpr auto nameOf(EnumType _type) { return TP_API::enum_name(_type); }

	//template<typename Representation>
	//struct DynamicEnum_API_Maker
	//{
	//	EnforceConstraint(IsInt<Representation>(), "Dynamic enum representation must be derived from int integral.");

	//	using Type = Representation;
	//};

	//#define DynamicEnum(_NAME, _TYPE) \
	//namespace _NAME\
	//{ \
	//	enum \
	//	{\
	//		Max = NumLimits<_TYPE>::max() - 1 \
	//	};\
	//	\
	//	using API = DynamicEnum_Type_Maker<_TYPE>;\
	//	\
	//}

	//#define DynamicEnum_Add(_NAME, _Entires) \
	//namespace _NAME##_Def \
	//{ \
	//	enum \
	//	{ \
	//		_Entires \
	//	}; \
	//} 
}

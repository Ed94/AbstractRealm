/*
C++ STL Reflection

*/



#pragma once

#include "LAL/Cpp_STL.hpp"



namespace LAL
{
	// Types

	using TypeHash = std::size_t;

	using TypeData = std::type_info;

	template<typename Type>
	using RemoveCV = std::remove_cv_t<Type>;

	template<typename Type>
	using RemoveRef = std::remove_reference_t<Type>;

	template<typename Type>
	using RawType = RemoveCV< RemoveRef<Type> >;

	template<typename Type, typename TypeRef>
	using SameTypeCV = std::is_same<Type, TypeRef>;

	template<typename Type, typename  TypeRef>
	using SameType = SameTypeCV< RawType<Type>, RawType<Type> >;

	template<typename Base, typename Derived>
	using OfClass = std::is_base_of<Base, Derived>;

	template<bool Constraint>
	using ShouldBe = typename std::enable_if<Constraint>::type;

	template<bool Constraint, typename ReturnType>
	using Where = typename std::enable_if<Constraint, ReturnType>::type;

	template<bool Constraint, typename ReturnTypeA, typename ReturnTypeB>
	using Choose = typename std::conditional<Constraint, ReturnTypeA, ReturnTypeB>::type;

	// Type Signage

	template<typename Type>
	using IsSignedType = std::is_signed<Type>;

	template<typename Type>
	using IsUnsignedType = std::is_unsigned<Type>;



	// Functions

	template<typename Type>
	constexpr bool IsSigned(void)
	{
		return IsSignedType<Type>::value;
	}

	template<typename Type>
	constexpr bool IsUnsigned(void)
	{
		return IsUnsignedType<Type>::value;
	}

	template<typename Base, typename Derived>
	constexpr bool IsOfClass(void)
	{
		return OfClass<Base, Derived>::value;
	}

	template<typename Type, typename TypeRef>
	constexpr bool IsSameType(void)
	{
		return SameType<Type, TypeRef>::value;
	}

	template<typename Type, typename TypeRef>
	constexpr bool IsSameTypeCV(void)
	{
		return SameTypeCV<Type, TypeRef>::value;
	}
}

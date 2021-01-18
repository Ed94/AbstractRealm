/*
C++ STL Reflection

*/



#pragma once



#include "LAL_Cpp_STL.hpp"



namespace LAL
{
	// Types

	using TypeHash = std::size_t;

	using TypeData = std::type_info;

	template<typename Type>
	using RemovePtr = typename std::remove_pointer<Type>::type;

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
	using ChooseEither = typename std::conditional<Constraint, ReturnTypeA, ReturnTypeB>::type;

	// Types as constants

	/*template<typename Type>
	using Something = std::integral_constant<Type>;*/

	// Type Signage

	template<typename Type>
	using IsClassType = std::is_class<Type>;

	template<typename Type>
	using IsEnumType = std::is_enum<Type>;

	template<typename Type>
	using IsSignedType = std::is_signed<Type>;

	template<typename Type>
	using IsUnsignedType = std::is_unsigned<Type>;

	// Type Primary Categories

	template<typename Type>
	using T_IsPointer= std::is_pointer<Type>;

	template<typename Type>
	using T_IsFunction = std::is_function<Type>;
	
	template<typename Type>
	using T_IsFunctionPtr = std::bool_constant< T_IsPointer<Type>::value && T_IsFunction< RemovePtr<Type>>::value >;



	// Functions

	template<typename Type>
	constexpr bool IsClass()
	{
		return IsClassType<Type>::value;
	}

	template<typename Type>
	constexpr bool IsEnum()
	{
		return IsEnumType<Type>::value;
	}

	template<typename Type>
	constexpr bool IsSigned()
	{
		return IsSignedType<Type>::value;
	}

	template<typename Type>
	constexpr bool IsUnsigned()
	{
		return IsUnsignedType<Type>::value;
	}

	template<typename Base, typename Derived>
	constexpr bool IsOfClass()
	{
		return OfClass<Base, Derived>::value;
	}

	template<typename Type, typename TypeRef>
	constexpr bool IsSameType()
	{
		return SameType<Type, TypeRef>::value;
	}

	template<typename Type, typename TypeRef>
	constexpr bool IsSameTypeCV()
	{
		return SameTypeCV<Type, TypeRef>::value;
	}

	// Type Primary Categories

	template<typename Type> constexpr
	bool IsPointer()
	{
		return T_IsPointer<Type>::value;
	}

	template<typename Type> constexpr
	bool IsFunction()
	{
		return T_IsFunction<Type>::value;
	}

	template<typename Type> constexpr
	bool IsFunctionPtr()
	{
		return T_IsFunctionPtr<Type>::value;
	}
}

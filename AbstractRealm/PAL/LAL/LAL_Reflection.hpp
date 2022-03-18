/*
C++ STL Reflection

*/


#pragma once

#include "LAL_Cpp_STL.hpp"
#include "LAL_Types.hpp"
#include "LAL_CharacterTypes.hpp"
#include "LAL_String.hpp"


namespace LAL
{
	// Types

	template<bool Value>
	using TBool = STL::bool_constant<Value>;

	using FalseType = STL::false_type;
	using TrueType  = STL::true_type;

	using TypeHash = STL::size_t;
	using TypeData = STL::type_info;

	template<typename Type>
	using RemovePtr = typename STL::remove_pointer<Type>::type;

	template<typename Type>
	using RemoveCV = STL::remove_cv_t<Type>;

	template<typename Type>
	using RemoveRef = STL::remove_reference_t<Type>;

	template<typename Type>
	using RawType = RemoveCV< RemoveRef<Type> >;

	template<typename Type, typename TypeRef>
	using SameTypeCV = STL::is_same<Type, TypeRef>;

	template<typename Type, typename  TypeRef>
	using SameType = SameTypeCV< RawType<Type>, RawType<Type> >;

	template<typename Base, typename Derived>
	using OfClass = STL::is_base_of<Base, Derived>;

	template<bool Constraint>
	using ShouldBe = typename STL::enable_if<Constraint>::type;

	template<bool Constraint, typename ReturnType>
	using Where = typename STL::enable_if<Constraint, ReturnType>::type;

	template<bool Constraint, typename ReturnTypeA, typename ReturnTypeB>
	using ChooseEither = typename STL::conditional<Constraint, ReturnTypeA, ReturnTypeB>::type;

	// Types as constants

	/*template<typename Type>
	using Something = STL::integral_constant<Type>;*/

	// ABI

	template<typename Type>
	using IsTrivialType = STL::is_trivial<Type>;

	template<typename Type>
	using IsStandardLayoutType = STL::is_standard_layout<Type>;

	// Type Signage

	template<typename Type>
	using IsClassType = STL::is_class<Type>;

	template<typename Type>
	using IsEnumType = STL::is_enum<Type>;

	template<typename Type>
	using IsSignedType = STL::is_signed<Type>;

	template<typename Type>
	using IsUnsignedType = STL::is_unsigned<Type>;

	// Composite Type Categories

	template<typename Type>
	using IsArithmeticType = STL::is_arithmetic<Type>;

	template<typename Type>
	using IsReferenceType = STL::is_reference<Type>;

	// Type Primary Categories

	template<typename Type>
	using IsIntType = STL::is_integral<Type>;

	template<typename Type>
	using IsFloatType = STL::is_floating_point<Type>;

	template<typename Type>
	using IsPointerType = STL::is_pointer<Type>;

	template<typename Type>
	using IsFunctionType = STL::is_function<Type>;
	
	template<typename Type>
	using IsFunctionPtrType = TBool< IsPointerType<Type>::value && IsFunctionType< RemovePtr<Type>>::value >;


	// Functions

	template<typename Type>
	constexpr 
	bool IsClass()
	{
		return IsClassType<Type>::value;
	}

	template<typename Type>
	constexpr
	bool IsEnum()
	{
		return IsEnumType<Type>::value;
	}

	template<typename Type>
	constexpr 
	bool IsSigned()
	{
		return IsSignedType<Type>::value;
	}

	template<typename Type>
	constexpr 
	bool IsUnsigned()
	{
		return IsUnsignedType<Type>::value;
	}

	template<typename Type>
	constexpr 
	bool IsWithinDataModelSize()
	{
		return sizeof(Type) <= sizeof(uDM);
	}

	// ABI

	template<typename Type>
	constexpr
	bool Is_POD()
	{
		return IsTrivialType<Type>::value && IsStandardLayoutType<Type>::value;
	}

	// Type Signage

	template<typename Base, typename Derived>
	constexpr 
	bool IsOfClass()
	{
		return OfClass<Base, Derived>::value;
	}

	template<typename Type, typename TypeRef>
	constexpr 
	bool IsSameType()
	{
		return SameType<Type, TypeRef>::value;
	}

	template<typename Type, typename TypeRef>
	constexpr 
	bool IsSameTypeCV()
	{
		return SameTypeCV<Type, TypeRef>::value;
	}

	// Composite Type Categories

	template<typename Type> 
	constexpr
	bool IsArithmetic()
	{
		return IsArithmeticType<Type>::value;
	}

	template<typename Type> 
	constexpr
	bool IsReference()
	{
		return IsReferenceType<Type>::value;
	}

	// Type Primary Categories

	template<typename Type>
	constexpr
	bool IsInt()
	{
		return IsIntType<Type>::value;
	}

	template<typename Type>
	constexpr
	bool IsFloat()
	{
		return IsFloatType<Type>::value;
	}

	template<typename Type> 
	constexpr
	bool IsPointer()
	{
		return IsPointerType<Type>::value;
	}

	template<typename Type> 
	constexpr
	bool IsFunction()
	{
		return IsFunctionType<Type>::value;
	}

	template<typename Type> 
	constexpr
	bool IsFunctionPtr()
	{
		return IsFunctionPtrType<Type>::value;
	}

	// String

	template<typename Type> 
	constexpr 
	bool IsStringRelated()
	{
		using Raw = RawType<Type>;

		return 
			IsSameType<Type, RoCStr    >() &&
			IsSameType<Raw , String    >() &&
			IsSameType<Raw , StringView>();
	}

	// Type Traits

	#define HasMemberSymbol(__MEMBER_SYMBOL)\
	decltype(static_cast<void>(__MEMBER_SYMBOL))

	#define HasMemberAddress(__MEMBER_SYMBOL)\
	__MEMBER_SYMBOL != nullptr ? true : false
	

	// Nameof

	// Obtains name of type, reference and cv-qualifiers are ignored.
	//template <typename T>
	//[[nodiscard]] constexpr 
	//StringView nameof_type() noexcept {
	//	static_assert(detail::nameof_type_supported<T>::value, "nameof::nameof_type unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
	//	using U = detail::identity<detail::remove_cvref_t<T>>;
	//	constexpr string_view name = detail::type_name_v<U>;
	//	static_assert(name.size() > 0, "Type does not have a name.");

	//	return name;
	//}

	template<typename Type>
	constexpr auto NameOfType = nameof::nameof_type<Type>;
}

// NameOf

// Obtains name of variable, function, macro.
#define NameOf(...) \
[&]() \
constexpr noexcept \
{ \
  ::LAL::VoidType<decltype(__VA_ARGS__)>(); \
\
  constexpr auto _name = ::nameof::detail::pretty_name(#__VA_ARGS__); \
\
  static_assert(_name.size() > 0, "Expression does not have a name."); \
\
  constexpr auto _size   = _name.size(); \
  constexpr auto _nameof = ::nameof::cstring<_size>{_name}; \
\
  return _nameof; \
} \
() \


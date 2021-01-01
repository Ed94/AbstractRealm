/*
STL Memory

Last Modified: 5/18/2020
*/



#pragma once



// Engine
#include "LAL_Cpp_STL.hpp"
#include "LAL_Types.hpp"



namespace LAL
{
	#define LAL_UseMemorySpecifiers

#ifdef LAL_UseMemorySpecifiers

	/*
	Macro Stack scope specifier.
	*/
	#define Stack() \

	// Macro Static Variable scope specifier.
	#define StaticData() \

	// Macro Data Segment scope specifier.
	#define Data() \

	// Macro BSS Segment scope specifier.
	#define BSS() \

	enum class EHeap
	{
		Allocate,
		Free
	};

	// Macro Heap manipulation specifier.
	#define Heap() \

	/*
	Stack Scope Type Specifier

	Specifies to allocate the object within the stack, and live until the end of its block scope.
	(Explicit indication. Does not do anything)

	Template version of the C-Assist Stack macro.
	*/
	template<typename Type> using stack = Type;

	/**
	Static Data Scope Type Specifier

	Specifies that the object will be allocated within a readonly data segment of memory.
	(Explicit indication. Does not do anything)
	*/
	template<typename Type> using roData = Type;
	
	/*
	Data Segment Scope Type Specifier

	Specifies that object will be allocated the Data segment or initialization specified.
	(Explicit indication. Does not do anything)
	*/
	template<typename Type> using data = Type;
	
	/*
	BSS Template Scope Type Specifier

	Specifies that the object will be allocated the BSS segment or uninitialized data.
	(Explicit indication. Does not do anything)
	*/
	template<typename Type> using bss = Type;

#endif

	/*
	Pointer Template

	Provides a more explicit method for declaring a pointer

	It also sets const order to default template's order instead of pointer's unique order.
	*/
	template<typename Type> using ptr = Type*;

	/*
	Dereference operator
	*/
	template<typename Type> 
	inline Type& dref(const ptr<Type> _ptr)
	{
		return *_ptr;
	}

	/*
	Address operator
	*/
	template<typename Type>
	auto getAddress(Type& _obj)
	{
		return &_obj;
	}

	template<typename FunctionType>
	auto getFuncAddress(FunctionType& _obj)
	{
		return NULL;
	}

	using std::move;

	template<typename ReturnType, typename... ParameterTypes>
	using FPtr = ReturnType(*)(ParameterTypes...);

	// Clang-CL does not like window's offsetof macro.
	template <typename StructType, typename StructMemberType>
	constexpr WordSize TOffsetOf(StructMemberType StructType::* _member)
	{
		return reinterpret_cast<WordSize>(getAddress((ptr<StructType>(0)->*_member)));
	}

	#define OffsetOf(_MEMBEER) TOffsetOf(&_MEMBEER);
}

/*
STL Memory

Last Modified: 5/18/2020
*/



#pragma once

#include "Cpp_STL.hpp"
#include "Types/STL_BasicTypes.hpp"


namespace LAL
{
	/*
	Macro Stack Scope Specifier.
	*/
	#define Stack(...) \
	__VA_ARGS__

	// Macro Data Segment Scope Specifier
	#define Data(...) \
	__VA_ARGS__

	#define BSS(...) \
	__VA_ARGS__

	#define Heap(...) \
	__VA_ARGS__

	/*
	Stack Scope Type Specifier

	Specifies to allocate the object within the stack, and live until the end of its block scope.
	(Explicit indication. Does not do anything)

	Template version of the C-Assist Stack macro.
	*/
	template<typename Type> using stack = Type;
	
	/*
	Data Segment Scope Type Specifier

	Specifies to allocate the object within the Data segment or initialization specified.
	(Explicit indication. Does not do anything)
	*/
	template<typename Type> using data = Type;
	
	/*
	BSS Template Scope Type Specifier

	Specifies to allocate the object within the BSS segment or uninitialized data.
	(Explicit indication. Does not do anything)
	*/
	template<typename Type> using bss = Type;

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
	Type& dref(const ptr<Type> _ptr)
	{
		return *_ptr;
	}

	#define M_ADDRESS(_obj) \
	&_obj

	/*
	Address operator
	*/
	template<typename Type>
	auto getAddress(Type& _obj) -> ptr<Type>
	{
		return M_ADDRESS(_obj);
	}

	template<typename TypeS, typename TypeR>
	DataSize OffsetOf(TypeS _subject, TypeR _reference)
	{
		return offsetof(_subject, _reference);
	}
}

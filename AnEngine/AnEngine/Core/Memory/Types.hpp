/*
Memory Types

Last Modified: 5/18/2020
*/



#pragma once



#include "LAL.hpp"



namespace Memory
{
	/*
	Stack Type Specifier

	Specifies to allocate the object within the stack, and live until the end of its block scope.
	(Explicit indication. Does not do anything)

	Template version of the C-Assist Stack macro.
	*/
	template<typename Type> using stack = Type;
	
	/*
	Data Segment Type Specifier

	Specifies to allocate the object within the Data segment or initialization specified.
	(Explicit indication. Does not do anything)
	*/
	template<typename Type> using data = Type;
	
	/*
	BSS Template Type Specifier

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
		return *_ptr
	}

	/*
	Address operator
	*/
	template<typename Type>
	ptr<Type> getAddrs(Type& _obj)
	{
		return &_instance;
	}

	
}

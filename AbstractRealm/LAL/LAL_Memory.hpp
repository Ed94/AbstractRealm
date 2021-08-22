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
#ifdef LAL_UseMemorySpecifiers

	/*
	Macro Stack scope specifier.
	*/
	#define Stack() \

	// Macro Data Segment scope specifier.
	#define Data() \

	// Macro BSS Segment scope specifier.
	#define BSS() \

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

	template<typename Type> using ref = Type&;

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
	auto getPtr(Type& _obj)
	{
		return &_obj;
	}

	template<typename Type>
	auto getPtr(Type* _objPtr)
	{
		return &_objPtr;
	}

	template<typename FunctionType>
	auto getFnPtr(FunctionType& /*_obj*/)
	{
		return NULL;
	}

	using STL::move;

	template<typename ReturnType, typename... ParameterTypes>
	using fnPtr = ReturnType(*)(ParameterTypes...);

	// Clang-CL does not like window's offsetof macro.
	template <typename StructType, typename StructMemberType>
	constexpr uDM TOffsetOf(StructMemberType StructType::* _member)
	{
		return reinterpret_cast<uDM>(getPtr((ptr<StructType>(0)->*_member)));
	}

	#define OffsetOf(_MEMBEER) TOffsetOf(&_MEMBEER);

	template<typename Type>
	ptr<void> FormatByFill(ptr<Type> _memoryAddress, Type& _fillValue, uDM _count)
	{
		//return memset(_memoryAddress, _fillValue, _count * sizeof(Type));
		return STL::fill_n(_memoryAddress, _count, _fillValue);
	}

	template<typename Type>
	ptr<void> FormatWithData(ptr<Type> _memoryAddress, ptr<const Type> _dataSource, uDM _count)
	{
		//return memcpy(_memoryAddress, _dataSource, _count * sizeof(Type));
		return STL::copy_n(_dataSource, _count, _memoryAddress);
	}
}

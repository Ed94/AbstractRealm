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
	/*
	Macro Stack scope specifier.
	*/
	#define Stack(...) \
	__VA_ARGS__

	// Macro Static Variable scope specifier.
	#define StaticData(...) \
	__VA_ARGS__

	// Macro Data Segment scope specifier.
	#define Data(...) \
	__VA_ARGS__

	// Macro BSS Segment scope specifier.
	#define BSS(...) \
	__VA_ARGS__

	enum class EHeap
	{
		Allocate,
		Free
	};

	// Macro Heap manipulation specifier.
	#define Heap(...) \
	__VA_ARGS__


	// Heap Tracking stuff. Not using for now

//#define LAL_TrackHeap

//#ifdef LAL_TrackHeap
//
//	DataSize Internal_Heap_AllocationsLeft();
//
//	void ProcessHeapAction(EHeap _action, DataSize _objectsAllocated);
//
//	// Macro Heap manipulation specifier.
//	#define Heap(_EHEAP_, _NumObjects_, ...) \
//	ProcessHeapAction(_EHEAP_, _NumObjects_); \
//	__VA_ARGS__
//
//	#define Heap_AllocationsLeft() \
//	Internal_Heap_AllocationsLeft()
//
//#else
//
//	// Macro Heap manipulation specifier.
//	#define Heap(_EHEAP_, _NumObj_, ...) \
//	__VA_ARGS__
//
//	#define Heap_AllocationsLeft() \
//	0
//
//#endif
	

	/*
	Stack Scope Type Specifier

	Specifies to allocate the object within the stack, and live until the end of its block scope.
	(Explicit indication. Does not do anything)

	Template version of the C-Assist Stack macro.
	*/
	template<typename Type> using stack = Type;

	/**
	Static Data Scope Type Specifier

	Specifies that the object will be allocated within a static segment of memory.
	(Explicit indication. Does not do anything)
	*/
	template<typename Type> using staticData = Type;
	
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

	#define M_ADDRESS(_obj) \
	&(_obj)

	/*
	Address operator
	*/
	template<typename Type>
	auto getAddress(Type& _obj)
	{
		return M_ADDRESS(_obj);
	}

	template<typename FunctionType>
	auto getFuncAddress(FunctionType& _obj)
	{
		return NULL;
	}

	using std::move;

	template<typename TypeS, typename TypeR>
	DataSize OffsetOf(TypeS _subject, TypeR _reference)
	{
		return offsetof(_subject, _reference);
	}

	template<typename ReturnType, typename... ParameterTypes>
	using FPtr = ReturnType(*)(ParameterTypes...);
}

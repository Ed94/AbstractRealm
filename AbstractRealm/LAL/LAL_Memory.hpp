/*
STL Memory

Last Modified: 5/18/2020
*/


#pragma once


#include "LAL_Cpp_STL.hpp"
#include "LAL_Declarations.hpp"
#include "LAL_Types.hpp"


namespace LAL
{
	/*
	Reference Macro
	*/
	//#define ref(_Type) 

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
	ForceInline
	Type& dref(const ptr<Type> _ptr)
	{
		return *_ptr;
	}

	/*
	Address operator
	*/
	template<typename Type>
	ForceInline
	auto getPtr(Type& _obj)
	{
		return &_obj;
	}

	template<typename Type>
	ForceInline
	Type** getPtr(Type* _obj)
	{
		return &_obj;
	}


	template<typename FunctionType>
	ForceInline
	auto getFnPtr(FunctionType& /*_obj*/)
	{
		return nullptr;
	}

	using STL::forward;
	using STL::move;

	template <typename ReturnType, typename... ParameterTypes>
	using fnPtr = ReturnType(*)(ParameterTypes...);

	// Clang-CL does not like window's offsetof macro.
	template <typename StructType, typename StructMemberType>
	constexpr 
	uDM offsetOf(StructMemberType StructType::* _member)
	{
		return reinterpret_cast<uDM>(getPtr((ptr<StructType>(0)->*_member)));
	}

	#define OffsetOf(_MEMBEER) offsetOf(&_MEMBEER);

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

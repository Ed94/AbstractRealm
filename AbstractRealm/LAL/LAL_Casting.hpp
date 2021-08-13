/*
Casting

Last Modified: 5/18/2020
*/


#pragma once
#include "LAL_Declarations.hpp"


namespace LAL
{
	/*
	Constant Cast (Direct)
	*/
	template<typename Type>
	ForceInline 
	Type CVCast(const Type _obj)
	{
		return const_cast<Type>(_obj);
	}

	/*
	Constant Cast (Pointer)
	*/
	template<typename Type>
	ForceInline 
	Type* CVCast(const Type* _obj)
	{
		return const_cast<Type*>(_obj);
	}

	/*
	Dynamic Cast (Direct)
	*/
	template<typename Derived, typename Base>
	ForceInline 
	Derived DCast(const Base _obj)
	{
		return dynamic_cast<Derived>(_obj);
	}

	/*
	Dynamic Cast (Pointer)
	*/
	template<typename Derived, typename Base>
	ForceInline 
	Derived* DCast(Base* const _ptr)
	{
		return dynamic_cast< Derived* >(_ptr);
	}

	/*
	Reinterpret Cast (Direct)
	*/
	template<typename Derived, typename Base>
	ForceInline 
	Derived RCast(Base _obj)
	{
		return reinterpret_cast<Derived>(_obj);
	}

	/*
	Reinterpret Cast (Pointer)
	*/
	template<typename Derived, typename Base> 
	ForceInline 
	Derived* RCast(Base* _ptr)
	{
		return reinterpret_cast< Derived* >(_ptr);
	}

	//template<typename Object>
	

	/*
	Static Cast (Direct)
	*/
	template<typename Derived, typename Base> 
	ForceInline 
	Derived SCast(const Base _obj)
	{
		return static_cast<Derived>(_obj);
	}

	/*
	Static Cast (Pointer)
	*/
	template<typename Derived, typename Base> 
	ForceInline 
	Derived* SCast(const Base* _ptr)
	{
		return static_cast< Derived* >(_ptr);
	}
}

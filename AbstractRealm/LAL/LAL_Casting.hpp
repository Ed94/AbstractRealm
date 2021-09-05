/*
Casting

Last Modified: 5/18/2020
*/


#pragma once
#include "LAL_Declarations.hpp"
#include "LAL_Reflection.hpp"


namespace LAL
{
	/*
	Constant Cast(Ref)
	*/
	template<typename Type>
	constexpr
	Type& CVCast(Type& _obj)
	{
		return const_cast<Type& >(_obj);
	}

	/*
	Constant Cast (Pointer)
	*/
	template<typename Type>
	constexpr
	Type* CVCast(Type* _obj)
	{
		return const_cast<Type* >(_obj);
	}

	/*
	Dynamic Cast (Direct)
	*/
	//template<typename Derived, typename Base>
	//ForceInline Where<!IsReference<Base>(),
	//Derived> DCast(Base _obj)
	//{
	//	return dynamic_cast< Derived >(_obj);
	//}

	/*
	Dynamic Cast (Direct)
	*/
	template<typename Derived, typename Base>
	constexpr
	Derived& DCast(Base& _obj)
	{
		return dynamic_cast< Derived& >(_obj);
	}

	/*
	Dynamic Cast (Pointer)
	*/
	template<typename Derived, typename Base>
	constexpr
	Derived* DCast(Base* _ptr)
	{
		return dynamic_cast< Derived* >(_ptr);
	}

	/*
	Reinterpret Cast (Pointer)
	*/
	template<typename Derived, typename Base> 
	constexpr
	Derived& RCast(Base& _ptr)
	{
		return reinterpret_cast< Derived& >(_ptr);
	}

	/*
	Reinterpret Cast (Pointer)
	*/
	template<typename Derived, typename Base> 
	constexpr
	Derived* RCast(Base* _ptr)
	{
		return reinterpret_cast< Derived* >(_ptr);
	}

	/*
	Static Cast (Reference)
	*/
	template<typename Derived, typename Base>
	constexpr 
	Where<IsWithinDataModelSize<Base>(),
	Derived> SCast(Base _obj)
	{
		return static_cast<Derived>(_obj);
	}

	/*
	Static Cast (Reference)
	*/
	template<typename Derived, typename Base> 
	constexpr
	Where<! IsWithinDataModelSize<Base>(),
	Derived&>  SCast(Base& _obj)
	{
		return static_cast< Derived& >(_obj);
	}

	/*
	Static Cast (Pointer)
	*/
	template<typename Derived, typename Base> 
	constexpr 
	Derived* SCast(Base* _ptr)
	{
		return static_cast< Derived* >(_ptr);
	}
}

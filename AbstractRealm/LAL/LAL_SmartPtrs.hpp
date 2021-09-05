/*
STL Pointers
*/


#pragma once


#include "LAL_Cpp_STL.hpp"


namespace LAL
{
	template<typename Type> using UPtr = STL::unique_ptr<Type>;
	template<typename Type> using SPtr = STL::shared_ptr<Type>;
	template<typename Type> using WPtr = STL::weak_ptr  <Type>;

	template<typename Type, class... ArgumentsTypes>
	UPtr<Type> MakeUnique(ArgumentsTypes&&... _arguments)
	{
		return STL::make_unique<Type>(_arguments...);
	}

	template<typename Type, class... ArgumentsTypes>
	SPtr<Type> MakeShared(ArgumentsTypes&&... _arguments)
	{
		return STL::make_shared<Type>(_arguments...);
	}
}

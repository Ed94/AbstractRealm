/*
STL Pointers
*/



#pragma once



#include "LAL_Cpp_STL.hpp"



namespace LAL
{
	template<typename Type> using UPtr = std::unique_ptr<Type>;
	template<typename Type> using SPtr = std::shared_ptr<Type>;
	template<typename Type> using WPtr = std::weak_ptr  <Type>;

	template<typename Type, class... ArgumentsTypes>
	UPtr<Type> MakeUPtr(ArgumentsTypes&&... _arguments)
	{
		return std::make_unique<Type>(_arguments);
	}

	template<typename Type, class... ArgumentsTypes>
	SPtr<Type> MakeSPtr(ArgumentsTypes&&... _arguments)
	{
		return std::make_shared<Type>(_arguments);
	}
}

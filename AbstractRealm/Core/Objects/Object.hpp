/*

*/


#pragma once


#include "Objects_Backend.hpp"
#include "Indirection.hpp"


namespace Core
{
	class Type;

	/*
	Object
	
	Supports runtime reflection.
	*/
	class Object
	{
	public:			

		Object();
			
		ObjPtr Ptr()
		{
			return this;
		}

	protected:

		ptr<Type> type;

		String name;
	};


	/*
	Object

	Supports compile-time reflection
	*/
	template<typename TypeDef>
	class TObject
	{
	public:

		using Type = TypeDef;

		TPtr<Type> Ptr()
		{
			return this;
		}

	protected:

		static constexpr auto name = nameof::nameof_type<TObject<TypeDef>>();
	};
}



#pragma once


#include "Memory_Backend.hpp"
#include "Objects/Objects.hpp"


namespace Memory
{
	using CoreObject = Core::Object;
	using CoreObjPtr = Core::ObjPtr;

	/*
	Memory representation of a CoreObject.
	*/
	class Object
	{
	public:

		Object();

		

	protected:

		u32 allocatorIndex;

		ptr<CoreObject> instance;
	};

	/*
	Memory representation of any object using the core memory manager.
	*/
	template<typename Type>
	class TObject
	{
	public:

	protected:

		u32 allocatorIndex;

		ptr<Type> instance;
	};

}
